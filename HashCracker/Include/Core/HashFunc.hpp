#pragma once

#include "ProgramEntry.hpp"

#include "Core/Platform.hpp"
#include "Core/Charset.hpp"

#include "Tasks/BruteforceSetupTask.hpp"
#include "Tasks/BruteforceKernelTask.hpp"
#include "Tasks/BruteforceAttackTask.hpp"

namespace HonoursProject
{
    class SetupTask;
    class KernelTask;
    class AttackTask;

    class HashFunc
    {
    public:

        virtual const std::string& type() = 0;

        virtual std::shared_ptr<SetupTask> setup_task() = 0;

        virtual std::shared_ptr<KernelTask> kernel_task() = 0;

        virtual std::shared_ptr<AttackTask> attack_task() = 0;

        virtual std::vector<std::uint32_t> parse(std::string hash) = 0;

        virtual const ProgramEntry& source() = 0;

        virtual std::uint32_t mask() = 0;

        virtual std::uint32_t bit14() = 0;

        virtual std::uint32_t bit15() = 0;

        virtual std::array<std::uint32_t, 4> digest() = 0;

    };

    template
    <
        class Task, 
        class Parse, 
        std::uint32_t m, 
        std::uint32_t b1, 
        std::uint32_t b2
    >
    class HASH_CRACKER_PUBLIC THashFunc : public HashFunc
    {
    public:

        inline virtual std::shared_ptr<SetupTask> setup_task() override
        {
            return Task::setup();
        }

        inline virtual std::shared_ptr<KernelTask> kernel_task() override
        {
            return Task::kernel();
        }

        inline virtual std::shared_ptr<AttackTask> attack_task() override
        {
            return Task::attack();
        }

        inline virtual std::vector<std::uint32_t> parse(std::string hash) override
        {
            return Parse::parse(hash);
        }

        inline virtual std::uint32_t mask() override
        {
            return m;
        }

        inline virtual std::uint32_t bit14() override
        {
            return b1;
        }

        inline virtual std::uint32_t bit15() override
        {
            return b2;
        }

    };

    template<class Setup, class Kernel, class Attack>
    class HashProcess
    {
    public:

        static inline std::shared_ptr<SetupTask> setup()
        {
            return std::make_shared<Setup>();
        }

        static inline std::shared_ptr<KernelTask> kernel()
        {
            return std::make_shared<Kernel>();
        }

        static inline std::shared_ptr<AttackTask> attack()
        {
            return std::make_shared<Attack>();
        }

    };

    class Bruteforce_HashProcess : public HashProcess<BruteforceSetupTask, BruteforceKernelTask, BruteforceAttackTask> {};

    // class Dictionary_HashProcess : public HashProcess<DictionarySetupTask, DictionaryKernelTask, DictionaryAttackTask> {};

    class HASH_CRACKER_PUBLIC MD5_Parse
    {
    public:

        static std::vector<std::uint32_t> parse(std::string hash);

    };

    /*
    class HASH_CRACKER_PUBLIC SHA1_Parse
    {
    public:

        static std::vector<std::uint32_t> parse(std::string hash);

    };
    */
    
    template<class Task>
    class HASH_CRACKER_PUBLIC MD5_HashFunc : public THashFunc<Task, MD5_Parse, 0x80808080, 1, 0>
    {
    public:

        typedef struct
        {
            cl_uint data[4];
        } message_digest_t;

        MD5_HashFunc();

        inline virtual const std::string& type() override
        {
            return name;
        }

        inline virtual const ProgramEntry& source() override
        {
            return program;
        }

        virtual std::array<std::uint32_t, 4> digest() override
        {
            return { 0, 3, 2, 1 };
        }

    private:

        static const std::string name;

        static const ProgramEntry& program;

    };

    /*
    template<class Task>
    class HASH_CRACKER_PUBLIC SHA1_HashFunc : public THashFunc<Task, SHA1_Parse, 0x80808080, 0, 1>
    {
    public:

        inline virtual const std::string& type() override
        {
            return name;
        }

        inline virtual const ProgramEntry& source() override
        {
            return program;
        }

        virtual std::array<std::uint32_t, 4> digest() override
        {
            return { 3, 4, 2, 1 };
        }

    private:

        static const std::string name;

        static const ProgramEntry& program;

    };
    */
}
