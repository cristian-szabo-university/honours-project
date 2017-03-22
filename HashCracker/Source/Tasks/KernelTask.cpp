/**
    KernelTask.cpp

    Project Name: HashCracker

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#include "Config.hpp"

#include "Tasks/KernelTask.hpp"
#include "Tasks/SetupTask.hpp"

#include "Core/Logger.hpp"

namespace HonoursProject
{
    KernelTask::~KernelTask()
    {
    }

    void KernelTask::transfer(std::shared_ptr<BaseTask> task)
    {
        std::shared_ptr<SetupTask> cast_task = std::dynamic_pointer_cast<SetupTask>(task);

        if (cast_task)
        {
            msg_dgst = cast_task->getMessageDigest();
        }
    }

    std::shared_ptr<Kernel> KernelTask::getKernel()
    {
        return kernel_hash_crack;
    }

    std::vector<std::uint32_t> KernelTask::getMessageDigest()
    {
        return msg_dgst;
    }
}
