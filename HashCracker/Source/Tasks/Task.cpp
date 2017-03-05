#include "Config.hpp"

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    NoReturnTask::~NoReturnTask()
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
}
