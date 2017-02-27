#include "Config.hpp"

#include "Core/Platform.hpp"

#include "OpenCL/KernelBuffer.hpp"

namespace HonoursProject
{
    namespace Platform
    {
        std::uint32_t to_int32(const std::uint8_t * bytes)
        {
            return (std::uint32_t)bytes[0]
                | ((std::uint32_t)bytes[1] << 8)
                | ((std::uint32_t)bytes[2] << 16)
                | ((std::uint32_t)bytes[3] << 24);
        }
    }

    namespace KernelPlatform
    {
        std::string CleanCLString(const std::string & str)
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
    }
}
