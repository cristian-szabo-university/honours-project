#include "Config.hpp"

#include "Tasks/BruteforceSetupTask.hpp"

#include "ProgramEntry.hpp"

#include "Core/Charset.hpp"

#include "OpenCL/Program.hpp"
#include "OpenCL/Kernel.hpp"
#include "OpenCL/Device.hpp"

#include "Tasks/BruteforceAttackTask.hpp"

namespace HonoursProject
{
    BruteforceSetupTask::BruteforceSetupTask(std::shared_ptr<HashCracker> hash_cracker, std::shared_ptr<Device> device, const std::string & hash_msg, HashCracker::HashFunc hash_func, const std::string & mask)
        : SetupTask(hash_cracker, device, hash_msg, hash_func)
    {
        charsets = Charset::Create(mask);
    }

    BruteforceSetupTask::~BruteforceSetupTask()
    {
    }

    std::shared_ptr<AttackTask> BruteforceSetupTask::run()
    {
        std::vector<KernelPlatform::charset_t> css;
        std::vector<KernelPlatform::message_digest_t> msg_dgst;
        std::uint32_t mask;
        std::uint32_t bit14;
        std::uint32_t bit15;

        switch (hash_func)
        {
        case HashCracker::HashFunc::MD5:
        {
            msg_dgst.push_back(parseMD5Hash(hash_msg));

            mask = 0x80808080;
            bit14 = 1;
            bit15 = 0;
        }
        break;

        case HashCracker::HashFunc::SHA1:
            throw std::runtime_error("Error: Hash function not implemented yet!");
            break;

        default:
            break;
        }

        for (auto& charset : charsets)
        {
            KernelPlatform::charset_t cs;

            memset(&cs, 0, sizeof(KernelPlatform::charset_t));

            std::copy(charset.getData(), charset.getData() + charset.getSize(), cs.data);

            cs.size = charset.getSize();

            css.push_back(cs);
        }

        total_batch_size = Charset::GetTotalMsgNum(charsets);

        std::uint32_t message_size, msg_prefix_size, msg_suffix_size;

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

        inner_loop_size = Charset::GetTotalMsgNum(charsets, msg_prefix_size);

        total_batch_size /= inner_loop_size;

        std::uint32_t device_power = device->getMaxComputeUnits() * device->getMaxWorkGroupSize();

        std::vector< std::string > build_opts;
        build_opts.push_back("-D _unroll");

        cl::Program::Sources program_sources;
        program_sources.push_back(std::make_pair(inc_hash_const.getData().c_str(), inc_hash_const.getData().size()));
        program_sources.push_back(std::make_pair(inc_hash_func.getData().c_str(), inc_hash_func.getData().size()));
        program_sources.push_back(std::make_pair(inc_types.getData().c_str(), inc_types.getData().size()));

        switch (hash_func)
        {
        case HashCracker::HashFunc::MD5:
        {
            program_sources.push_back(std::make_pair(bruteforce_md5.getData().c_str(), bruteforce_md5.getData().size()));
        }
        break;

        case HashCracker::HashFunc::SHA1:
            throw std::runtime_error("Error: Hash function not implemented yet!");
            break;
        }

        std::shared_ptr<Program> prog_hash_crack = Program::Create(device, program_sources, build_opts, CL_QUEUE_PROFILING_ENABLE);

        std::shared_ptr<Kernel> kernel_hash_crack;

        if (message_size < 16)
        {
            kernel_hash_crack = prog_hash_crack->findKernel("hash_crack_4");
        }
        else if (message_size < 32)
        {
            kernel_hash_crack = prog_hash_crack->findKernel("hash_crack_8");
        }
        else
        {
            kernel_hash_crack = prog_hash_crack->findKernel("hash_crack_16");
        }

        kernel_hash_crack->setParam("msg_digest", msg_dgst);

        cl::Program::Sources gen_word_source;
        gen_word_source.push_back(std::make_pair(inc_types.getData().c_str(), inc_types.getData().size()));
        gen_word_source.push_back(std::make_pair(gen_word.getData().c_str(), gen_word.getData().size()));

        std::shared_ptr<Program> prog_gen_word = Program::Create(device, gen_word_source, build_opts, CL_QUEUE_PROFILING_ENABLE);

        std::shared_ptr<Kernel> kernel_gen_word_suffix = prog_gen_word->findKernel("generate_word_suffix");

        kernel_gen_word_suffix->setParam("css_buf", css);
        kernel_gen_word_suffix->setParam("css_count", css.size());
        kernel_gen_word_suffix->setParam("msg_prefix_size", msg_prefix_size);
        kernel_gen_word_suffix->setParam("msg_suffix_size", msg_suffix_size);
        kernel_gen_word_suffix->setParam("mask", mask);
        kernel_gen_word_suffix->setParam("bit14", bit14);
        kernel_gen_word_suffix->setParam("bit15", bit15);

        std::shared_ptr<Kernel> kernel_gen_word_prefix = prog_gen_word->findKernel("generate_word_prefix");

        kernel_gen_word_prefix->setParam("css_buf", css);
        kernel_gen_word_prefix->setParam("css_count", css.size());
        kernel_gen_word_prefix->setParam("msg_prefix_size", 0u);
        kernel_gen_word_prefix->setParam("msg_suffix_size", msg_prefix_size);
        kernel_gen_word_prefix->setParam("mask", 0u);
        kernel_gen_word_prefix->setParam("bit14", 0u);
        kernel_gen_word_prefix->setParam("bit15", 0u);

        std::shared_ptr<BruteforceAttackTask> result = std::make_shared<BruteforceAttackTask>(hash_cracker, kernel_gen_word_prefix, kernel_gen_word_suffix, kernel_hash_crack, charsets);

        result->setBatchSize(device_power);
        result->setInnerLoopSize(inner_loop_size);

        return result;
    }
}


