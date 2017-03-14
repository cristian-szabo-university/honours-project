#pragma once

namespace HonoursProject
{
    class HASH_CRACKER_PUBLIC MessageMask
    {
    public:

        using Charset = std::vector<char>;

        MessageMask();

        bool create(const std::string& mask);

        std::string toString() const;

        bool destroy();

        std::string createMesssage(std::uint64_t rank);

        Charset getCharset(std::size_t index);

        std::size_t getLength();

        std::uint64_t getSize(std::size_t size = 0);

    private:

        bool ready;

        std::string mask;

        std::vector<Charset> data;

        Charset createCharset(char tag);

    };

    HASH_CRACKER_PUBLIC std::ostream & operator<<(std::ostream & os, const MessageMask& message);
}
