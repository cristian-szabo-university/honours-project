#pragma once

namespace HonoursProject
{
    class Device;
    class HashCracker;
    class AttackTask;

    class HASH_CRACKER_PUBLIC AttackDispatch
    {
    public:

        AttackDispatch(std::shared_ptr<HashCracker> hash_cracker, std::vector<std::shared_ptr<AttackTask>> attack_tasks, std::uint64_t total_batch_size);

        std::string execute();

        std::uint64_t getTotalMessageProgress();

        std::size_t getDeviceNum();

        std::shared_ptr<Device> getDeviceAt(std::size_t device_pos);

        double getDeviceSpeed(std::size_t device_pos);

        double getDeviceExec(std::size_t device_pos);

    private:

        std::shared_ptr<HashCracker> hash_cracker;

        std::vector<std::shared_ptr<AttackTask>> attack_tasks;

        std::uint64_t total_batch_size;

        std::uint64_t total_message_progress;

    };
}
