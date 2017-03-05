#pragma once

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class AttackTask;
    class Kernel;

    class AutotuneTask : public TTask<void, std::shared_ptr<AttackTask>, double>
    {
    public:

        virtual ~AutotuneTask();

        virtual void run(std::shared_ptr<AttackTask> attack_task, double target_speed) override;

    private:

        double try_execute(std::shared_ptr<Kernel> kernel, std::uint32_t device_speed, std::uint32_t kernel_loops);

    };
}
