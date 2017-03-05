#include "Config.hpp"

#include "Core/HashCracker.hpp"

#include "Core/Logger.hpp"
#include "Core/Charset.hpp"
#include "Core/HashFunc.hpp"

#include "OpenCL/Device.hpp"

#include "Tasks/KernelTask.hpp"
#include "Tasks/SetupTask.hpp"
#include "Tasks/AttackTask.hpp"
#include "Tasks/BruteforceSetupTask.hpp"
#include "Tasks/AttackTask.hpp"
#include "Tasks/WorkDispatchTask.hpp"
#include "Tasks/AutotuneTask.hpp"

namespace HonoursProject
{
    HashCracker::HashCracker(const std::string& hash_msg, std::shared_ptr<HashFunc> hash_func, bool benchmark)
        : hash_msg(hash_msg), hash_func(hash_func), benchmark(benchmark), status(HashCracker::Status::Idle)
    {
        if (hash_msg.empty())
        {
            throw std::runtime_error("Error: Hash message can't be empty!");
        }

        work_dispatch = std::make_shared<WorkDispatchTask>();
    }

    HashCracker::~HashCracker()
    {
        work_dispatch.reset();

        attack_tasks.clear();
    }

    void HashCracker::setStatus(Status status)
    {
        this->status = status;
    }

    HashCracker::Status HashCracker::getStatus()
    {
        return status;
    }

    bool HashCracker::benchmarkEnable()
    {
        return benchmark;
    }

    std::string HashCracker::getHashMsg()
    {
        return hash_msg;
    }

    std::shared_ptr<HashFunc> HashCracker::getHashFunc()
    {
        return hash_func;
    }

    std::chrono::system_clock::time_point HashCracker::getTimeStart()
    {
        return start_time;
    }

    std::chrono::seconds HashCracker::getTimeRunning()
    {
        std::chrono::system_clock::time_point now_time = std::chrono::system_clock::now();

        return std::chrono::duration_cast<std::chrono::seconds>(now_time - start_time);
    }

    std::chrono::system_clock::time_point HashCracker::getTimeFinish()
    {
        return start_time + getTimeEstimated();
    }

    std::chrono::seconds HashCracker::getTimeEstimated()
    {
        double hash_speed_ms = 0.0;

        for (auto device : attack_tasks)
        {
            hash_speed_ms += device->getAvgSpeedTime();
        }

        hash_speed_ms /= attack_tasks.size();

        double eta_time = total_message_count / hash_speed_ms / 1000.0;

        return std::chrono::seconds(static_cast<std::uint64_t>(eta_time));
    }

    std::uint64_t HashCracker::getMessageTotal()
    {
        return total_message_count;
    }

    std::uint64_t HashCracker::getMessageProgress()
    {
        return work_dispatch->getTotalMessageTested();
    }

    std::size_t HashCracker::getDeviceNum()
    {
        return attack_tasks.size();
    }

    double HashCracker::getDeviceSpeed(std::size_t device_pos)
    {
        return attack_tasks.at(device_pos)->getAvgSpeedTime();
    }

    double HashCracker::getDeviceExec(std::size_t device_pos)
    {
        return attack_tasks.at(device_pos)->getAvgExecTime();
    }

