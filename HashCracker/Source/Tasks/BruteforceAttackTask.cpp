#include "Config.hpp"

#include "Tasks/BruteforceAttackTask.hpp"

#include "Core/Charset.hpp"

#include "OpenCL/Kernel.hpp"
#include "OpenCL/Device.hpp"

#include "Tasks/KernelTask.hpp"
#include "Tasks/BruteforceKernelTask.hpp"

namespace HonoursProject
{
    BruteforceAttackTask::~BruteforceAttackTask()
    {
    }

    std::string BruteforceAttackTask::run(std::shared_ptr<HashCracker> hash_cracker)
    {
        std::shared_ptr<Device> device = kernel_hash_crack->getProgram()->getDevice();

        kernel_gen_word_suffix->execute(batch_size, device->getMaxWorkGroupSize());

        KernelParam::Copy("msg", kernel_gen_word_suffix, kernel_hash_crack, batch_size);

        AttackTask::run(hash_cracker);

        if (!msg_idx.found)
        {
            return std::string();
        }

        std::uint64_t hash_rank = batch_offset;

        hash_rank += msg_idx.msg_batch_pos;
        hash_rank *= inner_loop_size;
        hash_rank += inner_loop_pos + msg_idx.inner_loop_pos;

        return Charset::GetMsgFromIndex(hash_rank, charsets);
    }

    void BruteforceAttackTask::transfer(std::shared_ptr<BaseTask> task)
    {
        AttackTask::transfer(task);

        std::shared_ptr<BruteforceKernelTask> cast_task = std::dynamic_pointer_cast<BruteforceKernelTask>(task);

        if (cast_task)
        {
            charsets = cast_task->getCharsets();

            kernel_gen_word_prefix = cast_task->getGenWordPrefixKernel();

            kernel_gen_word_suffix = cast_task->getGenWordSuffixKernel();
        }
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
        std::shared_ptr<Device> device = kernel_gen_word_prefix->getProgram()->getDevice();

        kernel_gen_word_prefix->setParam("msg_batch_size", inner_loop_left);

        kernel_gen_word_prefix->setParam("msg_batch_offset", inner_loop_pos);

        kernel_gen_word_prefix->execute(inner_loop_left, device->getMaxWorkGroupSize());

        KernelParam::Copy("msg_prefix", kernel_gen_word_prefix, kernel_hash_crack, inner_loop_left);
    }
}
