#pragma once

#include "Tasks/Task.hpp"

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class Device;
    class Kernel;
    class SetupTask;
    class HashFactory;

    class HASH_CRACKER_PUBLIC KernelTask : public TTask<void>
    {
    public:

        virtual ~KernelTask();

        virtual void transfer(std::shared_ptr<BaseTask> task) override;

        std::shared_ptr<Kernel> getKernel();

        std::vector<std::uint32_t> getMessageDigest();

    protected:

        std::vector<std::uint32_t> msg_dgst;

        std::shared_ptr<Kernel> kernel_hash_crack;

    };
}
