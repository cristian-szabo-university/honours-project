#include "Config.hpp"

#include "Core/AttackFactory.hpp"

#include "Tasks/Bruteforce/BruteforceSetupTask.hpp"
#include "Tasks/Bruteforce/BruteforceKernelTask.hpp"
#include "Tasks/Bruteforce/BruteforceAttackTask.hpp"

namespace HonoursProject
{
    AttackFactory::AttackFactory(std::shared_ptr<HashFactory> hash_factory)
        : hash_factory(hash_factory)
    {
    }

    AttackFactory::~AttackFactory()
    {
    }

    std::shared_ptr<HashFactory> AttackFactory::getHashFactory()
    {
        return hash_factory;
    }

    Bruteforce::Bruteforce(std::shared_ptr<HashFactory> hash_factory)
        : AttackFactory(hash_factory)
    {
    }

    Bruteforce::~Bruteforce()
    {
    }

    std::shared_ptr<SetupTask> Bruteforce::createSetupTask(std::vector<std::string> input)
    {
        return std::make_shared<BruteforceSetupTask>(input, hash_factory);
    }

    std::shared_ptr<KernelTask> Bruteforce::createKernelTask(std::shared_ptr<Device> device)
    {
        return std::make_shared<BruteforceKernelTask>(device, hash_factory);
    }

    std::shared_ptr<AttackTask> Bruteforce::createAttakTask(std::shared_ptr<HashCracker> hash_cracker)
    {
        return std::make_shared<BruteforceAttackTask>(hash_cracker);
    }
}
