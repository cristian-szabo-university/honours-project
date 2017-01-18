#pragma once

namespace HonoursProject
{
    class KernelBuffer;
    template<class T> class TKernelBuffer;
    template<class T> class TKernelBufferArray;

    class HASHCRACK_PUBLIC Platform
    {
    public:

        static const std::uint32_t CHAR_SIZE = 256;

        static const std::uint32_t MIN_MESSAGE_SIZE = 1;
        static const std::uint32_t MAX_MESSAGE_SIZE = 54;

        static const std::uint32_t MAX_CHARSET_COUNT = 64;

        static const std::uint8_t OPENCL_COMPILER_MAJOR_VERSION = '1';
        static const std::uint8_t OPENCL_COMPILER_MINOR_VERSION = '1';

        static const std::size_t MAX_EXEC_CACHE_SIZE = 128;
        static const std::size_t MAX_SPEED_CACHE_SIZE = 128;

        static const std::size_t AUTOTUNE_VALIDATE_CHECKS = 1;

        static std::uint32_t to_int32(const std::uint8_t * bytes);

    };

    class HASHCRACK_PUBLIC KernelPlatform
    {
    public:

        static const std::size_t MAX_KERNEL_WORK_GROUP = 256;

        static const std::size_t DEVICE_SPEED_MIN = 1;

        static const std::size_t DEVICE_SPEED_MAX = 1024;

        static const std::size_t KERNEL_LOOPS_MIN = 1;

        static const std::size_t KERNEL_LOOPS_MAX = 1024;

        static const std::uint32_t MAX_MESSAGE_PREFIX = 1024;

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

        typedef struct
        {
            cl_uint data[4];
        } message_digest_t;
#pragma pack(pop)

        template<class T>
        static void RegisterDataType()
        {
            data_types.push_back(std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<T>>()));
            data_types.push_back(std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<T>>()));
        }

        static std::shared_ptr<KernelBuffer> CreateDataType(const std::string& type_name);

        template< class T >
        static std::string CleanTypeName()
        {
            std::string result = typeid(T).name();

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

        static std::string CleanCLString(const std::string& str);

    private:

        static std::vector< std::shared_ptr<KernelBuffer> > data_types;

    };
}
