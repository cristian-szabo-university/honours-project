#pragma once

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class Device;
    class HashCracker;
    class AttackTask;

    class HASH_CRACKER_PUBLIC CrackerTask : public TTask<std::string>
    {
    public:

        enum class Status : std::int32_t
        {
            Invalid,
            Idle,
            Running,
            Aborted,
            Cracked,
            Paused,
            MaxCount
        };

        CrackerTask(bool benchmark);

        virtual ~CrackerTask();

        virtual std::string run() override;

        virtual void transfer(std::shared_ptr<BaseTask> task) override;

        bool benchmarkEnable();

        void setStatus(Status status);

        Status getStatus();

        std::chrono::system_clock::time_point getTimeStart();

        std::chrono::seconds getTimeRunning();

        std::chrono::system_clock::time_point getTimeFinish();

        std::chrono::seconds getTimeEstimated();

        std::uint64_t getTotalMessageSize();

        std::uint64_t getTotalMessageProgress();

        std::size_t getDeviceNum();

        std::shared_ptr<Device> getDeviceAt(std::size_t device_pos);

        double getDeviceSpeed(std::size_t device_pos);

        double getDeviceExec(std::size_t device_pos);

    private:

        std::atomic<Status> status;

        bool benchmark;

        std::vector<std::shared_ptr<AttackTask>> attack_tasks;

        std::uint64_t total_batch_size;

        std::uint64_t total_message_size;

        std::uint64_t total_message_progress;

        std::chrono::system_clock::time_point start_time;

    };

    inline std::ostream & operator<<(std::ostream & os, const CrackerTask::Status & status)
    {
        switch (status)
        {
            case CrackerTask::Status::Aborted: os << "Aborted"; break;
            case CrackerTask::Status::Cracked: os << "Cracked"; break;
            case CrackerTask::Status::Idle: os << "Idle"; break;
            case CrackerTask::Status::Paused: os << "Paused"; break;
            case CrackerTask::Status::Running: os << "Running"; break;
        }

        return os;
    }
}
