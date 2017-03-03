#include "Config.hpp"

#include "OpenCL/KernelParam.hpp"
#include "OpenCL/KernelBuffer.hpp"
#include "OpenCL/Program.hpp"
#include "OpenCL/Kernel.hpp"
#include "OpenCL/DeviceMemory.hpp"

namespace HonoursProject
{
    KernelParam::KernelParam(std::shared_ptr<Kernel> kernel)
        : ready(false), auto_sync(false), index(0), kernel(kernel)
    {        
    }

    KernelParam::~KernelParam()
    {
    }

    bool KernelParam::create(const std::string & param_name, const std::string param_type, std::size_t param_pos, cl_kernel_arg_address_qualifier address, cl_kernel_arg_access_qualifier access)
    {
        if (ready)
        {
            return false;
        }

        name = param_name;

        type = param_type;

        std::size_t pos = type.find("*");
        if (pos != std::string::npos)
        {
            type = type.erase(pos, 1);
        }

        if (type == "u8" || type == "u8x")
        {
            type = "uchar";
        }
        else if (type == "u16" || type == "u16x")
        {
            type = "ushort";
        }
        else if (type == "u32" || type == "u32x")
        {
            type = "uint";
        }
        else if (type == "u64" || type == "u64x")
        {
            type = "ulong";
        }

        if (pos != std::string::npos)
        {
            type.append("*");
        }

        index = param_pos;

        switch (address)
        {
        case CL_KERNEL_ARG_ADDRESS_PRIVATE:
            address_qualifier = AddressQualifier::Private;
            break;

        case CL_KERNEL_ARG_ADDRESS_GLOBAL:
            address_qualifier = AddressQualifier::Global;
            break;

        case CL_KERNEL_ARG_ADDRESS_LOCAL:
            address_qualifier = AddressQualifier::Local;
            break;

        case CL_KERNEL_ARG_ADDRESS_CONSTANT:
            address_qualifier = AddressQualifier::Constant;
            break;
        }

        switch (access)
        {
        case CL_KERNEL_ARG_ACCESS_READ_ONLY:
            access_qualifier = AccessQualifier::ReadOnly;
            break;

        case CL_KERNEL_ARG_ACCESS_WRITE_ONLY:
            access_qualifier = AccessQualifier::WriteOnly;
            break;

        case CL_KERNEL_ARG_ACCESS_READ_WRITE:
            access_qualifier = AccessQualifier::ReadWrite;
            break;

        case CL_KERNEL_ARG_ACCESS_NONE:
            access_qualifier = AccessQualifier::ReadWrite;
            break;
        }

        ready = !ready;

        return true;
    }

    bool KernelParam::destroy()
    {
        if (!ready)
        {
            return false;
        }

        name.clear();

        type.clear();

        index = 0;

        auto_sync = false;

        address_qualifier = AddressQualifier::Invalid;

        access_qualifier = AccessQualifier::Invalid;

        ready = !ready;

        return true;
    }

    std::shared_ptr<Kernel> KernelParam::getKernel()
    {
        return kernel;
    }

    const std::string & KernelParam::getName()
    {
        return name;
    }

    const std::string & KernelParam::getType()
    {
        return type;
    }

    std::size_t KernelParam::getIndex()
    {
        return index;
    }

    void KernelParam::toggleAutoSync()
    {
        auto_sync = !auto_sync;
    }

    bool KernelParam::autoSyncEnable()
    {
        return auto_sync;
    }

    KernelParam::AddressQualifier KernelParam::getAddressQualifier()
    {
        return address_qualifier;
    }

    KernelParam::AccessQualifier KernelParam::getAccessQualifier()
    {
        return access_qualifier;
    }

    bool KernelParam::Copy(std::shared_ptr<KernelParam> src_param, std::shared_ptr<KernelParam> dst_param, std::size_t size, std::size_t src_offset, std::size_t dst_offset)
    {
        cl_int cl_error = CL_SUCCESS;

        std::shared_ptr<KernelBuffer> src_buf = src_param->getKernel()->findBuffer(src_param);
        std::shared_ptr<KernelBuffer> dst_buf = dst_param->getKernel()->findBuffer(dst_param);

        if (!src_buf->readPending() && 
            !KernelBuffer::Copy(src_buf, dst_buf, size, src_offset, dst_offset))
        {        
            return false;
        }

        std::shared_ptr<DeviceMemory> src_mem = src_param->getKernel()->getProgram()->findMemory(src_buf);
        std::shared_ptr<DeviceMemory> dst_mem = dst_param->getKernel()->getProgram()->findMemory(dst_buf);

        if (src_mem && dst_mem)
        {
            if (!src_buf->writePending() && 
                !DeviceMemory::Copy(src_mem, dst_mem, size * src_buf->getElemSize(), dst_offset * src_buf->getElemSize()))
            {
                return false;
            }
        }

        return true;
    }

    bool KernelParam::Copy(const std::string & name, std::shared_ptr<Kernel> src_kernel, std::shared_ptr<Kernel> dst_kernel, std::size_t size, std::size_t src_offset, std::size_t dst_offset)
    {
        std::shared_ptr<KernelParam> src_param = src_kernel->findParam(name);

        if (!src_param)
        {
            return false;
        }

        std::shared_ptr<KernelParam> dst_param = dst_kernel->findParam(name);

        if (!dst_param)
        {
            return false;
        }

        return Copy(src_param, dst_param, size, src_offset, dst_offset);
    }
}
