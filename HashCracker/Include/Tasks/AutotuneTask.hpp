#pragma once

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class AttackTask;
    class Kernel;

    class AutotuneTask : public TTask<void>
    {
    public:

        AutotuneTask(double target_speed);

        virtual ~AutotuneTask();

        virtual void transfer(std::shared_ptr<BaseTask> task) override;

        virtual void run() override;

        std::shared_ptr<Kernel> getKernel();

        std::uint32_t getDeviceSpeed();

        std::uint32_t getKernelLoops();

    private:

        double target_speed;

        std::shared_ptr<Kernel> kernel;

        std::uint32_t device_speed;

        std::uint32_t kernel_loops;

        double executeKernel(std::shared_ptr<Kernel> kernel, std::uint32_t device_speed, std::uint32_t kernel_loops);

    };
}
