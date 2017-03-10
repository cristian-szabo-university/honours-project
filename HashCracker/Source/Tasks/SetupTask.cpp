#include "Config.hpp"

#include "Tasks/SetupTask.hpp"

#include "Core/Platform.hpp"
#include "Core/Logger.hpp"

namespace HonoursProject
{
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

    std::vector<std::uint32_t> SetupTask::getMessageDigest()
    {
        return msg_dgst;
    }
}