    std::future<std::string> HashCracker::executeAttack(const std::vector<std::string>& input, DeviceFilter device_filter)
    {
        cl_int cl_error = CL_SUCCESS;

        attack_tasks.clear();

        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        if (platforms.size() == 0)
        {
            throw std::runtime_error("Error: No platform found!");
        }

        std::vector<std::shared_ptr<Device>> devices;

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

        Logger::info("# Setup Attack\n\n");

        std::shared_ptr<SetupTask> setup_task = hash_func->setup_task();

        auto future = std::async(std::launch::async, &SetupTask::run, setup_task.get(), input);

        if (!future.get())
        {
            throw std::runtime_error("Error:: Failed to setup attack!");
        }

        Logger::info("\n");

        std::uint64_t total_batch_size = setup_task->getTotalBatchSize();

        total_message_count = setup_task->getTotalBatchSize() * setup_task->getInnerLoopSize();

        std::vector<std::uint32_t> msg_dgst;

        for (std::size_t digest_pos = 0; digest_pos < 1; digest_pos++)
        {
            std::vector<std::uint32_t> result = hash_func->parse(hash_msg);

            std::copy(result.begin(), result.end(), std::back_inserter(msg_dgst));
        }

        Logger::info("# Kernel Setup\n\n");

        std::vector<std::shared_ptr<KernelTask>> kernel_tasks;

        std::vector<std::future<void>> kernel_futures;

        for (auto& device : devices)
        {
            std::shared_ptr<KernelTask> kernel_task = hash_func->kernel_task();

            kernel_task->transfer(setup_task);

            kernel_tasks.push_back(kernel_task);

            auto future = std::async(std::launch::async, &KernelTask::run, kernel_task.get(), device, hash_func, msg_dgst);

            kernel_futures.push_back(std::move(future));
        }

        while (kernel_futures.size() > 0)
        {
            auto future_iter = std::find_if(kernel_futures.begin(), kernel_futures.end(),
                [](const std::future<void>& future)
            {
                std::future_status status = future.wait_for(std::chrono::nanoseconds(1));

                return (status == std::future_status::ready);
            });

            if (future_iter != kernel_futures.end())
            {
                try
                {
                    future_iter->get();

                    std::size_t kernel_pos = std::distance(future_iter, kernel_futures.end()) - 1;
                    std::shared_ptr<KernelTask> kernel_task = kernel_tasks.at(kernel_pos);

                    std::shared_ptr<AttackTask> attack_task = hash_func->attack_task();
                    attack_task->transfer(setup_task);
                    attack_task->transfer(kernel_task);

                    attack_tasks.push_back(attack_task);
                }
                catch (std::exception& ex)
                {
                    Logger::error("Error: Failed to create kernel program! %s", ex.what());
                }

                future_iter = std::rotate(future_iter, future_iter + 1, kernel_futures.end());
            }

            future_iter = kernel_futures.erase(future_iter, kernel_futures.end());
        }

        Logger::info("\n");

        if (attack_tasks.size() == 0)
        {
            throw std::runtime_error("Error: No attack was able to create for any device!");
        }

        kernel_tasks.clear();

        Logger::info("# Autotune Devices\n\n");

        std::vector<std::shared_ptr<AutotuneTask>> autotune_tasks;

        std::vector<std::future<void>> autotune_futures;

        for (auto& attack_task : attack_tasks)
        {
            std::shared_ptr<AutotuneTask> autotune_task = std::make_shared<AutotuneTask>();

            autotune_tasks.push_back(autotune_task);

            auto future = std::async(std::launch::async, &AutotuneTask::run, autotune_task.get(), attack_task, 12);

            autotune_futures.push_back(std::move(future));
        }

        while (autotune_futures.size() > 0)
        {
            auto future_iter = std::find_if(autotune_futures.begin(), autotune_futures.end(),
                [](const std::future<void>& future)
            {
                std::future_status status = future.wait_for(std::chrono::nanoseconds(1));

                return (status == std::future_status::ready);
            });

            if (future_iter != autotune_futures.end())
            {
                try
                {
                    future_iter->get();
                }
                catch (std::exception& ex)
                {
                    Logger::error("Error: Failed to create kernel program!", ex.what());
                }

                future_iter = std::rotate(future_iter, future_iter + 1, autotune_futures.end());
            }

            future_iter = autotune_futures.erase(future_iter, autotune_futures.end());
        }

        autotune_tasks.clear();

        Logger::info("\n");

        start_time = std::chrono::system_clock::now();

        return std::async(std::launch::async, &WorkDispatchTask::run, work_dispatch.get(), shared_from_this(), attack_tasks, total_batch_size);
    }
}
