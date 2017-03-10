#include "Config.hpp"

#include "Core/AttackDispatch.hpp"

#include "Core/HashCracker.hpp"
#include "Core/Logger.hpp"

#include "OpenCL/Device.hpp"

#include "Tasks/AttackTask.hpp"

namespace HonoursProject
{
    AttackDispatch::AttackDispatch(
        std::shared_ptr<HashCracker> hash_cracker, 
        std::vector<std::shared_ptr<AttackTask>> attack_tasks, 
        std::uint64_t total_batch_size)
        : 
        hash_cracker(hash_cracker),
        attack_tasks(attack_tasks),
        total_batch_size(total_batch_size)
    {
    }

    std::string AttackDispatch::execute()
    {
        std::string hash_plain;

        std::uint64_t batch_offset = 0;
        std::uint64_t batch_left = total_batch_size - batch_offset;

        total_message_progress = 0;

        std::vector<std::future<std::string>> attack_futures;

        hash_cracker->setStatus(HashCracker::Status::Running);

        while (batch_left > 0 && hash_cracker->getStatus() != HashCracker::Status::Cracked)
        {
            if (hash_cracker->getStatus() == HashCracker::Status::Aborted)
            {
                break;
            }

            for (auto task : attack_tasks)
            {
                batch_left = total_batch_size - batch_offset;

                if (batch_left <= 0)
                {
                    hash_cracker->setStatus(HashCracker::Status::Idle);

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
        }

        return hash_plain;
    }

    std::uint64_t AttackDispatch::getTotalMessageProgress()
    {
        return total_message_progress;
    }

    std::size_t AttackDispatch::getDeviceNum()
    {
        return attack_tasks.size();
    }

    std::shared_ptr<Device> AttackDispatch::getDeviceAt(std::size_t device_pos)
    {
        return attack_tasks.at(device_pos)->getDevice();
    }

    double AttackDispatch::getDeviceSpeed(std::size_t device_pos)
    {
        return attack_tasks.at(device_pos)->getSpeedTime();
    }

    double AttackDispatch::getDeviceExec(std::size_t device_pos)
    {
        return attack_tasks.at(device_pos)->getExecTime();
    }
}
