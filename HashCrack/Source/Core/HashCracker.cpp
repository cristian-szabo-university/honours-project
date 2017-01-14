#include "Config.hpp"

#include "Core/HashCracker.hpp"

#include "Core/Logger.hpp"
#include "Core/Charset.hpp"

#include "OpenCL/Device.hpp"

#include "Tasks/SetupTask.hpp"
#include "Tasks/BruteforceSetupTask.hpp"
#include "Tasks/AttackTask.hpp"
#include "Tasks/WorkDispatchTask.hpp"
#include "Tasks/AutotuneTask.hpp"

namespace HonoursProject
{
    HashCracker::HashCracker(const std::string& hash_msg, HashFunc hash_func, bool benchmark)
        : hash_msg(hash_msg), hash_func(hash_func), benchmark(benchmark), status(HashCracker::Status::Idle)
    {
        if (hash_msg.empty())
        {
            throw std::runtime_error("Error: Hash message can't be empty!");
        }

        if (hash_func == HashFunc::Invalid || hash_func > HashFunc::MaxCount)
        {
            throw std::runtime_error("Error: Invalid hash function!");
        }
    }

    HashCracker::~HashCracker()
    {
    }

    void HashCracker::setStatus(Status status)
    {
        this->status = status;
    }

    HashCracker::Status HashCracker::getStatus()
    {
        return status;
    }

    void HashCracker::setPlainMsg(const std::string & plain_msg)
    {
        this->plain_msg = plain_msg;
    }

    std::string HashCracker::getPlainMsg()
    {
        worker_future.get();

        return plain_msg;
    }

    void HashCracker::resume()
    {
        if (status == Status::Paused && status != Status::Cracked)
        {
            status = Status::Running;
        }
    }

    void HashCracker::pause()
    {
        if (status == Status::Running && status != Status::Cracked)
        {
            status = Status::Paused;
        }
    }

    void HashCracker::quit()
    {
        if (status != Status::Cracked)
        {
            status = Status::Aborted;
        }
    }

    std::string HashCracker::getHashMsg()
    {
        return hash_msg;
    }

    std::string HashCracker::getHashFunc()
    {
        std::string result;

        switch (hash_func)
        {
        case HonoursProject::HashCracker::HashFunc::MD5:
            result = "MD5";
            break;

        case HonoursProject::HashCracker::HashFunc::SHA1:
            result = "SHA1";
            break;

        default:
            result = "Unknown";
            break;
        }

        return result;
    }

    std::string HashCracker::getTimeStart()
    {
        std::string result = std::ctime(&start_time);

        result = KernelPlatform::CleanCLString(result);

        return result;
    }

    std::string HashCracker::getTimeEstimate()
    {
        double hash_speed_ms = 0.0;

        for (auto device : devices)
        {
            hash_speed_ms += work_dispatch->getSpeedTime(device);
        }

        hash_speed_ms /= devices.size();

        double eta_time = total_message_count / hash_speed_ms;

        // Convert to milliseconds from seconds
        eta_time /= 1000;

        std::time_t finish_time = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::from_time_t(start_time) + 
            std::chrono::seconds(static_cast<long>(eta_time)));

        std::string finish_time_str = std::ctime(&finish_time);

        finish_time_str = KernelPlatform::CleanCLString(finish_time_str);

