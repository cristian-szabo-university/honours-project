#pragma once

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class AttackDispatch;
    class Device;
    class AttackFactory;
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

        HashCracker(DeviceFilter device_filter, bool benchmark);

        ~HashCracker();

        void setStatus(Status status);

        Status getStatus();

        bool benchmarkEnable();

        std::shared_ptr<AttackDispatch> getAttackDispatch();

        std::uint64_t getTotalMessageSize();

        std::chrono::system_clock::time_point getTimeStart();

        std::chrono::seconds getTimeRunning();

        std::chrono::system_clock::time_point getTimeFinish();

        std::chrono::seconds getTimeEstimated();

        std::future<std::string> executeAttack(const std::vector<std::string>& input, std::shared_ptr<AttackFactory> attack_factory);

    private:

        std::atomic<Status> status;

        bool benchmark;

        std::uint64_t total_message_size;

        std::chrono::system_clock::time_point start_time;

        std::vector<std::shared_ptr<Device>> devices;

        std::shared_ptr<AttackDispatch> attack_dispatch;

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
