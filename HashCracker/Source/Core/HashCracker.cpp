#include "Config.hpp"

#include "Core/HashCracker.hpp"

#include "Core/AttackDispatch.hpp"
#include "Core/Logger.hpp"
#include "Core/Charset.hpp"
#include "Core/AttackFactory.hpp"

#include "OpenCL/Device.hpp"

#include "Tasks/KernelTask.hpp"
#include "Tasks/SetupTask.hpp"
#include "Tasks/AttackTask.hpp"
#include "Tasks/Bruteforce/BruteforceSetupTask.hpp"
#include "Tasks/AttackTask.hpp"
#include "Tasks/AutotuneTask.hpp"

namespace HonoursProject
{
    HashCracker::HashCracker(DeviceFilter device_filter, bool benchmark)
        : benchmark(benchmark), status(HashCracker::Status::Idle)
    {
        std::vector<cl::Platform> cl_platforms;

        try
        {
            cl::Platform::get(&cl_platforms);
        }
        catch (cl::Error& ex)
        {
            if (ex.err() != CL_PLATFORM_NOT_FOUND_KHR)
            {
                throw ex;
            }

            throw std::runtime_error("Error: No platform found!");
        }

        for (auto& cl_platform : cl_platforms)
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
                filter = CL_DEVICE_TYPE_CPU | CL_DEVICE_TYPE_GPU;
                break;
            }

            std::vector<std::shared_ptr<Device>> platform_devices = Device::Create(cl_platform, filter);

            std::copy(platform_devices.begin(), platform_devices.end(), std::back_inserter(devices));
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

    bool HashCracker::benchmarkEnable()
    {
        return benchmark;
    }

    std::shared_ptr<AttackDispatch> HashCracker::getAttackDispatch()
    {
        return attack_dispatch;
    }

    std::uint64_t HashCracker::getTotalMessageSize()
    {
        return total_message_size;
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

        for (std::size_t device_pos = 0; device_pos < attack_dispatch->getDeviceNum(); device_pos++)
        {
            hash_speed_ms += attack_dispatch->getDeviceSpeed(device_pos);
        }

        hash_speed_ms /= attack_dispatch->getDeviceNum();

        double eta_time = total_message_size / hash_speed_ms / 1000.0;

        return std::chrono::seconds(static_cast<std::uint64_t>(eta_time));
    }

    std::future<std::string> HashCracker::executeAttack(const std::vector<std::string>& input, std::shared_ptr<AttackFactory> attack_factory)
    {
        if (devices.size() == 0)
        {
            throw std::runtime_error("Error: No device found to execute attack!");
        }

        Logger::info("# Setup Attack\n\n");

        std::shared_ptr<SetupTask> setup_task = attack_factory->createSetupTask(input);

        auto future = std::async(std::launch::async, &SetupTask::run, setup_task.get());

        if (!future.get())
        {
            throw std::runtime_error("Error:: Failed to setup attack!");
        }

        Logger::info("\n");

        total_message_size = setup_task->getTotalBatchSize() * setup_task->getInnerLoopSize();

        Logger::info("# Kernel Setup\n\n");

        std::vector<std::shared_ptr<KernelTask>> kernel_tasks;

        for (auto& device : devices)
        {
            std::shared_ptr<KernelTask> kernel_task = attack_factory->createKernelTask(device);

            kernel_task->transfer(setup_task);

            kernel_tasks.push_back(kernel_task);
        }

        std::vector<std::shared_ptr<AttackTask>> attack_tasks;

        Platform::ExecuteTasks(kernel_tasks,
            [&](std::shared_ptr<KernelTask> kernel_task, std::future<void> future, std::size_t index)
        {
            try
            {
                future.get();

                std::shared_ptr<AttackTask> attack_task = attack_factory->createAttakTask(shared_from_this());
                attack_task->transfer(setup_task);
                attack_task->transfer(kernel_task);

                attack_tasks.push_back(attack_task);
            }
            catch (std::exception& ex)
            {
                Logger::error("Error: Failed to setup attack! %s", ex.what());
            }
        });

        kernel_tasks.clear();

        Logger::info("\n");

        if (attack_tasks.size() == 0)
        {
            throw std::runtime_error("Error: No attack was able to create for any device!");
        }

        Logger::info("# Autotune Devices\n\n");

        std::vector<std::shared_ptr<AutotuneTask>> autotune_tasks;

        for (auto& attack_task : attack_tasks)
        {
            std::shared_ptr<AutotuneTask> autotune_task = std::make_shared<AutotuneTask>(Platform::AUTOTUNE_TARGET_SPEED);

            autotune_task->transfer(attack_task);

            autotune_tasks.push_back(autotune_task);
        }

        Platform::ExecuteTasks(autotune_tasks,
            [&](std::shared_ptr<AutotuneTask> autotune_task, std::future<void>& future, std::size_t index)
        {
            try
            {
                future.get();

                std::shared_ptr<AttackTask> attack_task = attack_tasks.at(index);
                attack_task->transfer(autotune_task);
            }
            catch (std::exception& ex)
            {
                Logger::error("Error: Failed to adjust attack speed! %s", ex.what());
            }
        });

        autotune_tasks.clear();

        Logger::info("\n");

        start_time = std::chrono::system_clock::now();

        attack_dispatch = std::make_shared<AttackDispatch>(shared_from_this(), attack_tasks, setup_task->getTotalBatchSize());

        return std::async(std::launch::async, &AttackDispatch::execute, attack_dispatch.get());
    }
}
