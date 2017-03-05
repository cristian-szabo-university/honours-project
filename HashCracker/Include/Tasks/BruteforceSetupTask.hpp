#pragma once

#include "Core/HashCracker.hpp"

#include "Tasks/SetupTask.hpp"

namespace HonoursProject
{
    class Charset;

    class HASHCRACK_PUBLIC BruteforceSetupTask : public SetupTask
    {
    public:

        virtual ~BruteforceSetupTask();

        virtual bool run(std::vector<std::string> input) override;

        std::vector<Charset> getCharsets();

        std::uint32_t getMessageSize();

        std::uint32_t getMessagePrefix();

        std::uint32_t getMessageSuffix();

    protected:

        std::vector<Charset> charsets;

        std::uint32_t message_size;
        
        std::uint32_t msg_prefix_size;
        
        std::uint32_t msg_suffix_size;

    };
}
