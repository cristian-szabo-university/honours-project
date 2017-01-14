#include "Config.hpp"

#include "Tasks/AutotuneTask.hpp"

#include "Tasks/AttackTask.hpp"

namespace HonoursProject
{
    AutotuneTask::AutotuneTask(std::shared_ptr<AttackTask> attack_task)
        : attack_task(attack_task)
    {
    }

    AutotuneTask::~AutotuneTask()
    {
    }

    void AutotuneTask::run()
    {
        attack_task->setBatchSize(attack_task->getBatchSize() * 50);
        attack_task->setInnerLoopStep(60);
    }
}