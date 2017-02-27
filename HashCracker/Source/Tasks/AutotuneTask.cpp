#include "Config.hpp"

#include "Tasks/AutotuneTask.hpp"

#include "Tasks/AttackTask.hpp"

#include "OpenCL/Device.hpp"
#include "OpenCL/Kernel.hpp"

namespace HonoursProject
{
    AutotuneTask::AutotuneTask(std::shared_ptr<AttackTask> attack_task, double target_speed)
        : attack_task(attack_task), target_speed(target_speed)
    {
    }

    AutotuneTask::~AutotuneTask()
    {
    }

    void AutotuneTask::run()
    {
        std::shared_ptr<Device> device = attack_task->getDevice();

        std::uint32_t device_power_max = device->getMaxComputeUnits() * device->getMaxWorkGroupSize() * KernelPlatform::DEVICE_SPEED_MAX;

        KernelPlatform::message_t msg_data = { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 };

        attack_task->getKernel()->setParam("msg", std::vector<KernelPlatform::message_t>(device_power_max, msg_data));
        attack_task->getKernel()->setParam("msg_prefix", std::vector<KernelPlatform::message_prefix_t>(KernelPlatform::KERNEL_LOOPS_MAX));

        std::size_t vector_width = device->getVectorWidth();
        std::shared_ptr<Program> program = attack_task->getKernel()->getProgram();

        double min_exec_time = try_execute(device->getMaxWorkGroupSize(), device->getMaxWorkGroupSize());

        for (std::size_t i = 0; i < Platform::AUTOTUNE_VALIDATE_CHECKS; i++)
        {
            double check_exec_ms = try_execute(device->getMaxWorkGroupSize(), device->getMaxWorkGroupSize());

            min_exec_time = std::min(min_exec_time, check_exec_ms);
        }

        for (std::size_t vec_size = vector_width << 1; vec_size <= KernelPlatform::MAX_DEVICE_VECTOR_WIDTH; vec_size <<= 1)
        {
            std::shared_ptr<Program> new_prog = std::make_shared<Program>(device);

            if (!new_prog->create(program, {std::make_pair("-DVECT_SIZE", std::to_string(vec_size))}))
            {
                break;
            }

            attack_task->setKernel(new_prog->findKernel(attack_task->getKernel()->getName()));

            double exec_time = try_execute(device->getMaxWorkGroupSize(), device->getMaxWorkGroupSize());

            for (std::size_t i = 0; i < Platform::AUTOTUNE_VALIDATE_CHECKS; i++)
            {
                double check_exec_time = try_execute(device->getMaxWorkGroupSize(), device->getMaxWorkGroupSize());

                exec_time = std::min(exec_time, check_exec_time);
            }

            if (exec_time < min_exec_time)
            {
                min_exec_time = exec_time;

                vector_width = vec_size;

                program = new_prog;
            }
            else
            {
                break;
            }
        }

        attack_task->setKernel(program->findKernel(attack_task->getKernel()->getName()));

        std::uint32_t device_speed = KernelPlatform::DEVICE_SPEED_MIN;
        std::uint32_t kernel_loops = KernelPlatform::KERNEL_LOOPS_MIN;

        // first find out highest kernel-loops that stays below target_ms

        for (kernel_loops = KernelPlatform::KERNEL_LOOPS_MAX; 
            kernel_loops > KernelPlatform::KERNEL_LOOPS_MIN; 
            kernel_loops >>= 1)
        {
            double exec_ms = try_execute(1, kernel_loops);

            for (std::size_t i = 0; i < Platform::AUTOTUNE_VALIDATE_CHECKS; i++)
            {
                double check_exec_ms = try_execute(1, kernel_loops);

                exec_ms = std::min(exec_ms, check_exec_ms);
            }

            if (exec_ms < target_speed)
            {
                break;
            }
        }

        // now the same for device-speed but with the new kernel-loops from previous loop set

        for (std::size_t i = 0; i < 10; i++)
        {
            std::uint32_t device_speed_try = 1u << i;

            double exec_ms = try_execute(device_speed_try, kernel_loops);

            for (std::size_t i = 0; i < Platform::AUTOTUNE_VALIDATE_CHECKS; i++)
            {
                double check_exec_ms = try_execute(device_speed_try, kernel_loops);

                exec_ms = std::min(exec_ms, check_exec_ms);
            }

            if (exec_ms > target_speed)
            {
                break;
            }

            device_speed = device_speed_try;
        }

        double last_exec_ms = try_execute(device_speed, kernel_loops);

        for (std::size_t i = 0; i < Platform::AUTOTUNE_VALIDATE_CHECKS; i++)
        {
            double check_exec_ms = try_execute(device_speed, kernel_loops);

            last_exec_ms = std::min(last_exec_ms, check_exec_ms);
        }

        std::uint32_t diff = kernel_loops - device_speed;
        std::uint32_t device_speed_tmp = device_speed;
        std::uint32_t kernel_loops_tmp = kernel_loops;

        for (std::size_t d = 1; d < 1024; d++)
        {
            std::uint32_t device_speed_try = device_speed_tmp * d;
            std::uint32_t kernel_loops_try = kernel_loops_tmp / d;

            if (device_speed_try > KernelPlatform::DEVICE_SPEED_MAX)
            {
                break;
            }

            if (kernel_loops_try < KernelPlatform::KERNEL_LOOPS_MIN)
            {
                break;
            }

            std::uint32_t diff_new = kernel_loops_try - device_speed_try;

            if (diff_new > diff)
            {
                break;
            }

            diff_new = diff;

            double exec_ms = try_execute(device_speed_try, kernel_loops_try);

            for (int i = 0; i < Platform::AUTOTUNE_VALIDATE_CHECKS; i++)
            {
                double check_exec_ms = try_execute(device_speed_try, kernel_loops_try);

                exec_ms = std::min(exec_ms, check_exec_ms);
            }

            if (exec_ms < last_exec_ms)
            {
                last_exec_ms = exec_ms;

                device_speed = device_speed_try;
                kernel_loops = kernel_loops_try;
            }
        }

        double left_speed_ms = target_speed / last_exec_ms;

        double speed_left = (double) KernelPlatform::DEVICE_SPEED_MAX / device_speed;

        double speed_ratio = std::min(left_speed_ms, speed_left);

        if (speed_ratio >= 1.0)
        {
            device_speed = (std::uint32_t)(device_speed * speed_ratio);
        }

        attack_task->setBatchSize(attack_task->getBatchSize() * device_speed);
        attack_task->setInnerLoopStep(kernel_loops);
    }

    double AutotuneTask::try_execute(std::uint32_t device_speed, std::uint32_t kernel_loops, double time_ratio)
    {
        std::shared_ptr<Device> device = attack_task->getDevice();

        std::uint32_t device_power = device->getMaxComputeUnits() * device->getMaxWorkGroupSize() * device_speed;

        std::shared_ptr<Kernel> kernel = attack_task->getKernel();

        kernel->setParam("msg_batch_size", device_power);
        kernel->setParam("inner_loop_size", kernel_loops);

        return kernel->execute({ device_power, 1, 1 }, { device->getMaxWorkGroupSize(), 1, 1 }) / time_ratio;
    }
}