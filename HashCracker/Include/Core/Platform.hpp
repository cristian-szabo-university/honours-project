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

        HASH_CRACKER_PUBLIC std::string CleanCLString(const std::string& str);

        template<class T>
        inline std::string GetTypeName()
        {
            std::string result;

            const char* name = typeid(T).name();
#if WIN32
            result = name;
#else
            int status;
            char* real_name;

            real_name = abi::__cxa_demangle(name, 0, 0, &status);

            if (status != 0)
            {
                result = real_name;

                free(real_name);
            }
#endif
            return result;
        }

        template<class T>
        inline std::string CleanTypeName()
        {
            std::string result = GetTypeName<T>();

            if (result.empty())
            {
                return result;
            }

            std::size_t pos = result.find_last_of("::");

            if (pos != std::string::npos)
            {
                std::size_t pos_space = result.find(" ");

                result = result.erase(pos_space + 1, pos - pos_space);
            }

            std::size_t pos_star = result.find("*");

            if (pos_star != std::string::npos)
            {
                pos_star--;

                if (result[pos_star] == ' ')
                {
                    result = result.erase(pos_star, 1);
                }
            }

            std::size_t pos_s = result.find("__int32");

            if (pos_s != std::string::npos)
            {
                result = result.replace(pos_s, 7, "int");
            }

            std::size_t pos_l = result.find("__int64");

            if (pos_l != std::string::npos)
            {
                result = result.replace(pos_l, 7, "long");
            }

            std::size_t pos_u = result.find("unsigned");

            if (pos_u != std::string::npos)
            {
                result = result.erase(pos_u, 8);

                result[0] = 'u';
            }

            return result;
        }
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
    };
}
