#include "Config.hpp"

#include "Core/HashFactory.hpp"

#include "OpenCL/KernelBuffer.hpp"

#include "ProgramEntry.hpp"

namespace HonoursProject
{
    HashFactory::~HashFactory()
    {
    }

    const std::string MD5::name = "MD5";

    MD5::MD5()
    {
        if (!KernelBuffer::HasDataType<message_digest_t>())
        {
            KernelBuffer::RegisterDataType<message_digest_t>();
        }
    }

    MD5::~MD5()
    {
    }

    const std::string & MD5::type()
    {
        return name;
    }

    std::vector<std::uint32_t> MD5::parse(std::string hash)
    {
        std::array<std::uint8_t, 16> w;

        for (std::size_t o = 0, i = 0; o < hash.size(); o += 2, i++)
        {
            w[i] = static_cast<uint8_t>(std::stoul(hash.substr(o, 2), nullptr, 16));
        }

        std::vector<std::uint32_t> hash_digest(4);

        for (std::size_t o = 0, i = 0; o < w.size(); o += 4, i++)
        {
            hash_digest[i] = Platform::to_int32(&w[0] + o);
        }

        hash_digest[0] -= 0x67452301u;
        hash_digest[1] -= 0xefcdab89u;
        hash_digest[2] -= 0x98badcfeu;
        hash_digest[3] -= 0x10325476u;

        return hash_digest;
    }

    const ProgramEntry & MD5::source()
    {
        return bruteforce_md5;
    }

    std::uint32_t MD5::mask()
    {
        return 0x80808080;
    }

    std::uint32_t MD5::bit14()
    {
        return 1;
    }

    std::uint32_t MD5::bit15()
    {
        return 0;
    }

    std::array<std::uint32_t, 4> MD5::digest()
    {
        return { 0, 3, 2, 1 };
    }
}

