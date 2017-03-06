#pragma once

namespace HonoursProject
{
    class KernelBuffer;
    template<class T> class TKernelBufferValue;
    template<class T> class TKernelBufferArray;

    namespace Platform
    {
        const std::uint32_t CHAR_SIZE = 256;

        const std::uint32_t MIN_MESSAGE_SIZE = 1;
        const std::uint32_t MAX_MESSAGE_SIZE = 54;

        const std::uint32_t MAX_CHARSET_COUNT = 64;

        const std::uint8_t OPENCL_COMPILER_MAJOR_VERSION = '1';
        const std::uint8_t OPENCL_COMPILER_MINOR_VERSION = '1';

        const std::size_t MAX_EXEC_CACHE_SIZE = 128;
        const std::size_t MAX_SPEED_CACHE_SIZE = 128;

        const std::size_t AUTOTUNE_VALIDATE_CHECKS = 1;

        HASH_CRACKER_PUBLIC std::uint32_t to_int32(const std::uint8_t * bytes);
    };

    namespace KernelPlatform
    {
        const std::size_t MAX_KERNEL_WORK_GROUP = 256;

        const std::size_t DEVICE_SPEED_MIN = 1;
        const std::size_t DEVICE_SPEED_MAX = 1024;

        const std::size_t KERNEL_LOOPS_MIN = 1;
        const std::size_t KERNEL_LOOPS_MAX = 1024;

        const std::size_t MAX_DEVICE_VECTOR_WIDTH = 16;

        const std::uint32_t MAX_MESSAGE_PREFIX = 1024;

#pragma pack(push, 1)
        typedef struct
        {
            cl_uchar data[Platform::CHAR_SIZE];

            cl_uint size;
        } charset_t;

        typedef struct
        {
            cl_uint data[16];

            cl_uint size;
        } message_t;

        typedef struct
        {
            cl_uint data;
        } message_prefix_t;

        typedef struct
        {
            cl_uint found;
            cl_uint msg_batch_pos;
            cl_uint inner_loop_pos;
        } message_index_t;
#pragma pack(pop)

        HASH_CRACKER_PUBLIC std::string CleanCLString(const std::string& str);
    };
}
