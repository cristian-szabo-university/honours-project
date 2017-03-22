/**
    KernelParam.hpp

    Project Name: HashCracker

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#pragma once

namespace HonoursProject
{
    class KernelBuffer;
    class Kernel;

    class HASH_CRACKER_PUBLIC KernelParam : public std::enable_shared_from_this<KernelParam>
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

        KernelParam(std::shared_ptr<Kernel> kernel);

        virtual ~KernelParam();

        bool create(const std::string& param_name,
            const std::string param_type,
            std::size_t param_pos,
            cl_kernel_arg_address_qualifier address,
            cl_kernel_arg_access_qualifier access);

        bool destroy();

        std::shared_ptr<Kernel> getKernel();

        const std::string& getName();

        const std::string& getType();

        std::size_t getIndex();

        void toggleAutoSync();

        bool autoSyncEnable();

        void setAddressQualifier(AddressQualifier address_qualifier);

        AddressQualifier getAddressQualifier();

        void setAccessQualifier(AccessQualifier access_qualifier);

        AccessQualifier getAccessQualifier();

        static bool Copy(std::shared_ptr<KernelParam> src_param, std::shared_ptr<KernelParam> dst_param, std::size_t size, std::size_t src_offset = 0, std::size_t dst_offset = 0);

        static bool Copy(const std::string& name, std::shared_ptr<Kernel> src_kernel, std::shared_ptr<Kernel> dst_kernel, std::size_t size, std::size_t src_offset = 0, std::size_t dst_offset = 0);

    protected:

        bool ready;

        std::shared_ptr<Kernel> kernel;

        std::string name;

        std::string type;

        std::uint32_t index;

        bool auto_sync;

        AddressQualifier address_qualifier;

        AccessQualifier access_qualifier;

    };
}
