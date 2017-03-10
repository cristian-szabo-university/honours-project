#pragma once

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class HashFactory;
    class Kernel;

    class HASH_CRACKER_PUBLIC SetupTask : public TTask<bool>
    {
    public:

        virtual ~SetupTask();

        std::uint64_t getTotalBatchSize();

        std::uint32_t getInnerLoopSize();

        std::vector<std::uint32_t> getMessageDigest();

    protected:

        std::vector<std::uint32_t> msg_dgst;

        std::uint64_t total_batch_size;

        std::uint32_t inner_loop_size;

    };
}
