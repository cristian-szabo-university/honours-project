/**
    Task.cpp

    Project Name: HashCracker

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#include "Config.hpp"

#include "Tasks/Task.hpp"

namespace HonoursProject
{
    NoReturnTask::~NoReturnTask()
    {
    }

    void NoReturnTask::run()
    {
    }

    BaseTask::~BaseTask()
    {
    }

    void BaseTask::transfer(std::shared_ptr<BaseTask> task)
    {
    }

    Task::~Task()
    {
    }

    std::int32_t Task::run()
    {
        return 0;
    }
}
