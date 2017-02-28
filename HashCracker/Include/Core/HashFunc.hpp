#pragma once

#include "Core/Platform.hpp"

namespace HonoursProject
{
    class HASHCRACK_PUBLIC HashFunc
    {
    public:

        enum class Type : std::int32_t
        {
            Invalid,
            MD5,
            SHA1,
            MaxCount
        };

        virtual ~HashFunc();

        virtual Type type() = 0;

        virtual KernelPlatform::message_digest_t parse(std::string hash) = 0;

        virtual std::uint32_t mask() = 0;

        virtual std::uint32_t bit14() = 0;

        virtual std::uint32_t bit15() = 0;

    };

    class HASHCRACK_PUBLIC MD5_HashFunc : public HashFunc
    {
    public:

        virtual ~MD5_HashFunc();

        virtual Type type() override;

        virtual KernelPlatform::message_digest_t parse(std::string hash) override;

        virtual std::uint32_t mask() override;

        virtual std::uint32_t bit14() override;

        virtual std::uint32_t bit15() override;

    };
}
