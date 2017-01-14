#pragma once

#include "Tasks/AttackTask.hpp"

namespace HonoursProject
{
    class Charset;

    class HASHCRACK_PUBLIC BruteforceAttackTask : public AttackTask
    {
    public:

        BruteforceAttackTask(std::shared_ptr<HashCracker> hash_cracker, std::shared_ptr<Kernel> kernel_gen_word_prefix, std::shared_ptr<Kernel> kernel_gen_word_suffix, std::shared_ptr<Kernel> kernel_hash_crack, std::vector<Charset> charsets);

        virtual ~BruteforceAttackTask();

        virtual std::string run() override;

        virtual void setBatchSize(std::uint32_t batch_size) override;

        virtual void setBatchOffset(std::uint64_t batch_offset) override;

        virtual void setInnerLoopStep(std::uint32_t inner_loop_step) override;

    private:

        std::vector<Charset> charsets;

        std::shared_ptr<Kernel> kernel_gen_word_prefix;

        std::shared_ptr<Kernel> kernel_gen_word_suffix;

        virtual void preKernelExecute(std::uint32_t inner_loop_left, std::uint32_t inner_loop_pos) override;

    };
}
