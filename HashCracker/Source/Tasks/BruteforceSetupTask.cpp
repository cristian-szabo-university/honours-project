#include "Config.hpp"

#include "Tasks/BruteforceSetupTask.hpp"

#include "ProgramEntry.hpp"

#include "Core/Logger.hpp"
#include "Core/Charset.hpp"
#include "Core/HashFunc.hpp"

#include "OpenCL/Program.hpp"
#include "OpenCL/Kernel.hpp"
#include "OpenCL/Device.hpp"

#include "Tasks/BruteforceAttackTask.hpp"

namespace HonoursProject
{
    BruteforceSetupTask::~BruteforceSetupTask()
    {
    }

    bool BruteforceSetupTask::run(std::vector<std::string> input)
    {
        if (!input.size())
        {
            return false;
        }

        charsets = Charset::Create(input.front());

        if (!charsets.size())
        {
            Logger::error("Error: Invalid charset!\n");

            return false;
        }

        total_batch_size = Charset::GetTotalMsgNum(charsets);

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

        message_size = charsets.size();

        msg_suffix_size = message_size - msg_prefix_size;

        inner_loop_size = static_cast<uint32_t>(Charset::GetTotalMsgNum(charsets, msg_prefix_size));

        total_batch_size /= inner_loop_size;

        Logger::info("Attack.Type: ............: Bruteforce\n", message_size);
        Logger::info("Password.Length: ........: %d characters\n", message_size);
        Logger::info("Batch.Prefix.Suffix: ....: %d + %d\n", msg_prefix_size, msg_suffix_size);

        return true;
    }

    std::vector<Charset> BruteforceSetupTask::getCharsets()
    {
        return charsets;
    }
    std::uint32_t BruteforceSetupTask::getMessageSize()
    {
        return message_size;
    }

    std::uint32_t BruteforceSetupTask::getMessagePrefix()
    {
        return msg_prefix_size;
    }

    std::uint32_t BruteforceSetupTask::getMessageSuffix()
    {
        return msg_suffix_size;
    }
}


