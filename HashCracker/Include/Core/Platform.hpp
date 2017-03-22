/**
    Platform.hpp

    Project Name: HashCracker

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#pragma once

namespace HonoursProject
{
    namespace Platform
    {
        const std::uint32_t CHAR_SIZE = 256;

        const std::uint32_t MIN_MESSAGE_SIZE = 1;
        const std::uint32_t MAX_MESSAGE_SIZE = 54;

        const std::uint32_t MAX_CHARSET_COUNT = 64;

        const std::uint8_t OPENCL_COMPILER_MAJOR_VERSION = '1';
        const std::uint8_t OPENCL_COMPILER_MINOR_VERSION = '1';

        const std::size_t MAX_EXEC_CACHE_SIZE = 128;
        const std::size_t MAX_SPEED_CACHE_SIZE = 128;

        const std::size_t AUTOTUNE_VALIDATE_CHECKS = 3;
        const std::size_t AUTOTUNE_DEVICE_SPEED_TRY = 128;
        const std::size_t AUTOTUNE_KERNEL_LOOPS_TRY = 128;
        const double AUTOTUNE_TARGET_SPEED = 12;

        HASH_CRACKER_PUBLIC std::uint32_t to_int32(const std::uint8_t * bytes);

        HASH_CRACKER_PUBLIC std::string Demangle(const char* name);

        HASH_CRACKER_PUBLIC std::string CleanTypeName(std::string type_name);

        template<class T>
        inline std::string CleanTypeName()
        {
            const char* type_name = typeid(T).name();

            return CleanTypeName(Demangle(type_name));
        }

        template<class R = void, class T, class U>
        inline void ExecuteTasks(std::vector<std::shared_ptr<T>>& tasks, U functor)
        {
            std::size_t tasks_left = tasks.size();
            std::vector<std::future<R>> futures;

            for (auto& task : tasks)
            {
                auto future = std::async(std::launch::async, &T::run, task.get());

                futures.push_back(std::move(future));
            }

            while (tasks_left)
            {
                auto future_iter = std::find_if(futures.begin(), futures.end(),
                    [](const std::future<R>& future)
                {
                    if (!future.valid())
                    {
                        return false;
                    }

                    std::future_status status = future.wait_for(std::chrono::nanoseconds(1));

                    return (status == std::future_status::ready);
                });

                if (future_iter != futures.end())
                {
                    std::size_t index = std::distance(futures.begin(), future_iter);

                    std::shared_ptr<T> task = tasks.at(index);

                    functor(task, std::move(*future_iter), index);
                }

                tasks_left = std::count_if(futures.begin(), futures.end(),
                    [](const std::future<R>& future)
                {
                    return future.valid();
                });
            }
        }

        class comma_num_punct : public std::numpunct<char>
        {
        protected:

            inline virtual char do_thousands_sep() const
            {
                return ',';
            }

            inline virtual std::string do_grouping() const
            {
                return "\03";
            }

        };
    };

    namespace KernelPlatform
    {
        const std::size_t MAX_KERNEL_WORK_GROUP = 256;

        const std::size_t DEVICE_SPEED_MIN = 1;
        const std::size_t DEVICE_SPEED_MAX = 1024;

        const std::size_t KERNEL_LOOPS_MIN = 1;
        const std::size_t KERNEL_LOOPS_MAX = 1024;

        const std::size_t MAX_DEVICE_VECTOR_WIDTH = 16;

        const std::uint32_t MAX_MESSAGE_PREFIX = 1024;

#pragma pack(push, 1)
        struct charset_t
        {
            cl_uchar data[Platform::CHAR_SIZE];

            cl_uint size;
        };

        struct message_t
        {
            cl_uint data[16];

            cl_uint size;
        };

        struct message_prefix_t
        {
            cl_uint data;
        };

        struct message_index_t
        {
            cl_uint found;
            cl_uint msg_batch_pos;
            cl_uint inner_loop_pos;
        };
#pragma pack(pop)
    };
}
