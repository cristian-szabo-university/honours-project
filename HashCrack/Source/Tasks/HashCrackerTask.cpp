#include "Config.hpp"

#include "Tasks/HashCrackerTask.hpp"

#include "ProgramEntry.hpp"

#include "OpenCL/Device.hpp"
#include "OpenCL/Program.hpp"
#include "OpenCL/Kernel.hpp"

#include "Core/Charset.hpp"
#include "Core/Logger.hpp"

#include "Tasks/CrackHashTask.hpp"
#include "Tasks/BruteforceTask.hpp"
#include "Tasks/CrackJobDispatchTask.hpp"
#include "Tasks/AttackTask.hpp"

namespace HonoursProject
{
    HashCrackerTask::HashCrackerTask(std::string hash, AttackMode attack_mode, HashFunc hash_func, std::vector<Charset> charsets, DeviceFilter device_filter)
        : hash(hash), attack_mode(attack_mode), hash_func(hash_func), charsets(charsets), device_filter(device_filter)
    {
    }

    HashCrackerTask::~HashCrackerTask()
    {
    }

    void HashCrackerTask::run()
    {
        cl_int cl_error = CL_SUCCESS;

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
            throw std::runtime_error("Error: No device found!");
        }

        std::uint32_t msg_prefix_size, msg_suffix_size;
        std::uint64_t total_message_count = Charset::GetTotalMsgNum(charsets);

        if (charsets.size() < 6)
        {
            msg_prefix_size = 1;
        }
        else if (charsets.size() < 8)
        {
            msg_prefix_size = 2;
        }
        else if (charsets.size() < 10)
        {
            msg_prefix_size = 3;
        }
        else
        {
            msg_prefix_size = 4;
        }

        msg_suffix_size = charsets.size() - msg_prefix_size;

        std::uint32_t inner_loop_size = (std::uint32_t)Charset::GetTotalMsgNum(charsets, msg_prefix_size);

        std::uint32_t message_size = msg_prefix_size + msg_suffix_size;

        total_message_count /= inner_loop_size;

        std::vector<std::shared_ptr<AttackTask>> setup_tasks;

        std::vector<std::future<std::shared_ptr<CrackHashTask>>> crack_task_futures;

        for (auto& device : devices)
        {
            std::shared_ptr<AttackTask> task = std::make_shared<AttackTask>(attack_mode, hash_func, hash, charsets, msg_prefix_size, msg_suffix_size, device);

            crack_task_futures.push_back(std::async(std::launch::async, &AttackTask::run, task.get()));

            setup_tasks.push_back(task);
        }

        std::vector<std::shared_ptr<CrackHashTask>> crack_tasks;

        for (auto& future : crack_task_futures)
        {
            std::shared_ptr<CrackHashTask> crack_hash_task;

            try
            {
                crack_hash_task = future.get();
            }
            catch (std::exception ex)
            {
                Logger::error("%s", ex.what());

                continue;
            }

            crack_hash_task->setInnerLoopSize(inner_loop_size);

            crack_tasks.push_back(crack_hash_task);
        }

        setup_tasks.clear();

        std::shared_ptr<CrackJobDispatchTask> task = std::make_shared<CrackJobDispatchTask>(crack_tasks, total_message_count);

        std::future<std::int64_t> result = std::async(std::launch::async, &CrackJobDispatchTask::run, task.get());

        std::int64_t hash_pos = result.get();

        if (hash_pos < 0)
        {
            Logger::info("%s: Not found!\n", hash.c_str());
        }
        else
        {
            Logger::info("%s: %s\n", hash.c_str(), Charset::GetMsgFromIndex(hash_pos, charsets).c_str());
        }
    }
}
