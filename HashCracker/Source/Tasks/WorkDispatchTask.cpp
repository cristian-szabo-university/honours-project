#include "Config.hpp"

#include "Tasks/WorkDispatchTask.hpp"

#include "Core/HashCracker.hpp"

#include "OpenCL/Device.hpp"

#include "Tasks/AttackTask.hpp"

namespace HonoursProject
{
    WorkDispatchTask::~WorkDispatchTask()
    {
    }

    std::string WorkDispatchTask::run(std::shared_ptr<HashCracker> hash_cracker, std::vector<std::shared_ptr<AttackTask>> attack_tasks, std::uint64_t total_batch_size)
    {
        std::string hash_plain;

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

                attack_futures.push_back(std::async(std::launch::async, &AttackTask::run, task.get(), hash_cracker));
            }

            while (attack_futures.size() > 0)
            {
                auto future_iter = std::find_if(attack_futures.begin(), attack_futures.end(),
                    [](std::future<std::string>& future)
                {
                    std::future_status status = future.wait_for(std::chrono::nanoseconds(1));

                    return (status == std::future_status::ready);
                });

                if (future_iter != attack_futures.end())
                {
                    std::string result = future_iter->get();

                    if (!result.empty())
                    {
                        hash_plain = result;
                    }

                    std::size_t attack_pos = std::distance(future_iter, attack_futures.end()) - 1;

                    std::shared_ptr<AttackTask> attack_task = attack_tasks.at(attack_pos);

                    total_message_tested += (attack_task->getBatchSize() * attack_task->getInnerLoopSize());

                    future_iter = std::rotate(future_iter, future_iter + 1, attack_futures.end());
                }

                future_iter = attack_futures.erase(future_iter, attack_futures.end());
            }
        }

        return hash_plain;
    }

    std::uint64_t WorkDispatchTask::getTotalBatchSize()
    {
        return total_batch_size;
    }

    std::uint64_t WorkDispatchTask::getTotalMessageTested()
    {
        return total_message_tested;
    }
}
