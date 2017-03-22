/**
    BruteforceSetupTask.cpp

    Project Name: HashCracker

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#include "Config.hpp"

#include "Tasks/Bruteforce/BruteforceSetupTask.hpp"

#include "ProgramEntry.hpp"

#include "Core/Logger.hpp"
#include "Core/MessageMask.hpp"
#include "Core/HashFactory.hpp"

#include "OpenCL/Program.hpp"
#include "OpenCL/Kernel.hpp"
#include "OpenCL/Device.hpp"

namespace HonoursProject
{
    BruteforceSetupTask::BruteforceSetupTask(std::vector<std::string> input, std::shared_ptr<HashFactory> hash_factory)
        : input(input), hash_factory(hash_factory)
    {
        comma_locale = std::locale(std::locale(), new HonoursProject::Platform::comma_num_punct());
    }

    BruteforceSetupTask::~BruteforceSetupTask()
    {
    }

    bool BruteforceSetupTask::run()
    {
        std::stringstream ss;
        ss.imbue(comma_locale);

        if (input.size() != 2)
        {
            return false;
        }

        if (!message_mask.create(input[1]))
        {
            return false;
        }

        total_batch_size = message_mask.getSize();

        if (message_mask.getLength() < 6)
        {
            msg_prefix_size = 1;
        }
        else if (message_mask.getLength() < 7)
        {
            msg_prefix_size = 2;
        }
        else if (message_mask.getLength() < 8)
        {
            msg_prefix_size = 3;
        }
        else
        {
            msg_prefix_size = 4;
        }

        message_size = message_mask.getLength();

        msg_suffix_size = message_size - msg_prefix_size;

        inner_loop_size = static_cast<uint32_t>(message_mask.getSize(msg_prefix_size));

        total_batch_size /= inner_loop_size;

        Logger::info("Attack.Type: ............: Bruteforce\n", message_size);
        Logger::info("Message.Length: .........: %d + %d = %d characters\n", msg_prefix_size, msg_suffix_size, message_size);

        ss << inner_loop_size << " x " << total_batch_size;
        Logger::info("Message.Batch: ..........: %s\n", ss.str().c_str()); ss.clear();

        for (std::size_t digest_pos = 0; digest_pos < 1; digest_pos++)
        {
            std::vector<std::uint32_t> result = hash_factory->parse(input[0]);

            std::copy(result.begin(), result.end(), std::back_inserter(msg_dgst));
        }

        return true;
    }

    MessageMask BruteforceSetupTask::getMessageMask()
    {
        return message_mask;
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


