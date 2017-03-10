#pragma once

namespace HonoursProject
{
    class ProgramEntry;

    class HASH_CRACKER_PUBLIC HashFactory
    {
    public:

        virtual ~HashFactory();

        virtual const std::string& type() = 0;

        virtual std::vector<std::uint32_t> parse(std::string hash) = 0;

        virtual const ProgramEntry& source() = 0;

        virtual std::uint32_t mask() = 0;

        virtual std::uint32_t bit14() = 0;

        virtual std::uint32_t bit15() = 0;

        virtual std::array<std::uint32_t, 4> digest() = 0;

    };

    class HASH_CRACKER_PUBLIC MD5 : public HashFactory
    {
    public:

        struct message_digest_t
        {
            std::uint32_t data[4];
        };

        MD5();

        virtual ~MD5();

        virtual const std::string & type() override;

        virtual std::vector<std::uint32_t> parse(std::string hash) override;

        virtual const ProgramEntry & source() override;

        virtual std::uint32_t mask() override;

        virtual std::uint32_t bit14() override;

        virtual std::uint32_t bit15() override;

        virtual std::array<std::uint32_t, 4> digest() override;

    private:

        static const std::string name;

    };
}