        return finish_time_str;
    }

    std::string HashCracker::getProgress()
    {
        std::stringstream ss;
        ss.imbue({ std::locale(), new comma_numpunct() });

        ss << total_message_count;
        ss << " / ";
        ss << work_dispatch->getTotalMessageTested();

        return ss.str();
    }

    std::size_t HashCracker::getDeviceNum()
    {
        return devices.size();
    }

    std::string HashCracker::getDeviceSpeed(std::size_t device_pos)
    {
        std::stringstream ss;
        ss.imbue({ std::locale(), new comma_numpunct() });

        double speed_time = work_dispatch->getSpeedTime(devices.at(device_pos)) * 1000.0;

        char units[7] = { ' ', 'k', 'M', 'G', 'T', 'P', 'E' };

        std::uint32_t level = 0;

        while (speed_time > 99999)
        {
            speed_time /= 1000;

            level++;
        }

        if (level > 0)
        {
            ss << std::setprecision(2) << std::fixed << speed_time << " " << units[level] << "H/s";
        }
        else
        {
            ss << std::setprecision(2) << std::fixed << speed_time << "H/s";
        }

        double exec_time = work_dispatch->getExecTime(devices.at(device_pos));

        ss << " (" << std::setprecision(2) << std::fixed << exec_time << " ms)";

        return ss.str();
    }

    void HashCracker::executeAttack(AttackMode attack_mode, const std::string & input, DeviceFilter device_filter)
    {
        cl_int cl_error = CL_SUCCESS;

        std::uint64_t total_batch_size = 0;

        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        if (platforms.size() == 0)
        {
            throw std::runtime_error("Error: No platform found!");
        }

        for (auto& platform : platforms)
        {
            cl_device_type filter;

            switch (device_filter)
            {
            case DeviceFilter::CPU_ONLY:
                filter = CL_DEVICE_TYPE_CPU;
                break;

            case DeviceFilter::GPU_ONLY:
                filter = CL_DEVICE_TYPE_GPU;
                break;

            case DeviceFilter::CPU_GPU:
                filter = CL_DEVICE_TYPE_ALL;
                break;
            }

            std::vector<std::shared_ptr<Device>> platform_devices = Device::Create(platform, filter);

            std::copy(platform_devices.begin(), platform_devices.end(), std::back_inserter(devices));
        }

        if (devices.size() == 0)
        {
            throw std::runtime_error("Error: No device found to execute attack!");
        }

        std::vector<std::shared_ptr<SetupTask>> setup_tasks;

        std::vector<std::future<std::shared_ptr<AttackTask>>> setup_futures;

        for (auto& device : devices)
        {
            std::shared_ptr<SetupTask> setup_task;

            switch (attack_mode)
            {
            case HashCracker::AttackMode::Bruteforce:
                setup_task = std::make_shared<BruteforceSetupTask>(shared_from_this(), device, hash_msg, hash_func, input);
                break;

            case HashCracker::AttackMode::Dictionary:
                throw std::runtime_error("Error: Attack mode not implemented yet!");
                break;
            }

            setup_futures.push_back(std::async(std::launch::async, &SetupTask::run, setup_task.get()));

            setup_tasks.push_back(setup_task);
        }

        std::vector<std::shared_ptr<AttackTask>> attack_tasks;

        while (setup_futures.size() > 0)
        {
            auto future_iter = std::remove_if(setup_futures.begin(), setup_futures.end(),
                [](const std::future<std::shared_ptr<AttackTask>>& future)
            {
                std::future_status status = future.wait_for(std::chrono::nanoseconds(1));

                return (status == std::future_status::ready);
            });

            std::for_each(future_iter, setup_futures.end(),
                [&](std::future<std::shared_ptr<AttackTask>>& future)
            {
                std::shared_ptr<AttackTask> attack_task;

                try
                {
                    attack_task = future.get();
                }
                catch (std::exception ex)
                {
                    Logger::error(ex.what());
                }

                if (attack_task)
                {
                    attack_tasks.push_back(attack_task);
                }
            });

            future_iter = setup_futures.erase(future_iter, setup_futures.end());
        }

        if (attack_tasks.size() == 0)
        {
            throw std::runtime_error("Error: No attack was able to create for any device!");
        }

        total_batch_size = setup_tasks.front()->getTotalBatchSize();

        total_message_count = setup_tasks.front()->getTotalBatchSize() * setup_tasks.front()->getInnerLoopSize();

        setup_tasks.clear();

        std::vector<std::shared_ptr<AutotuneTask>> autotune_tasks;

        std::vector<std::future<void>> autotune_futures;

        for (auto& attack_task : attack_tasks)
        {
            std::shared_ptr<AutotuneTask> autotune_task = std::make_shared<AutotuneTask>(attack_task);

            autotune_futures.push_back(std::async(std::launch::async, &AutotuneTask::run, autotune_task.get()));

            autotune_tasks.push_back(autotune_task);
        }

        while (autotune_futures.size() > 0)
        {
            auto future_iter = std::remove_if(autotune_futures.begin(), autotune_futures.end(), 
                [](const std::future<void>& future)
            {
                std::future_status status = future.wait_for(std::chrono::milliseconds(10));

                return (status == std::future_status::ready);
            });

            future_iter = autotune_futures.erase(future_iter, autotune_futures.end());
        }

        autotune_tasks.clear();

        work_dispatch = std::make_shared<WorkDispatchTask>(shared_from_this(), attack_tasks, total_message_count);

        worker_future = std::async(std::launch::async, &WorkDispatchTask::run, work_dispatch.get());

        start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }
}
