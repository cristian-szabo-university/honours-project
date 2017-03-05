#include "Config.hpp"

#include "Core/HashFunc.hpp"
#include "Core/HashCracker.hpp"

#include "OpenCL/KernelBuffer.hpp"

#include "ProgramEntry.hpp"

namespace HonoursProject
{
    template class MD5_HashFunc<Bruteforce_HashProcess>;
    // template class MD5_HashFunc<Dictionary_HashTask>;

    template<class Task>
    const HASHCRACK_PUBLIC std::string MD5_HashFunc<Task>::name = "MD5";

    template<>
    const HASHCRACK_PUBLIC ProgramEntry& MD5_HashFunc<Bruteforce_HashProcess>::program = bruteforce_md5;

    // const ProgramEntry& MD5_HashFunc<Dictionary_HashTask>::program = dictionary_md5;

    template<class Task>
    MD5_HashFunc<Task>::MD5_HashFunc()
    {
        if (!KernelBuffer::HasDataType<message_digest_t>())
        {
            KernelBuffer::RegisterDataType<message_digest_t>();
        }
    }

    /*
    template<class Task>
    const std::string SHA1_HashFunc<Task>::name = "SHA1";

    const ProgramEntry& SHA1_HashFunc<Bruteforce_HashTask>::program = bruteforce_sha1;

    // const ProgramEntry& SHA1_HashFunc<Dictionary_HashTask>::program = dictionary_sha1;
    */
    std::vector<std::uint32_t> MD5_Parse::parse(std::string hash)
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
}

