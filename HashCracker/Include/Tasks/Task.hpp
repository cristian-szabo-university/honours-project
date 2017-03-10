#pragma once

namespace HonoursProject
{
    class HASH_CRACKER_PUBLIC BaseTask
    {
    public:

        virtual ~BaseTask();

        virtual void transfer(std::shared_ptr<BaseTask> task);

    };

    template<class T>
    class TTask : public BaseTask
    {
    public:

        virtual ~TTask()
        {
        }

        virtual T run() = 0;

    };

    class HASH_CRACKER_PUBLIC NoReturnTask : public TTask<void>
    {
    public:

        virtual ~NoReturnTask();

        virtual void run() override;

    };

    class HASH_CRACKER_PUBLIC Task : public TTask<std::int32_t>
    {
    public:

        virtual ~Task();

        virtual std::int32_t run() override;

    };
}
