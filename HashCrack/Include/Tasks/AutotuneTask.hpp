#pragma once

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class AttackTask;

    class AutotuneTask : public NoReturnTask
    {
    public:

        AutotuneTask(std::shared_ptr<AttackTask> attack_task, double target_speed);

        virtual ~AutotuneTask();

        virtual void run() override;

    private:

        std::shared_ptr<AttackTask> attack_task;

        double target_speed;

        double try_execute(std::uint32_t device_speed, std::uint32_t kernel_loops, double time_ratio = 1000000.0);

    };
}
