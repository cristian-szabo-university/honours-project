#pragma once

#include "Tasks/Task.hpp"

#include "Tasks/KernelTask.hpp"

namespace HonoursProject
{
    class HashCracker;
    class AttackTask;
    class Device;
    class HashFunc;
    class Kernel;

    class HASH_CRACKER_PUBLIC BruteforceKernelTask : public KernelTask
    {
    public:

        virtual ~BruteforceKernelTask();

        virtual void run(std::shared_ptr<Device> device, std::shared_ptr<HashFunc> hash_func, std::vector<std::uint32_t> msg_dgst) override;

        virtual void transfer(std::shared_ptr<BaseTask> task) override;

        std::vector<Charset> getCharsets();

        std::shared_ptr<Kernel> getGenWordPrefixKernel();

        std::shared_ptr<Kernel> getGenWordSuffixKernel();

    protected:

        std::vector<Charset> charsets;

        std::uint32_t message_size;

        std::uint32_t msg_prefix_size;

        std::uint32_t msg_suffix_size;

        std::shared_ptr<Kernel> kernel_gen_word_prefix;

        std::shared_ptr<Kernel> kernel_gen_word_suffix;

    };
}
