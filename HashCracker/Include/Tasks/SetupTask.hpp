#pragma once

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class HashFunc;
    class Kernel;

    class HASH_CRACKER_PUBLIC SetupTask : public TTask<bool, std::vector<std::string>>
    {
    public:

        virtual ~SetupTask();

        std::uint64_t getTotalBatchSize();

        std::uint32_t getInnerLoopSize();

    protected:

        std::uint64_t total_batch_size;

        std::uint32_t inner_loop_size;

    };
}
