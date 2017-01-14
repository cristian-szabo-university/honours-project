#pragma once

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class WorkDispatchTask;
    class Device;

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

        enum class HashFunc : std::int32_t
        {
            Invalid = -1,
            MD5,
            SHA1,
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

        HashCracker(const std::string& hash_msg, HashFunc hash_func, bool benchmark);

        ~HashCracker();

        void setStatus(Status status);

        Status getStatus();

        void setPlainMsg(const std::string& plain_msg);

        std::string getPlainMsg();

        void resume();

        void pause();

        void quit();

        std::string getHashMsg();

        std::string getHashFunc();

        std::string getTimeStart();

        std::string getTimeEstimate();

        std::string getProgress();

        std::size_t getDeviceNum();

        std::string getDeviceSpeed(std::size_t device_pos);

        void executeAttack(AttackMode attack_mode, const std::string& input, DeviceFilter device_filter = DeviceFilter::GPU_ONLY);

    private:

        std::atomic<Status> status;

        bool benchmark;

        std::string hash_msg;

        std::string plain_msg;

        HashFunc hash_func;

        std::uint64_t total_message_count;

        std::time_t start_time;

        std::vector<std::shared_ptr<Device>> devices;

        std::shared_ptr<WorkDispatchTask> work_dispatch;

        std::future<void> worker_future;

    };
}
