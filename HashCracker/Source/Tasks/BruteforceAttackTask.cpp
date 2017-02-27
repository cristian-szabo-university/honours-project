#include "Config.hpp"

#include "Tasks/BruteforceAttackTask.hpp"

#include "Core/Charset.hpp"

#include "OpenCL/Kernel.hpp"
#include "OpenCL/Device.hpp"

namespace HonoursProject
{
    BruteforceAttackTask::BruteforceAttackTask(
        std::shared_ptr<HashCracker> hash_cracker,
        std::shared_ptr<Kernel> kernel_gen_word_prefix, 
        std::shared_ptr<Kernel> kernel_gen_word_suffix, 
        std::shared_ptr<Kernel> kernel_hash_crack, 
        std::vector<Charset> charsets)
        : 
        AttackTask(hash_cracker, kernel_hash_crack),
        kernel_gen_word_prefix(kernel_gen_word_prefix),
        kernel_gen_word_suffix(kernel_gen_word_suffix),
        charsets(charsets)
    {
        std::vector<bool> param_check =
        {
            kernel_gen_word_prefix->hasParam("msg_prefix"),
            kernel_gen_word_prefix->hasParam("msg_batch_size"),
            kernel_gen_word_prefix->hasParam("msg_batch_offset"),

            kernel_gen_word_suffix->hasParam("msg"),
            kernel_gen_word_suffix->hasParam("msg_batch_size"),
            kernel_gen_word_suffix->hasParam("msg_batch_offset")
        };

        for (bool test : param_check)
        {
            if (!test)
            {
                throw std::runtime_error("Kernel does not have a required parameter!");
            }
        }

        kernel_gen_word_prefix->findParam("msg_prefix")->toggleAutoSync();
        kernel_gen_word_suffix->findParam("msg")->toggleAutoSync();
    }

    BruteforceAttackTask::~BruteforceAttackTask()
    {
    }

    std::string BruteforceAttackTask::run()
    {
        kernel_gen_word_suffix->execute(batch_size, device->getMaxWorkGroupSize());

        KernelParam::Copy("msg", kernel_gen_word_suffix, kernel_hash_crack, batch_size);

        AttackTask::run();

        if (!msg_idx.found)
        {
            return std::string();
        }

        std::uint64_t hash_rank;

        hash_rank = batch_offset;
        hash_rank += msg_idx.msg_batch_pos;
        hash_rank *= inner_loop_size;
        hash_rank += inner_loop_pos + msg_idx.inner_loop_pos;

        return Charset::GetMsgFromIndex(hash_rank, charsets);
    }

    void BruteforceAttackTask::setBatchSize(std::uint32_t batch_size)
    {
        AttackTask::setBatchSize(batch_size);

        kernel_gen_word_suffix->setParam("msg_batch_size", batch_size);

        kernel_gen_word_suffix->setParam("msg", std::vector<KernelPlatform::message_t>(batch_size));
    }

    void BruteforceAttackTask::setBatchOffset(std::uint64_t batch_offset)
    {
        AttackTask::setBatchOffset(batch_offset);

        kernel_gen_word_suffix->setParam("msg_batch_offset", batch_offset);
    }

    void BruteforceAttackTask::setInnerLoopStep(std::uint32_t inner_loop_step)
    {
        AttackTask::setInnerLoopStep(inner_loop_step);

        kernel_gen_word_prefix->setParam("msg_prefix", std::vector<KernelPlatform::message_prefix_t>(inner_loop_step));
    }

    void BruteforceAttackTask::preKernelExecute(std::uint32_t inner_loop_left, std::uint32_t inner_loop_pos)
    {
        kernel_gen_word_prefix->setParam("msg_batch_size", inner_loop_left);

        kernel_gen_word_prefix->setParam("msg_batch_offset", inner_loop_pos);

        kernel_gen_word_prefix->execute(inner_loop_left, device->getMaxWorkGroupSize());

        KernelParam::Copy("msg_prefix", kernel_gen_word_prefix, kernel_hash_crack, inner_loop_left);
    }
}
