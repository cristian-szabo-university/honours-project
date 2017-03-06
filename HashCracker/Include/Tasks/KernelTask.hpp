#pragma once

#include "Tasks/Task.hpp"

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class Device;
    class Kernel;
    class SetupTask;
    class HashFunc;

    class HASH_CRACKER_PUBLIC KernelTask : public TTask<void, std::shared_ptr<Device>, std::shared_ptr<HashFunc>, std::vector<std::uint32_t>>
    {
    public:

        virtual ~KernelTask();

        std::shared_ptr<Kernel> getKernel();

    protected:

        std::shared_ptr<Kernel> kernel_hash_crack;

    };
}
