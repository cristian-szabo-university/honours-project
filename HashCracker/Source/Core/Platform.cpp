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

#if WIN32
	std::string Demangle(const char* name)
	{
	    return name;
	}
#else
	std::string Demangle(const char* name)
	{
	    int status = -4;

    	    std::unique_ptr<char, void(*)(void*)> result {
            	abi::__cxa_demangle(name, NULL, NULL, &status),
        	std::free
    	    };

    	    return (status == 0) ? result.get() : name; 
	}
#endif
    }
}
