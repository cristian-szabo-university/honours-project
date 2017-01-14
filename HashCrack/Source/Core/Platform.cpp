#include "Config.hpp"

#include "Core/Platform.hpp"

#include "OpenCL/KernelBuffer.hpp"

namespace HonoursProject
{
    std::vector< std::shared_ptr<KernelBuffer> > KernelPlatform::data_types =
    {
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_bool>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_bool>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_char>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_char>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_uchar>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_uchar>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_short>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_short>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_ushort>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_ushort>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_int>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_int>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_uint>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_uint>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_long>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_long>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_ulong>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_ulong>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_float>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_float>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<cl_double>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<cl_double>>()),

        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<charset_t>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<charset_t>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<message_t>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<message_t>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<message_prefix_t>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<message_prefix_t>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<message_index_t>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<message_index_t>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBuffer<message_digest_t>>()),
        std::dynamic_pointer_cast<KernelBuffer>(std::make_shared<TKernelBufferArray<message_digest_t>>()),
    };

    std::shared_ptr<KernelBuffer> KernelPlatform::CreateDataType(const std::string & type_name)
    {
        std::shared_ptr<KernelBuffer> result;

        for (auto& param : data_types)
        {
            if (type_name == param->getType())
            {
                result = param->clone();

                break;
            }
        }

        if (!result)
        {
            throw std::runtime_error("Error: Data type not found!");
        }

        return result;
    }

    std::string KernelPlatform::CleanCLString(const std::string & str)
    {
        std::string result = str;

        std::size_t pos_end = result.find('\0');

        if (pos_end != std::string::npos)
        {
            result = result.erase(pos_end, 1);
        }

        std::size_t pos_null = result.find('\n');

        if (pos_null != std::string::npos)
        {
            result = result.erase(pos_null, 1);
        }

        return result;
    }

    std::uint32_t Platform::to_int32(const std::uint8_t * bytes)
    {
        return (std::uint32_t)bytes[0]
            | ((std::uint32_t)bytes[1] << 8)
            | ((std::uint32_t)bytes[2] << 16)
            | ((std::uint32_t)bytes[3] << 24);
    }
}

char comma_numpunct::do_thousands_sep() const
{
    return ',';
}

std::string comma_numpunct::do_grouping() const
{
    return "\03";
}
