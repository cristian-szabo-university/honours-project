#pragma once

namespace HonoursProject
{
    class HASHCRACK_PUBLIC KernelParam
    {
    public:

        enum class AccessQualifier : std::int32_t
        {
            Invalid = -1,
            ReadOnly,
            WriteOnly,
            ReadWrite,
            MaxCount
        };

        enum class AddressQualifier : std::int32_t
        {
            Invalid = -1,
            Private,
            Global,
            Local,
            Constant,
            MaxCount
        };

        KernelParam(
            const std::string& name,
            const std::string& type,
            std::size_t index,
            AddressQualifier address_qualifier,
            AccessQualifier access_qualifier);

        virtual ~KernelParam();

        const std::string& getName();

        const std::string& getType();

        std::size_t getIndex();

        void toggleAutoSync();

        bool autoSyncEnable();

        AddressQualifier getAddressQualifier();

        AccessQualifier getAccessQualifier();

        static std::shared_ptr<KernelParam> Create(
            const std::string& param_name,
            const std::string param_type,
            std::size_t param_pos,
            cl_kernel_arg_address_qualifier address,
            cl_kernel_arg_access_qualifier access);

    protected:

        std::string name;

        std::string type;

        std::uint32_t index;

        bool auto_sync;

        AddressQualifier address_qualifier;

        AccessQualifier access_qualifier;

    };
}
