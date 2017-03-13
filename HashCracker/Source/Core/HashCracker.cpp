#include "Config.hpp"

#include "Core/HashCracker.hpp"

#include "Core/Logger.hpp"
#include "Core/Charset.hpp"
#include "Core/AttackFactory.hpp"

#include "OpenCL/Device.hpp"

#include "Tasks/CrackerTask.hpp"
#include "Tasks/KernelTask.hpp"
#include "Tasks/SetupTask.hpp"
#include "Tasks/AttackTask.hpp"
#include "Tasks/Bruteforce/BruteforceSetupTask.hpp"
#include "Tasks/AttackTask.hpp"
#include "Tasks/AutotuneTask.hpp"

namespace HonoursProject
{
    HashCracker::HashCracker() : ready(false)
    {
        
    }

    HashCracker::~HashCracker()
    {
    }

    bool HashCracker::create(DeviceFilter device_filter)
    {
        if (ready)
        {
            return false;
        }

        std::string sep_line(32, '-');
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

            Logger::error("Error: No platform found!");

            return false;
        }

        Logger::info("# Find Devices\n\n");

        for (std::size_t platform_id = 0; platform_id < cl_platforms.size(); platform_id++)
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

            cl::Platform cl_platform = cl_platforms.at(platform_id);
            std::string platform_name = cl_platform.getInfo<CL_PLATFORM_NAME>();

            std::vector<cl::Device> platform_devices;

            try
            {
                cl_platform.getDevices(filter, &platform_devices);
            }
            catch (cl::Error& ex)
            {
                if (ex.err() != CL_DEVICE_NOT_FOUND)
                {
                    throw ex;
                }

                continue;
            }

            Logger::info("Platform: %s\n%s\n", platform_name.c_str(), sep_line.c_str());

            for (std::size_t device_id = 0; device_id < platform_devices.size(); device_id++)
            {
                cl::Device cl_device = platform_devices.at(device_id);

                std::shared_ptr<Device> device = std::make_shared<Device>();

                if (!device->create(cl_device))
                {
                    continue;
                }

                devices.push_back(device);

                Logger::info(" + Device #%d: ", device->getId());              
                Logger::info("%s, ", device->getName().c_str()); 
                Logger::info("%u / %u MB (Maximum / Available), ", 
                    (std::size_t) device->getMaxMemAllocSize() / 1024 / 1024,
                    (std::size_t) device->getTotalGlobalMemSize() / 1024 / 1024);
                Logger::info("%u Compute Units\n", device->getMaxComputeUnits());
            }

            Logger::info("\n");
        }

        if (devices.size() == 0)
        {
            Logger::error("Error: No device found to execute attack!");
        }

        ready = !ready;

        return true;
    }

    bool HashCracker::destroy()
    {
        if (!ready)
        {
            return false;
        }

        devices.clear();

        ready = !ready;

        return true;
    }

    std::shared_ptr<CrackerTask> HashCracker::createCrackerTask(
        const std::vector<std::string>& input, 
        std::shared_ptr<AttackFactory> attack_factory,
        bool benchmark)
    {
        std::shared_ptr<CrackerTask> result;

        Logger::info("# Setup Attack\n\n");

        std::shared_ptr<SetupTask> setup_task = attack_factory->createSetupTask(input);

        auto future = std::async(std::launch::async, &SetupTask::run, setup_task.get());

        bool complete = false;

        try
        {
            complete = future.get();
        }
        catch (std::exception& ex)
        {
            Logger::error("Error: Failed to setup attack! %s", ex.what());
        }

        Logger::info("\n");

        if (!complete)
        {
            return result;
        }

        result = std::make_shared<CrackerTask>(benchmark);

        result->transfer(setup_task);

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
            [&](std::shared_ptr<KernelTask> kernel_task, std::future<void>&& future, std::size_t index)
        {
            try
            {
                future.get();

                std::shared_ptr<AttackTask> attack_task = attack_factory->createAttakTask();
                attack_task->transfer(setup_task);
                attack_task->transfer(kernel_task);
                attack_task->transfer(result);

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
            [&](std::shared_ptr<AutotuneTask> autotune_task, std::future<void>&& future, std::size_t index)
        {
            try
            {
                future.get();

                std::shared_ptr<AttackTask> attack_task = attack_tasks.at(index);
                attack_task->transfer(autotune_task);

                result->transfer(attack_task);
            }
            catch (std::exception& ex)
            {
                Logger::error("Error: Failed to adjust attack speed! %s", ex.what());
            }
        });

        autotune_tasks.clear();

        Logger::info("\n");

        return result;
    }
}
