#include "Config.hpp"

#include "Tasks/AttackTask.hpp"

#include "Core/Platform.hpp"
#include "Core/HashCracker.hpp"

#include "OpenCL/Kernel.hpp"
#include "OpenCL/Device.hpp"

#include "Tasks/KernelTask.hpp"
#include "Tasks/SetupTask.hpp"
#include "Tasks/AutotuneTask.hpp"
#include "Tasks/CrackerTask.hpp"

namespace HonoursProject
{
    AttackTask::AttackTask()
        :
        batch_size(0),
        batch_offset(0),
        inner_loop_size(0),
        inner_loop_step(0),
        inner_loop_pos(0)
    {
    }

    AttackTask::~AttackTask()
    {
    }

    std::string AttackTask::run()
    {
        std::shared_ptr<Device> device = kernel_hash_crack->getProgram()->getDevice();
        std::vector<KernelPlatform::message_index_t> hash_cracked(1);

        for (inner_loop_pos = 0; inner_loop_pos < inner_loop_size; inner_loop_pos += inner_loop_step)
        {
            while (cracker_task->getStatus() == CrackerTask::Status::Paused)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            if (cracker_task->getStatus() == CrackerTask::Status::Cracked ||
                cracker_task->getStatus() == CrackerTask::Status::Aborted)
            {
                break;
            }

            std::uint32_t inner_loop_left = inner_loop_size - inner_loop_pos;

            if (inner_loop_left > inner_loop_step)
            {
                inner_loop_left = inner_loop_step;
            }

            auto speed_start_time = std::chrono::high_resolution_clock::now();

            preKernelExecute(inner_loop_left, inner_loop_pos);

            kernel_hash_crack->setParam("inner_loop_size", inner_loop_left);

            std::uint64_t exec_time = kernel_hash_crack->execute(batch_size, device->getMaxWorkGroupSize());

            auto speed_end_time = std::chrono::high_resolution_clock::now();

            if (exec_time)
            {
                exec_ms.push_front(exec_time / 1000000.0);

                if (exec_ms.size() > Platform::MAX_EXEC_CACHE_SIZE)
                {
                    exec_ms.pop_back();
                }
            }

            speed_ms.push_front(std::make_pair(batch_size * inner_loop_left, std::chrono::duration_cast<std::chrono::milliseconds>(speed_end_time - speed_start_time).count()));

            if (speed_ms.size() > Platform::MAX_SPEED_CACHE_SIZE)
            {
                speed_ms.pop_back();
            }

            kernel_hash_crack->getParam("msg_index", hash_cracked);

            if (hash_cracked.front().found)
            {
                cracker_task->setStatus(CrackerTask::Status::Cracked);

                break;
            }
        }

        msg_idx = hash_cracked.front();

        return std::string();
    }

    void AttackTask::transfer(std::shared_ptr<BaseTask> task)
    {
        {
            std::shared_ptr<KernelTask> cast_task = std::dynamic_pointer_cast<KernelTask>(task);

            if (cast_task)
            {
                kernel_hash_crack = cast_task->getKernel();
            }
        }

        {
            std::shared_ptr<SetupTask> cast_task = std::dynamic_pointer_cast<SetupTask>(task);

            if (cast_task)
            {
                setInnerLoopSize(cast_task->getInnerLoopSize());
            }
        }

        {
            std::shared_ptr<AutotuneTask> cast_task = std::dynamic_pointer_cast<AutotuneTask>(task);

            if (cast_task)
            {
                kernel_hash_crack = cast_task->getKernel();

                setBatchSize(cast_task->getDeviceSpeed());
                setInnerLoopStep(cast_task->getKernelLoops());
            }
        }

        {
            std::shared_ptr<CrackerTask> cast_task = std::dynamic_pointer_cast<CrackerTask>(task);

            if (cast_task)
            {
                cracker_task = cast_task;
            }
        }
    }

    void AttackTask::setBatchSize(std::uint32_t batch_size)
    {
        this->batch_size = batch_size;

        kernel_hash_crack->setParam("msg_batch_size", batch_size);
    }

    std::uint32_t AttackTask::getBatchSize()
    {
        return batch_size;
    }

    void AttackTask::setBatchOffset(std::uint64_t batch_offset)
    {
        this->batch_offset = batch_offset;
    }

    std::uint64_t AttackTask::getBatchOffset()
    {
        return batch_offset;
    }

    void AttackTask::setInnerLoopSize(std::uint32_t inner_loop_size)
    {
        this->inner_loop_size = inner_loop_size;
    }

    std::uint32_t AttackTask::getInnerLoopSize()
    {
        return inner_loop_size;
    }

    void AttackTask::setInnerLoopStep(std::uint32_t inner_loop_step)
    {
        this->inner_loop_step = inner_loop_step;
    }

    std::uint32_t AttackTask::getInnerLoopStep()
    {
        return inner_loop_step;
    }

    double AttackTask::getExecTime()
    {
        double result;

        result = std::accumulate(exec_ms.begin(), exec_ms.end(), 0.0, std::plus<double>());

        result /= exec_ms.size();

        return result;
    }

    double AttackTask::getSpeedTime()
    {
        std::pair<std::uint64_t, std::uint64_t> result;

        result = std::accumulate(speed_ms.begin(), speed_ms.end(), std::make_pair(0ull, 0ull), 
            [](std::pair<std::uint64_t, std::uint64_t> a, std::pair<std::uint64_t, std::uint64_t> b)
        {
            return std::make_pair(a.first + b.first, a.second + b.second);
        });

        double speed_cnt = (double) result.first / speed_ms.size();
        double speed_tm = (double) result.second / speed_ms.size();

        return (speed_cnt / speed_tm);
    }

    void AttackTask::setKernel(std::shared_ptr<Kernel> kernel)
    {
        kernel_hash_crack = kernel;
    }

    std::shared_ptr<Kernel> AttackTask::getKernel()
    {
        return kernel_hash_crack;
    }

    std::shared_ptr<Device> AttackTask::getDevice()
    {
        return kernel_hash_crack->getProgram()->getDevice();
    }

    void AttackTask::preKernelExecute(std::uint32_t inner_loop_left, std::uint32_t inner_loop_pos)
    {
    }
}
