#include "Config.hpp"

#include "OpenCL/Device.hpp"

#include "OpenCL/KernelParam.hpp"
#include "OpenCL/KernelBuffer.hpp"
#include "OpenCL/DeviceMemory.hpp"

#include "Core/Platform.hpp"

namespace HonoursProject
{
    Device::Device(cl::Device device, cl::Context context)
        : handle(device), context(context), used_global_mem_size(0), used_local_mem_size(0)
    {
        cl_int cl_error = CL_SUCCESS;

        std::string device_name = device.getInfo<CL_DEVICE_NAME>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

        this->name = device_name;

        std::string vendor_name = device.getInfo<CL_DEVICE_VENDOR>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

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

        cl_device_type type = device.getInfo<CL_DEVICE_TYPE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

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

        cl_uint max_compute_units = device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

        this->max_compute_units = max_compute_units;

        cl_ulong max_mem_alloc_size = device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

        // Limit max memory to up to 2GB
        this->max_mem_alloc_size = std::min(max_mem_alloc_size, static_cast<cl_ulong>(0x7fffffff));

        cl_ulong global_mem_size = device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()\n");
        }

        this->total_global_mem_size = global_mem_size;

        cl_ulong local_mem_size = device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()\n");
        }

        this->total_local_mem_size = local_mem_size;

        std::size_t max_work_group_size = device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()\n");
        }

        this->max_work_group_size = std::min(max_work_group_size, KernelPlatform::MAX_KERNEL_WORK_GROUP);

        cl_ulong max_const_buffer_size = device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device::getInfo()\n");
        }

        this->max_const_buffer_size = max_const_buffer_size;

        cl_uint vector_width = device.getInfo<CL_DEVICE_NATIVE_VECTOR_WIDTH_INT>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device::getInfo()\n");
        }

        this->vector_width = vector_width;
    }

    Device::~Device()
    {
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

    std::shared_ptr<DeviceMemory> Device::createMemory(std::size_t size, std::shared_ptr<KernelParam> param)
    {
        std::shared_ptr<DeviceMemory> result;

        if (size > max_mem_alloc_size)
        {
            return result;
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

        switch (param->getAddressQualifier())
        {
        case KernelParam::AddressQualifier::Global:
            used_global_mem_size += size;
            break;

        case KernelParam::AddressQualifier::Local:
            used_local_mem_size += size;
            break;
        }

        return DeviceMemory::Create(size, param, shared_from_this());
    }

    void Device::destroyMemory(std::shared_ptr<DeviceMemory> buffer)
    {
        switch (buffer->getAddressFlag())
        {
        case DeviceMemory::AddressFlag::Global:
            used_global_mem_size -= buffer->getSize();
            break;

        case DeviceMemory::AddressFlag::Local:
            used_local_mem_size -= buffer->getSize();
            break;
        }
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

    std::shared_ptr<Device> Device::Create(cl::Device device, cl_context_properties context_props)
    {
        cl_int cl_error = CL_SUCCESS;

        cl_bool available = device.getInfo<CL_DEVICE_AVAILABLE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

        if (available == CL_FALSE)
        {
            return std::shared_ptr<Device>();
        }

        cl_bool endian_little = device.getInfo<CL_DEVICE_ENDIAN_LITTLE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

        if (endian_little == CL_FALSE)
        {
            return std::shared_ptr<Device>();
        }

        cl_bool compiler_available = device.getInfo<CL_DEVICE_COMPILER_AVAILABLE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

        if (compiler_available == CL_FALSE)
        {
            return std::shared_ptr<Device>();
        }

        cl_device_exec_capabilities execution_capabilities = device.getInfo<CL_DEVICE_EXECUTION_CAPABILITIES>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

        if ((execution_capabilities & CL_EXEC_KERNEL) == 0)
        {
            return std::shared_ptr<Device>();
        }

        std::string opencl_version = device.getInfo<CL_DEVICE_OPENCL_C_VERSION>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: device.getInfo()!\n");
        }

        if (opencl_version[9] == Platform::OPENCL_COMPILER_MAJOR_VERSION && opencl_version[11] < Platform::OPENCL_COMPILER_MINOR_VERSION)
        {
            return std::shared_ptr<Device>();
        }

        cl::Context context = cl::Context(device, &context_props, nullptr, nullptr, &cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: kernel::getInfo()\n");
        }

        return std::make_shared<Device>(device, context);
    }

    std::vector<std::shared_ptr<Device>> Device::Create(cl::Platform platform, cl_device_type device_filter, cl_context_properties context_props)
    {
        cl_int cl_error = CL_SUCCESS;

        std::vector<std::shared_ptr<Device>> result;

        std::vector<cl::Device> devices;

        cl_error = platform.getDevices(device_filter, &devices);

        if (cl_error == CL_DEVICE_NOT_FOUND)
        {
            return result;
        }

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: platform::getDevices()\n");
        }

        for (std::size_t device_pos = 0; device_pos < devices.size(); device_pos++)
        {
            std::shared_ptr<Device> device = Create(devices.at(device_pos), context_props);

            if (device)
            {
                result.push_back(device);
            }
        }

        return result;
    }
}
