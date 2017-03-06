#include "Config.hpp"

#include "OpenCL/DeviceMemory.hpp"

#include "OpenCL/Device.hpp"
#include "OpenCL/KernelParam.hpp"
#include "OpenCL/Kernel.hpp"
#include "OpenCL/Program.hpp"

namespace HonoursProject
{
    DeviceMemory::DeviceMemory(std::shared_ptr<KernelParam> param)
        : ready(false), param(param), address_flag(AddressFlag::Invalid)
    {
    }

    DeviceMemory::~DeviceMemory()
    {
        if (ready)
        {
            destroy();
        }
    }

    bool DeviceMemory::create(std::size_t size, AddressFlag flag)
    {
        if (ready)
        {
            return false;
        }

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

        std::shared_ptr<Device> device = param->getKernel()->getProgram()->getDevice();

        handle = cl::Buffer(device->getContext(), mem_flag, size);

        this->size = size;

        address_flag = AddressFlag::Invalid;

        switch (param->getAddressQualifier())
        {
        case KernelParam::AddressQualifier::Global:
            address_flag = AddressFlag::Global;
            break;

        case KernelParam::AddressQualifier::Local:
            address_flag = AddressFlag::Local;
            break;
        }

        ready = !ready;

        return true;
    }

    bool DeviceMemory::isReady()
    {
        return ready;
    }

    bool DeviceMemory::destroy()
    {
        if (!ready)
        {
            return false;
        }

        handle = cl::Buffer();

        access_flag = AccessFlag::Invalid;

        address_flag = AddressFlag::Invalid;

        ready = !ready;

        return true;
    }

    cl::Buffer DeviceMemory::getHandle()
    {
        return handle;
    }

    std::shared_ptr<KernelParam> DeviceMemory::getParam()
    {
        return param;
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

    bool DeviceMemory::Copy(std::shared_ptr<DeviceMemory> src_mem, std::shared_ptr<DeviceMemory> dst_mem, std::size_t size, std::size_t src_offset, std::size_t dst_offset)
    {
        if (!size ||
            size + dst_offset > dst_mem->getSize() ||
            size + src_offset > src_mem->getSize())
        {
            return false;
        }

        std::shared_ptr<Program> dst_prog = dst_mem->getParam()->getKernel()->getProgram();

        dst_prog->getCommandQueue().enqueueCopyBuffer(
            src_mem->getHandle(),
            dst_mem->getHandle(),
            src_offset, dst_offset, size);

        return true;
    }
}
