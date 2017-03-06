#pragma once

namespace HonoursProject
{
    class HASH_CRACKER_PUBLIC Charset
    {
    public:

        static const Charset lowerCaseLetters;

        static const Charset upperCaseLetters;

        static const Charset specialLetters;

        static const Charset digitalLetters;

        static const Charset allLetters;

        Charset(char id);

        Charset(std::initializer_list<char> cs);

        char getChar(std::size_t index) const;

        std::size_t getIndex(char c) const;

        const char* getData() const;

        std::size_t getSize() const;

        static std::vector<Charset> Create(const std::string& mask);

        static uint64_t GetTotalMsgNum(std::vector<Charset> charsets, std::size_t size = 0);

        static std::string GetMsgFromIndex(std::uint64_t index, std::vector<Charset> charsets);

    private:

        std::vector<char> data;

    };
}
