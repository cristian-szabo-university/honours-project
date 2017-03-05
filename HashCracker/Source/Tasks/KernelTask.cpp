#include "Config.hpp"

#include "Tasks/KernelTask.hpp"

#include "Core/Logger.hpp"

namespace HonoursProject
{
    KernelTask::~KernelTask()
    {
    }

    std::shared_ptr<Kernel> KernelTask::getKernel()
    {
        return kernel_hash_crack;
    }
}
