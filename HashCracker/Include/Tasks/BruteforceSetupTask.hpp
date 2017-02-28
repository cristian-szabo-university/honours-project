#pragma once

#include "Core/HashCracker.hpp"

#include "Tasks/SetupTask.hpp"

namespace HonoursProject
{
    class Charset;

    class BruteforceSetupTask : public SetupTask
    {
    public:

        BruteforceSetupTask(std::shared_ptr<HashCracker> hash_cracker, std::shared_ptr<Device> device, const std::string& hash_msg, std::shared_ptr<HashFunc> hash_func, const std::string& mask);

        virtual ~BruteforceSetupTask();

        virtual std::shared_ptr<AttackTask> run() override;

    private:

        std::vector<Charset> charsets;

    };
}
