#include "Config.hpp"

#include "Tasks/SetupTask.hpp"

namespace HonoursProject
{
    SetupTask::SetupTask(
        std::shared_ptr<HashCracker> hash_cracker,
        std::shared_ptr<Device> device, 
        const std::string& hash_msg, 
        std::shared_ptr<HashFunc> hash_func)
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
}
