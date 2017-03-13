#pragma once

#include "Core/Charset.hpp"

#include "Tasks/SetupTask.hpp"

namespace HonoursProject
{
    class HASH_CRACKER_PUBLIC BruteforceSetupTask : public SetupTask
    {
    public:

        BruteforceSetupTask(std::vector<std::string> input, std::shared_ptr<HashFactory> hash_factory);

        virtual ~BruteforceSetupTask();

        virtual bool run() override;

        std::vector<Charset> getCharsets();

        std::uint32_t getMessageSize();

        std::uint32_t getMessagePrefix();

        std::uint32_t getMessageSuffix();

    protected:

        std::vector<std::string> input;
        
        std::shared_ptr<HashFactory> hash_factory;

        std::vector<Charset> charsets;

        std::uint32_t message_size;
        
        std::uint32_t msg_prefix_size;
        
        std::uint32_t msg_suffix_size;

        std::locale comma_locale;

    };
}
