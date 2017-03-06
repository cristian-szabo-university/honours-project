#pragma once

namespace HonoursProject
{
    class KernelParam;

    class HASH_CRACKER_PUBLIC DeviceMemory : public std::enable_shared_from_this<DeviceMemory>
    {
    public:

        enum class AccessFlag : std::int32_t
        {
            Invalid = -1,
            ReadOnly,
            WriteOnly,
            ReadWrite,
            MaxCount
        };

        enum class AddressFlag : std::int32_t
        {
            Invalid = -1,
            Global,
            Local,
            MaxCount
        };

        DeviceMemory(std::shared_ptr<KernelParam> param);

        ~DeviceMemory();

        bool create(std::size_t size, AddressFlag flag);

        bool isReady();

        bool destroy();

        cl::Buffer getHandle();

        std::shared_ptr<KernelParam> getParam();

        AccessFlag getAccessFlag();

        AddressFlag getAddressFlag();

        std::size_t getSize();

        static bool Copy(std::shared_ptr<DeviceMemory> src_mem, std::shared_ptr<DeviceMemory> dst_mem, std::size_t size, std::size_t src_offset = 0, std::size_t dst_offset = 0);

    private:

        bool ready;

        cl::Buffer handle;

        std::shared_ptr<KernelParam> param;

        AccessFlag access_flag;

        AddressFlag address_flag;

        std::size_t size;

    };
}
