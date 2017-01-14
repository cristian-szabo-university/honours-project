#pragma once

#include "Core/Platform.hpp"

#include "Task.hpp"

namespace HonoursProject
{
    class HashCracker;
    class Kernel;
    class Device;

    class HASHCRACK_PUBLIC AttackTask : public TTask<std::string>
    {
    public:

        AttackTask(std::shared_ptr<HashCracker> hash_cracker, std::shared_ptr<Kernel> kernel_hash_crack);

        virtual ~AttackTask();

        virtual std::string run() override;

        virtual void setBatchSize(std::uint32_t batch_size);

        std::uint32_t getBatchSize();

        virtual void setBatchOffset(std::uint64_t batch_offset);

        std::uint64_t getBatchOffset();

        virtual void setInnerLoopSize(std::uint32_t inner_loop_size);

        std::uint32_t getInnerLoopSize();

        virtual void setInnerLoopStep(std::uint32_t inner_loop_step);

        std::uint32_t getInnerLoopStep();

        double getAvgExecTime();

        double getAvgSpeedTime();

        std::shared_ptr<Device> getDevice();

    protected:

        std::shared_ptr<HashCracker> hash_cracker;

        std::shared_ptr<Kernel> kernel_hash_crack;

        KernelPlatform::message_index_t msg_idx;

        std::uint32_t batch_size;

        std::uint64_t batch_offset;

        std::uint32_t inner_loop_size;

        std::uint32_t inner_loop_step;

        std::uint32_t inner_loop_pos;

        std::list<double> exec_ms;

        std::list< std::pair<std::uint32_t, std::uint64_t> > speed_ms;

        virtual void preKernelExecute(std::uint32_t inner_loop_left, std::uint32_t inner_loop_pos);

    };
}
