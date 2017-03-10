#pragma once

namespace HonoursProject
{
    class HashFactory;
    class SetupTask;
    class KernelTask;
    class AttackTask;
    class Device;
    class HashCracker;

    class HASH_CRACKER_PUBLIC AttackFactory
    {
    public:

        AttackFactory(std::shared_ptr<HashFactory> hash_factory);

        virtual ~AttackFactory();

        std::shared_ptr<HashFactory> getHashFactory();

        virtual std::shared_ptr<SetupTask> createSetupTask(std::vector<std::string> input) = 0;

        virtual std::shared_ptr<KernelTask> createKernelTask(std::shared_ptr<Device> device) = 0;

        virtual std::shared_ptr<AttackTask> createAttakTask(std::shared_ptr<HashCracker> hash_cracker) = 0;

    protected:

        std::shared_ptr<HashFactory> hash_factory;

    };

    class HASH_CRACKER_PUBLIC Bruteforce : public AttackFactory
    {
    public:

        Bruteforce(std::shared_ptr<HashFactory> hash_factory);

        virtual ~Bruteforce();

        virtual std::shared_ptr<SetupTask> createSetupTask(std::vector<std::string> input) override;

        virtual std::shared_ptr<KernelTask> createKernelTask(std::shared_ptr<Device> device) override;

        virtual std::shared_ptr<AttackTask> createAttakTask(std::shared_ptr<HashCracker> hash_cracker) override;

    };
}
