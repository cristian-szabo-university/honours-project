#include "Config.hpp"

#include "OpenCL/DeviceMemory.hpp"

#include "OpenCL/Device.hpp"
#include "OpenCL/KernelParam.hpp"

namespace HonoursProject
{
    DeviceMemory::DeviceMemory(cl::Buffer buffer, AddressFlag address_flag, std::shared_ptr<Device> device)
        : handle(buffer), device(device), address_flag(address_flag)
    {
        cl_int cl_error = CL_SUCCESS;

        cl_mem_flags mem_flags = handle.getInfo<CL_MEM_FLAGS>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::Buffer::getInfo()\n");
        };

        if (mem_flags & CL_MEM_READ_ONLY)
        {
            access_flag = AccessFlag::ReadOnly;
        }
        else if (mem_flags & CL_MEM_WRITE_ONLY)
        {
            access_flag = AccessFlag::WriteOnly;
        }
        else
        {
            access_flag = AccessFlag::ReadWrite;
        }

        size = handle.getInfo<CL_MEM_SIZE>(&cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::Buffer::getInfo()\n");
        };
    }

    DeviceMemory::~DeviceMemory()
    {
    }

    cl::Buffer DeviceMemory::getHandle()
    {
        return handle;
    }

    std::shared_ptr<Device> DeviceMemory::getDevice()
    {
        return device;
    }

    DeviceMemory::AccessFlag DeviceMemory::getAccessFlag()
    {
        return access_flag;
    }

    DeviceMemory::AddressFlag DeviceMemory::getAddressFlag()
    {
        return address_flag;
    }

    std::size_t DeviceMemory::getSize()
    {
        return size;
    }

    std::shared_ptr<DeviceMemory> DeviceMemory::Create(std::size_t mem_size, std::shared_ptr<KernelParam> param, std::shared_ptr<Device> device)
    {
        cl_int cl_error = CL_SUCCESS;

        cl_mem_flags mem_flag = 0;

        switch (param->getAccessQualifier())
        {
        case KernelParam::AccessQualifier::ReadOnly:
            mem_flag = CL_MEM_READ_ONLY;
            break;

        case KernelParam::AccessQualifier::WriteOnly:
            mem_flag = CL_MEM_WRITE_ONLY;
            break;

        case KernelParam::AccessQualifier::ReadWrite:
            mem_flag = CL_MEM_READ_WRITE;
            break;
        }

        cl::Buffer buffer = cl::Buffer(device->getContext(), mem_flag, mem_size, nullptr, &cl_error);

        if (cl_error != CL_SUCCESS)
        {
            throw std::runtime_error("ERROR: cl::Buffer()\n");
        };

        AddressFlag address_flag = AddressFlag::Invalid;

        switch (param->getAddressQualifier())
        {
        case KernelParam::AddressQualifier::Global:
            address_flag = AddressFlag::Global;
            break;

        case KernelParam::AddressQualifier::Local:
            address_flag = AddressFlag::Local;
            break;
        }

        return std::make_shared<DeviceMemory>(buffer, address_flag, device);
    }
}
