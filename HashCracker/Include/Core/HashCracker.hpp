#pragma once

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class WorkDispatchTask;
    class Device;
    class HashFunc;
    class AttackTask;

    class HASH_CRACKER_PUBLIC HashCracker : public std::enable_shared_from_this<HashCracker>
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

        enum class DeviceFilter : std::int32_t
        {
            Invalid = -1,
            CPU_ONLY,
            GPU_ONLY,
            CPU_GPU,
            MaxCount
        };

        HashCracker(const std::string& hash_msg, std::shared_ptr<HashFunc> hash_func, bool benchmark);

        ~HashCracker();

        void setStatus(Status status);

        Status getStatus();

        bool benchmarkEnable();

        std::string getHashMsg();

        std::shared_ptr<HashFunc> getHashFunc();

        std::chrono::system_clock::time_point getTimeStart();

        std::chrono::seconds getTimeRunning();

        std::chrono::system_clock::time_point getTimeFinish();

        std::chrono::seconds getTimeEstimated();

        std::uint64_t getMessageTotal();

        std::uint64_t getMessageProgress();

        std::size_t getDeviceNum();

        double getDeviceSpeed(std::size_t device_pos);

        double getDeviceExec(std::size_t device_pos);

        std::future<std::string> executeAttack(const std::vector<std::string>& input, DeviceFilter device_filter);

    private:

        std::atomic<Status> status;

        bool benchmark;

        std::string hash_msg;

        std::string plain_msg;

        std::shared_ptr<HashFunc> hash_func;

        std::uint64_t total_message_count;

        std::chrono::system_clock::time_point start_time;

        std::shared_ptr<WorkDispatchTask> work_dispatch;

        std::vector<std::shared_ptr<AttackTask>> attack_tasks;

        std::future<void> worker_future;

    };

    inline std::ostream & operator<<(std::ostream & os, const HashCracker::Status & status)
    {
        switch (status)
        {
        case HonoursProject::HashCracker::Status::Aborted: os << "Aborted"; break;
        case HonoursProject::HashCracker::Status::Cracked: os << "Cracked"; break;
        case HonoursProject::HashCracker::Status::Idle: os << "Idle"; break;
        case HonoursProject::HashCracker::Status::Paused: os << "Paused"; break;
        case HonoursProject::HashCracker::Status::Running: os << "Running"; break;
        }

        return os;
    }
}
