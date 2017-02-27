#include "Config.hpp"

#include "Tasks/SetupTask.hpp"

namespace HonoursProject
{
    SetupTask::SetupTask(
        std::shared_ptr<HashCracker> hash_cracker,
        std::shared_ptr<Device> device, 
        const std::string& hash_msg, 
        HashCracker::HashFunc hash_func)
        :
        hash_cracker(hash_cracker),
        device(device),
        hash_msg(hash_msg),
        hash_func(hash_func),
        total_batch_size(0),
        inner_loop_size(0)
    {
    }

    SetupTask::~SetupTask()
    {
    }

    std::uint64_t SetupTask::getTotalBatchSize()
    {
        return total_batch_size;
    }

    std::uint32_t SetupTask::getInnerLoopSize()
    {
        return inner_loop_size;
    }

    KernelPlatform::message_digest_t SetupTask::parseMD5Hash(std::string hash)
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
}
