#pragma once

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class AttackTask;
    class HashCracker;
    class Device;

    class HASHCRACK_PUBLIC WorkDispatchTask : public TTask<std::string>
    {
    public:

        WorkDispatchTask(std::shared_ptr<HashCracker> hash_cracker, std::vector<std::shared_ptr<AttackTask>> attack_tasks, std::uint64_t total_batch_size);

        virtual ~WorkDispatchTask();

        virtual std::string run() override;

        std::uint64_t getTotalBatchSize();

        std::uint64_t getTotalMessageTested();

        double getExecTime(std::shared_ptr<Device> device);

        double getSpeedTime(std::shared_ptr<Device> device);

    private:

        std::shared_ptr<HashCracker> hash_cracker;

        std::vector<std::shared_ptr<AttackTask>> attack_tasks;

        std::uint64_t total_batch_size;

        std::uint64_t total_message_tested;

    };
}
