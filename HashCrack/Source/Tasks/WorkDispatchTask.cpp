#include "Config.hpp"

#include "Tasks/WorkDispatchTask.hpp"

#include "Core/HashCracker.hpp"

#include "OpenCL/Device.hpp"

#include "Tasks/AttackTask.hpp"

namespace HonoursProject
{
    WorkDispatchTask::WorkDispatchTask(std::shared_ptr<HashCracker> hash_cracker, std::vector<std::shared_ptr<AttackTask>> attack_tasks, std::uint64_t total_batch_size)
        : hash_cracker(hash_cracker), attack_tasks(attack_tasks), total_batch_size(total_batch_size), total_message_tested(0)
    {
    }

    WorkDispatchTask::~WorkDispatchTask()
    {
    }

    void WorkDispatchTask::run()
    {
        std::string hash_msg;

        std::uint64_t batch_offset = 0;
        std::uint64_t batch_left = total_batch_size - batch_offset;

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

                attack_futures.push_back(std::async(std::launch::async, &AttackTask::run, task.get()));
            }

            while (attack_futures.size() > 0 && hash_cracker->getStatus() != HashCracker::Status::Cracked)
            {
                auto future_iter = std::remove_if(attack_futures.begin(), attack_futures.end(),
                    [](std::future<std::string>& future)
                {
                    std::future_status status = future.wait_for(std::chrono::nanoseconds(1));

                    return (status == std::future_status::ready);
                });

                auto future_begin = future_iter;

                std::for_each(future_iter, attack_futures.end(),
                    [&](std::future<std::string>& future)
                {
                    std::string plain_msg = future.get();

                    if (!plain_msg.empty())
                    {
                        hash_cracker->setPlainMsg(plain_msg);
                    }

                    std::size_t attack_pos = std::distance(future_begin, attack_futures.end()) - 1;

                    std::shared_ptr<AttackTask> attack_task = attack_tasks.at(attack_pos);

                    total_message_tested += (attack_task->getBatchSize() * attack_task->getInnerLoopSize());

                    std::advance(future_begin, 1);
                });

                future_iter = attack_futures.erase(future_iter, attack_futures.end());
            }
        }
    }

    std::uint64_t WorkDispatchTask::getTotalBatchSize()
    {
        return total_batch_size;
    }

    std::uint64_t WorkDispatchTask::getTotalMessageTested()
    {
        return total_message_tested;
    }

    double WorkDispatchTask::getExecTime(std::shared_ptr<Device> device)
    {
        double result;

        for (auto& attack_task : attack_tasks)
        {
            if (attack_task->getDevice()->getHandle()() == device->getHandle()())
            {
                result = attack_task->getAvgExecTime();
            }
        }

        return result;
    }

    double WorkDispatchTask::getSpeedTime(std::shared_ptr<Device> device)
    {
        double result;

        for (auto& attack_task : attack_tasks)
        {
            if (attack_task->getDevice()->getHandle()() == device->getHandle()())
            {
                result = attack_task->getAvgSpeedTime();
            }
        }

        return result;
    }
}
