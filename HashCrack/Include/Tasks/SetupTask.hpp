#pragma once

#include "Core/HashCracker.hpp"

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class HashCracker;
    class AttackTask;
    class Device;

    class HASHCRACK_PUBLIC SetupTask : public TTask<std::shared_ptr<AttackTask>>
    {
    public:

        SetupTask(std::shared_ptr<HashCracker> hash_cracker, std::shared_ptr<Device> device, const std::string& hash_msg, HashCracker::HashFunc hash_func);

        virtual ~SetupTask();

        std::uint64_t getTotalBatchSize();

        std::uint32_t getInnerLoopSize();

    protected:

        std::shared_ptr<HashCracker> hash_cracker;

        HashCracker::HashFunc hash_func;

        std::string hash_msg;

        std::shared_ptr<Device> device;

        std::uint64_t total_batch_size;

        std::uint32_t inner_loop_size;

        KernelPlatform::message_digest_t parseMD5Hash(std::string hash);

    };
}
