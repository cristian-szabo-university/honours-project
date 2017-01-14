#pragma once

namespace HonoursProject
{
    class Device;
    class KernelParam;

    class HASHCRACK_PUBLIC DeviceMemory : public std::enable_shared_from_this<DeviceMemory>
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

        DeviceMemory(cl::Buffer buffer, AddressFlag address_flag, std::shared_ptr<Device> device);

        ~DeviceMemory();

        cl::Buffer getHandle();

        std::shared_ptr<Device> getDevice();

        AccessFlag getAccessFlag();

        AddressFlag getAddressFlag();

        std::size_t getSize();

        static std::shared_ptr<DeviceMemory> Create(std::size_t mem_size, std::shared_ptr<KernelParam> param, std::shared_ptr<Device> device);

    private:

        cl::Buffer handle;

        std::shared_ptr<Device> device;

        AccessFlag access_flag;

        AddressFlag address_flag;

        std::size_t size;

    };
}
