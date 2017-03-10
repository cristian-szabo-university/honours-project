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

        std::string CleanTypeName(std::string type_name)
        {
            if (type_name.empty())
            {
                return type_name;
            }

            std::size_t pos_end = type_name.find('\0');

            if (pos_end != std::string::npos)
            {
                type_name = type_name.erase(pos_end, 1);
            }

            std::size_t pos_null = type_name.find('\n');

            if (pos_null != std::string::npos)
            {
                type_name = type_name.erase(pos_null, 1);
            }

            std::size_t pos = type_name.find_last_of("::");

            if (pos != std::string::npos)
            {
                std::size_t pos_space = type_name.find(" ");

                if (pos_space == std::string::npos)
                {
                    pos_space = -1;
                }

                type_name = type_name.erase(pos_space + 1, pos - pos_space);
            }

            std::size_t pos_struct = type_name.find("struct");

            if (pos_struct != std::string::npos)
            {
                type_name = type_name.erase(pos_struct, 7);
            }

            std::size_t pos_class = type_name.find("class");

            if (pos_class != std::string::npos)
            {
                type_name = type_name.erase(pos_class, 6);
            }

            std::size_t pos_star = type_name.find("*");

            if (pos_star != std::string::npos)
            {
                pos_star--;

                if (type_name[pos_star] == ' ')
                {
                    type_name = type_name.erase(pos_star, 1);
                }
            }

            std::size_t pos_s = type_name.find("__int32");

            if (pos_s != std::string::npos)
            {
                type_name = type_name.replace(pos_s, 7, "int");
            }

            std::size_t pos_l = type_name.find("__int64");

            if (pos_l != std::string::npos)
            {
                type_name = type_name.replace(pos_l, 7, "long");
            }

            std::size_t pos_u = type_name.find("unsigned");

            if (pos_u != std::string::npos)
            {
                type_name = type_name.erase(pos_u, 8);

                type_name[0] = 'u';
            }

            return type_name;
        }
    }
}
