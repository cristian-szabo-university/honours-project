#pragma once

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class AttackTask;
    class HashCracker;
    class Device;

    class HASHCRACK_PUBLIC WorkDispatchTask : public TTask<std::string, std::shared_ptr<HashCracker>, std::vector<std::shared_ptr<AttackTask>>, std::uint64_t>
    {
    public:

        virtual ~WorkDispatchTask();

        virtual std::string run(std::shared_ptr<HashCracker> hash_cracker, std::vector<std::shared_ptr<AttackTask>> attack_tasks, std::uint64_t total_batch_size) override;

        std::uint64_t getTotalBatchSize();

        std::uint64_t getTotalMessageTested();

    private:

        std::uint64_t total_batch_size;

        std::uint64_t total_message_tested;

    };
}
