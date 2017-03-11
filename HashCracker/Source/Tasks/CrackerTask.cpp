#include "Config.hpp"

#include "Tasks/CrackerTask.hpp"

#include "Core/HashCracker.hpp"
#include "Core/Logger.hpp"

#include "OpenCL/Device.hpp"

#include "Tasks/AttackTask.hpp"
#include "Tasks/SetupTask.hpp"

namespace HonoursProject
{
    CrackerTask::CrackerTask(bool benchmark)
        : status(Status::Idle), total_message_size(0), total_message_progress(0)
    {
    }

    CrackerTask::~CrackerTask()
    {
    }

    std::string CrackerTask::run()
    {
        std::string hash_plain;

        std::uint64_t batch_offset = 0;
        std::uint64_t batch_left = total_batch_size - batch_offset;

        total_message_progress = 0;

        std::vector<std::future<std::string>> attack_futures;

        setStatus(Status::Running);

        start_time = std::chrono::system_clock::now();

        while (batch_left > 0 && status != Status::Cracked)
        {
            if (status == Status::Aborted)
            {
                break;
            }

            for (auto task : attack_tasks)
            {
                batch_left = total_batch_size - batch_offset;

                if (batch_left <= 0)
                {
                    status = Status::Idle;

                    break;
                }

                std::uint32_t batch_size = task->getBatchSize();

                if (batch_left < batch_size)
                {
                    batch_size = (std::uint32_t)batch_left;

                    task->setBatchSize(batch_size);
                }

                task->setBatchOffset(batch_offset);

                batch_offset += batch_size;
            }

            Platform::ExecuteTasks<std::string>(attack_tasks,
                [&](std::shared_ptr<AttackTask> attack_task, std::future<std::string>&& future, std::size_t index)
            {
                try
                {
                    std::string result = future.get();

                    if (!result.empty())
                    {
                        hash_plain = result;
                    }

                    total_message_progress += (attack_task->getBatchSize() * attack_task->getInnerLoopSize());
                }
                catch (std::exception& ex)
                {
                    Logger::error("Error: Failed execute attack! %s", ex.what());
                }
            });

            if (benchmark)
            {
                break;
            }
        }

        return hash_plain;
    }

    void CrackerTask::transfer(std::shared_ptr<BaseTask> task)
    {
        {
            std::shared_ptr<SetupTask> cast_task = std::dynamic_pointer_cast<SetupTask>(task);

            if (cast_task)
            {
                total_batch_size = cast_task->getTotalBatchSize();

                total_message_size = cast_task->getInnerLoopSize() * total_batch_size;
            }
        }

        {
            std::shared_ptr<AttackTask> cast_task = std::dynamic_pointer_cast<AttackTask>(task);

            if (cast_task)
            {
                attack_tasks.push_back(cast_task);
            }
        }
    }

    bool CrackerTask::benchmarkEnable()
    {
        return benchmark;
    }

    void CrackerTask::setStatus(Status status)
    {
        this->status = status;
    }

    CrackerTask::Status CrackerTask::getStatus()
    {
        return status;
    }

    std::chrono::system_clock::time_point CrackerTask::getTimeStart()
    {
        return start_time;
    }

    std::chrono::seconds CrackerTask::getTimeRunning()
    {
        std::chrono::system_clock::time_point now_time = std::chrono::system_clock::now();

        return std::chrono::duration_cast<std::chrono::seconds>(now_time - start_time);
    }

    std::chrono::system_clock::time_point CrackerTask::getTimeFinish()
    {
        return start_time + getTimeEstimated();
    }

    std::chrono::seconds CrackerTask::getTimeEstimated()
    {
        double hash_speed_ms = 0.0;

        for (std::size_t device_pos = 0; device_pos < attack_tasks.size(); device_pos++)
        {
            hash_speed_ms += attack_tasks.at(device_pos)->getSpeedTime();
        }

        double eta_time = total_message_size / hash_speed_ms / 1000.0;

        return std::chrono::seconds(static_cast<std::uint64_t>(eta_time));
    }

    std::uint64_t CrackerTask::getTotalMessageSize()
    {
        return total_message_size;
    }

    std::uint64_t CrackerTask::getTotalMessageProgress()
    {
        return total_message_progress;
    }

    std::size_t CrackerTask::getDeviceNum()
    {
        return attack_tasks.size();
    }

    std::shared_ptr<Device> CrackerTask::getDeviceAt(std::size_t device_pos)
    {
        return attack_tasks.at(device_pos)->getDevice();
    }

    double CrackerTask::getDeviceSpeed(std::size_t device_pos)
    {
        return attack_tasks.at(device_pos)->getSpeedTime();
    }

    double CrackerTask::getDeviceExec(std::size_t device_pos)
    {
        return attack_tasks.at(device_pos)->getExecTime();
    }
}
