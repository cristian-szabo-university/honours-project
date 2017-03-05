#pragma once

namespace HonoursProject
{
    class KernelParam;
    class KernelBuffer;
    class DeviceMemory;

    class HASHCRACK_PUBLIC Device : public std::enable_shared_from_this<Device>
    {
    public:

        enum class Type : std::int32_t
        {
            Invalid = -1,
            CPU,
            GPU,
            Accelerator,
            Custom,
            MaxCount
        };

        enum class Vendor : std::int32_t
        {
            Invalid = -1,
            NVIDIA,
            Intel,
            AMD,
            MaxCount
        };

        Device();

        ~Device();

        bool create(cl::Platform platform, std::size_t index, cl_context_properties context_props = 0);

        bool destroy();

        std::uint32_t getId();

        cl::Device getHandle();

        cl::Context getContext();

        std::string getName();

        Vendor getVendor();

        Type getType();

        std::size_t getMaxComputeUnits();

        std::size_t getVectorWidth();

        std::size_t getMaxWorkGroupSize();

        std::uint64_t getMaxMemAllocSize();

        std::shared_ptr<DeviceMemory> createMemory(std::shared_ptr<KernelParam> param, std::size_t size);

        bool destroyMemory(std::shared_ptr<DeviceMemory> mem);

        std::uint64_t getTotalGlobalMemSize();

        std::uint64_t getUsedGlobalMemSize();

        std::uint64_t getTotalLocalMemSize();

        std::uint64_t getUsedLocalMemSize();

        static std::vector<std::shared_ptr<Device>> Create(cl::Platform platform, cl_device_type device_filter, cl_context_properties context_props = 0);

    private:

        static std::uint32_t next_buffer_id;

        bool ready;

        std::uint32_t id;

        cl::Device handle;

        cl::Context context;

        std::string name;

        Vendor vendor;

        Type type;

        cl_uint max_compute_units;

        std::size_t vector_width;

        std::size_t max_work_group_size;

        cl_ulong max_mem_alloc_size;

        cl_ulong total_global_mem_size;

        cl_ulong used_global_mem_size;

        cl_ulong total_local_mem_size;

        cl_ulong used_local_mem_size;

        cl_ulong max_const_buffer_size;

    };
}
