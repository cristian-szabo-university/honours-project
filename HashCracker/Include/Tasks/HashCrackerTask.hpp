#pragma once

#include "Core/Platform.hpp"

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    class Charset;

    class HASHCRACK_PUBLIC HashCrackerTask : public NoReturnTask
    {
    public:

        HashCrackerTask(std::string hash, AttackMode attack_mode, HashFunc hash_func, std::vector<Charset> charsets, DeviceFilter device_filter = DeviceFilter::GPU_ONLY);

        virtual ~HashCrackerTask();

        virtual void run() override;

    private:

        std::string hash;

        AttackMode attack_mode;

        HashFunc hash_func;

        std::vector<Charset> charsets;

        DeviceFilter device_filter;

    };
}
