#include "Config.hpp"

#include "Core/HashFunc.hpp"

namespace HonoursProject
{
    HashFunc::~HashFunc()
    {
    }

    MD5_HashFunc::~MD5_HashFunc()
    {
    }

    MD5_HashFunc::Type MD5_HashFunc::type()
    {
        return Type::MD5;
    }

    KernelPlatform::message_digest_t MD5_HashFunc::parse(std::string hash)
    {
        std::array<std::uint8_t, 16> w;

        for (std::size_t o = 0, i = 0; o < hash.size(); o += 2, i++)
        {
            w[i] = static_cast<uint8_t>(std::stoul(hash.substr(o, 2), nullptr, 16));
        }

        KernelPlatform::message_digest_t hash_digest;

        for (std::size_t o = 0, i = 0; o < w.size(); o += 4, i++)
        {
            hash_digest.data[i] = Platform::to_int32(&w[0] + o);
        }

        hash_digest.data[0] -= 0x67452301u;
        hash_digest.data[1] -= 0xefcdab89u;
        hash_digest.data[2] -= 0x98badcfeu;
        hash_digest.data[3] -= 0x10325476u;

        return hash_digest;
    }

    std::uint32_t MD5_HashFunc::mask()
    {
        return 0x80808080;
    }

    std::uint32_t MD5_HashFunc::bit14()
    {
        return 1;
    }

    std::uint32_t MD5_HashFunc::bit15()
    {
        return 0;
    }
}

