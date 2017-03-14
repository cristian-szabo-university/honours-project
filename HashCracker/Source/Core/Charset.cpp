#include "Config.hpp"

#include "Core/Charset.hpp"

namespace HonoursProject
{
    Charset::Charset(char id)
    {
        char chr;

        switch (id)
        {
        case 'l':
            for (chr = 'a'; chr <= 'z'; chr++)
            {
                data.push_back(chr);
            }
            break;

        case 'u':
            for (chr = 'A'; chr <= 'Z'; chr++)
            {
                data.push_back(chr);
            }
            break;

        case 'o' :
            for (chr = 'A'; chr <= 'z'; chr++)
            {
                data.push_back(chr);
            }
            break;

        case 'd':
            for (chr = '0'; chr <= '9'; chr++)
            {
                data.push_back(chr);
            }
            break;

        case 'e':
            for (chr = '0'; chr <= 'z'; chr++)
            {
                data.push_back(chr);
            }
            break;

        case 's':
            for (chr = 0x20; chr <= 0x7e; chr++)
            {
                if (chr >= '0' && chr < 'z')
                {
                    continue;
                }

                data.push_back(chr);
            }
            break;

        case 'a':
            for (chr = 0x20; chr <= 0x7e; chr++)
            {
                data.push_back(chr);
            }
            break;

        default:
            throw std::runtime_error("Invalid charset id!");
        }
    }

    Charset::Charset(std::initializer_list<char> cs)
    {
        data.resize(cs.size());

        std::copy(cs.begin(), cs.end(), data.begin());
    }

    char Charset::getChar(std::size_t index) const
    {
        return data.at(index);
    }

    std::size_t Charset::getIndex(char c) const
    {
        auto result = std::find(data.begin(), data.end(), c);

        if (result == data.end())
        {
            return -1;
        }

        return result - data.begin();
    }

    const char* Charset::getData() const
    {
        return data.data();
    }

    std::size_t Charset::getSize() const
    {
        return data.size();
    }

    std::vector<Charset> Charset::Create(const std::string & mask)
    {
        std::vector<Charset> result;

        for (std::size_t pos = 0; pos < mask.size(); pos++)
        {
            if (mask[pos] == '?')
            {
                result.push_back(mask[++pos]);
            }
        }

        return result;
    }

    uint64_t Charset::GetTotalMsgNum(std::vector<Charset> charsets, std::size_t size)
    {
        uint64_t result = 1;

        if (size == 0 || size > charsets.size())
        {
            size = charsets.size();
        }

        std::for_each(charsets.begin(), charsets.begin() + size,
            [&](const Charset& cs)
        {
            result *= cs.getSize();
        });

        return result;
    }

    std::string Charset::GetMsgFromIndex(std::uint64_t rank, std::vector<Charset> charsets)
    {
        std::string result;

        for (std::size_t index = 0; index < charsets.size(); index++)
        {
            const Charset& cs = charsets.at(index);

            std::uint64_t pos = rank % cs.getSize();

            result.push_back(cs.getChar(static_cast<uint32_t>(pos)));

            rank = rank / cs.getSize();
        }

        return result;
    }
}
