#pragma once

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class WorkDispatchTask;
    class Device;
    class HashFunc;

    class HASHCRACK_PUBLIC HashCracker : public std::enable_shared_from_this<HashCracker>
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

        enum class AttackMode : std::int32_t
        {
            Invalid = -1,
            Bruteforce,
            Dictionary,
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

        std::string getHashFunc();

        std::chrono::system_clock::time_point getTimeStart();

        std::chrono::seconds getTimeRunning();

        std::chrono::system_clock::time_point getTimeFinish();

        std::chrono::seconds getTimeEstimated();

        std::uint64_t getMessageTotal();

        std::uint64_t getMessageProgress();

        std::size_t getDeviceNum();

        double getDeviceSpeed(std::size_t device_pos);

        double getDeviceExec(std::size_t device_pos);

        std::future<std::string> executeAttack(AttackMode attack_mode, const std::string& input, DeviceFilter device_filter = DeviceFilter::GPU_ONLY);

    private:

        std::atomic<Status> status;

        bool benchmark;

        std::string hash_msg;

        std::string plain_msg;

        std::shared_ptr<HashFunc> hash_func;

        std::uint64_t total_message_count;

        std::chrono::system_clock::time_point start_time;

        std::vector<std::shared_ptr<Device>> devices;

        std::shared_ptr<WorkDispatchTask> work_dispatch;

        std::future<void> worker_future;

    };
}
