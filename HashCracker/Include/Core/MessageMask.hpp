#pragma once

namespace HonoursProject
{
    class HASH_CRACKER_PUBLIC MessageMask
    {
    public:

        using Charset = std::vector<char>;

        MessageMask();

        bool create(const std::string& mask);

        bool destroy();

        std::string createMesssage(std::uint64_t rank);

        Charset getCharset(std::size_t index);

        std::size_t getLength();

        std::uint64_t getSize(std::size_t size = 0);

    private:

        bool ready;

        std::vector<Charset> data;

        Charset createCharset(char tag);

    };
}
