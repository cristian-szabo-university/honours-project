/**
    Device.cpp

    Project Name: HashCracker

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#include "Config.hpp"

#include "OpenCL/Device.hpp"

#include "OpenCL/KernelParam.hpp"
#include "OpenCL/KernelBuffer.hpp"
#include "OpenCL/DeviceMemory.hpp"

#include "Core/Platform.hpp"

namespace HonoursProject
{
    std::uint32_t Device::next_device_id = 1;

    Device::Device() : ready(false), used_global_mem_size(0), used_local_mem_size(0)
    {
        id = next_device_id++;
    }

    Device::~Device()
    {
    }

    bool Device::create(cl::Device device, cl_context_properties context_props)
    {
        if (ready)
        {
            return false;
        }

        cl_bool available = device.getInfo<CL_DEVICE_AVAILABLE>();

        if (available == CL_FALSE)
        {
            return false;
        }

        cl_bool endian_little = device.getInfo<CL_DEVICE_ENDIAN_LITTLE>();

        if (endian_little == CL_FALSE)
        {
            return false;
        }

        cl_bool compiler_available = device.getInfo<CL_DEVICE_COMPILER_AVAILABLE>();

        if (compiler_available == CL_FALSE)
        {
            return false;
        }

        cl_device_exec_capabilities execution_capabilities = device.getInfo<CL_DEVICE_EXECUTION_CAPABILITIES>();

        if ((execution_capabilities & CL_EXEC_KERNEL) == 0)
        {
            return false;
        }

        std::string opencl_version = device.getInfo<CL_DEVICE_OPENCL_C_VERSION>();

        if (opencl_version[9] == Platform::OPENCL_COMPILER_MAJOR_VERSION && opencl_version[11] < Platform::OPENCL_COMPILER_MINOR_VERSION)
        {
            return false;
        }

        std::string vendor_name = device.getInfo<CL_DEVICE_VENDOR>();

        if (vendor_name.find("NVIDIA Corporation") != std::string::npos)
        {
            this->vendor = Vendor::NVIDIA;
        }
        else if (vendor_name.find("Intel(R) Corporation") != std::string::npos)
        {
            this->vendor = Vendor::Intel;
        }
        else if (vendor_name.find("Advanced Micro Devices, Inc.") != std::string::npos)
        {
            this->vendor = Vendor::AMD;
        }
        else
        {
            this->vendor = Vendor::Invalid;
        }

        cl_device_type type = device.getInfo<CL_DEVICE_TYPE>();

        if (type & CL_DEVICE_TYPE_CPU)
        {
            this->type = Type::CPU;
        }
        else if (type & CL_DEVICE_TYPE_GPU)
        {
            this->type = Type::GPU;
        }
        else
        {
            this->type = Type::Custom;
        }

        if (this->vendor == Vendor::Intel && this->type == Type::GPU)
        {
            return false;
        }

        handle = device;

        context = cl::Context(handle, &context_props);

        std::string device_name = handle.getInfo<CL_DEVICE_NAME>();

        this->name = device_name;

        cl_uint max_compute_units = handle.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();

        this->max_compute_units = max_compute_units;

        cl_ulong max_mem_alloc_size = handle.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();

        // Limit max memory to up to 2GB
        this->max_mem_alloc_size = std::min(max_mem_alloc_size, static_cast<cl_ulong>(0x7fffffff));

        cl_ulong global_mem_size = handle.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();

        this->total_global_mem_size = global_mem_size;

        cl_ulong local_mem_size = handle.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();

        this->total_local_mem_size = local_mem_size;

        std::size_t max_work_group_size = handle.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

        if (this->type == Type::CPU)
        {
            this->max_work_group_size = 1;
        }
        else
        {
            this->max_work_group_size = std::min(max_work_group_size, KernelPlatform::MAX_KERNEL_WORK_GROUP);
        }

        cl_ulong max_const_buffer_size = handle.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>();

        this->max_const_buffer_size = max_const_buffer_size;

        cl_uint vector_width = handle.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT>();

        this->vector_width = vector_width;

        ready = !ready;

        return true;
    }

    bool Device::destroy()
    {
        if (!ready)
        {
            return false;
        }

        ready = !ready;

        return true;
    }

    std::uint32_t Device::getId()
    {
        return id;
    }

    cl::Device Device::getHandle()
    {
        return handle;
    }

    cl::Context Device::getContext()
    {
        return context;
    }

    std::string Device::getName()
    {
        return name;
    }

    Device::Vendor Device::getVendor()
    {
        return vendor;
    }

    Device::Type Device::getType()
    {
        return type;
    }

    std::size_t Device::getMaxComputeUnits()
    {
        return max_compute_units;
    }

    std::size_t Device::getVectorWidth()
    {
        return vector_width;
    }

    std::size_t Device::getMaxWorkGroupSize()
    {
        return max_work_group_size;
    }

    std::uint64_t Device::getMaxMemAllocSize()
    {
        return max_mem_alloc_size;
    }

    std::shared_ptr<DeviceMemory> Device::createMemory(std::shared_ptr<KernelParam> param, std::size_t size)
    {
        std::shared_ptr<DeviceMemory> result = std::make_shared<DeviceMemory>(param);

        if (size > max_mem_alloc_size)
        {
            return std::shared_ptr<DeviceMemory>();
        }

        bool has_mem_req = false;

        switch (param->getAddressQualifier())
        {
        case KernelParam::AddressQualifier::Global:
            has_mem_req = ((used_global_mem_size + size) < total_global_mem_size);
            break;

        case KernelParam::AddressQualifier::Local:
            has_mem_req = ((used_local_mem_size + size) < total_local_mem_size);
            break;

        case KernelParam::AddressQualifier::Constant:
            has_mem_req = size < max_const_buffer_size;
            break;
        }

        if (!has_mem_req)
        {
            return result;
        }

        DeviceMemory::AddressFlag address_flag;

        switch (param->getAddressQualifier())
        {
        case KernelParam::AddressQualifier::Global:
            used_global_mem_size += size;
            address_flag = DeviceMemory::AddressFlag::Global;
            break;

        case KernelParam::AddressQualifier::Local:
            used_local_mem_size += size;
            address_flag = DeviceMemory::AddressFlag::Local;
            break;
        }

        result = std::make_shared<DeviceMemory>(param);

        if (!result->create(size, address_flag))
        {
            return std::shared_ptr<DeviceMemory>();
        }

        return result;
    }

    bool Device::destroyMemory(std::shared_ptr<DeviceMemory> mem)
    {
        if (!mem)
        {
            return true;
        }

        switch (mem->getAddressFlag())
        {
        case DeviceMemory::AddressFlag::Global:
            used_global_mem_size -= mem->getSize();
            break;

        case DeviceMemory::AddressFlag::Local:
            used_local_mem_size -= mem->getSize();
            break;
        }

        return true;
    }

    std::uint64_t Device::getTotalGlobalMemSize()
    {
        return total_global_mem_size;
    }

    std::uint64_t Device::getUsedGlobalMemSize()
    {
        return used_global_mem_size;
    }

    std::uint64_t Device::getTotalLocalMemSize()
    {
        return total_local_mem_size;
    }

    std::uint64_t Device::getUsedLocalMemSize()
    {
        return used_local_mem_size;
    }
}
