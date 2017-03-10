#include "Config.hpp"

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    NoReturnTask::~NoReturnTask()
    {
    }

    void NoReturnTask::run()
    {
    }

    BaseTask::~BaseTask()
    {
    }

    void BaseTask::transfer(std::shared_ptr<BaseTask> task)
    {
    }

    Task::~Task()
    {
    }

    std::int32_t Task::run()
    {
        return 0;
    }
}
