#pragma once

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class AttackTask;

    class AutotuneTask : public NoReturnTask
    {
    public:

        AutotuneTask(std::shared_ptr<AttackTask> attack_task);

        virtual ~AutotuneTask();

        virtual void run() override;

    private:

        std::shared_ptr<AttackTask> attack_task;

    };
}
