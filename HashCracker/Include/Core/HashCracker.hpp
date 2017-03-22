/**
    HashCracker.hpp

    Project Name: HashCracker

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#pragma once

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class CrackerTask;
    class Device;
    class AttackFactory;
    class AttackTask;

    class HASH_CRACKER_PUBLIC HashCracker : public std::enable_shared_from_this<HashCracker>
    {
    public:

        enum class DeviceFilter : std::int32_t
        {
            Invalid = -1,
            CPU_ONLY,
            GPU_ONLY,
            CPU_GPU,
            MaxCount
        };

        HashCracker();

        ~HashCracker();

        bool create(DeviceFilter device_filter);

        bool destroy();

        std::shared_ptr<CrackerTask> createCrackerTask(
            const std::vector<std::string>& input, 
            std::shared_ptr<AttackFactory> attack_factory, 
            bool benchmark);

    private:

        bool ready;

        std::vector<std::shared_ptr<Device>> devices;

    };
}
