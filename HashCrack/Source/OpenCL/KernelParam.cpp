#include "Config.hpp"

#include "OpenCL/KernelParam.hpp"

namespace HonoursProject
{
    KernelParam::KernelParam(
        const std::string & name,
        const std::string & type,
        std::size_t index,
        AddressQualifier address_qualifier,
        AccessQualifier access_qualifier)
        :
        name(name),
        type(type),
        index(index),
        auto_sync(false),
        address_qualifier(address_qualifier),
        access_qualifier(access_qualifier)
    {
        if (this->type == "u8")
        {
            this->type = "uchar";
        }
        else if (this->type == "u16")
        {
            this->type = "ushort";
        }
        else if (this->type == "u32")
        {
            this->type = "uint";
        }
        else if (this->type == "u64")
        {
            this->type = "ulong";
        }
    }

    KernelParam::~KernelParam()
    {
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

    std::shared_ptr<KernelParam> KernelParam::Create(
        const std::string & param_name,
        const std::string param_type,
        std::size_t param_pos,
        cl_kernel_arg_address_qualifier address,
        cl_kernel_arg_access_qualifier access)
    {
        AddressQualifier address_qualifier = AddressQualifier::Invalid;

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

        AccessQualifier access_qualifier = AccessQualifier::Invalid;

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

        return std::make_shared<KernelParam>(
            param_name,
            param_type,
            param_pos,
            address_qualifier,
            access_qualifier);
    }
}
