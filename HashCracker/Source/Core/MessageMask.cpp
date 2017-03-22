/**
    MessageMask.cpp

    Project Name: HashCracker

    Copyright (c) 2017 University of the West of Scotland
    
    Authors: Ioan-Cristian Szabo <cristian.szabo@outlook.com>
*/

#include "Config.hpp"

#include "Core/MessageMask.hpp"

namespace HonoursProject
{
    MessageMask::MessageMask() : ready(false)
    {
    }

    bool MessageMask::create(const std::string & mask)
    {
        if (ready)
        {
            return false;
        }

        for (std::size_t pos = 0; pos < mask.size(); pos++)
        {
            if (mask[pos] == '?')
            {
                data.push_back(createCharset(mask[++pos]));
            }
        }

        this->mask = mask;

        ready = !ready;

        return true;
    }

    std::string MessageMask::toString() const
    {
        return mask;
    }

    bool MessageMask::destroy()
    {
        if (!ready)
        {
            return false;
        }

        data.clear();

        mask.clear();

        ready = !ready;

        return true;
    }

    std::string MessageMask::createMesssage(std::uint64_t rank)
    {
        std::string result;

        for (std::size_t index = 0; index < data.size(); index++)
        {
            const Charset& cs = data.at(index);

            std::uint64_t pos = rank % cs.size();

            result.push_back(cs.at(static_cast<uint32_t>(pos)));

            rank = rank / cs.size();
        }

        return result;
    }

    MessageMask::Charset MessageMask::getCharset(std::size_t index)
    {
        return data.at(index);
    }

    std::size_t MessageMask::getLength()
    {
        return data.size();
    }

    std::uint64_t MessageMask::getSize(std::size_t size)
    {
        uint64_t result = 1;

        if (size == 0 || size >= data.size())
        {
            size = data.size();
        }

        std::for_each(data.begin(), data.begin() + size,
            [&](const Charset& cs)
        {
            result *= cs.size();
        });

        return result;
    }

    std::ostream& operator<<(std::ostream & os, const MessageMask& message)
    {
        os << message.toString();

        return os;
    }

    MessageMask::Charset MessageMask::createCharset(char tag)
    {
        char chr;
        MessageMask::Charset result;

        switch (tag)
        {
        case 'l':
            for (chr = 'a'; chr <= 'z'; chr++)
            {
                result.push_back(chr);
            }
            break;

        case 'u':
            for (chr = 'A'; chr <= 'Z'; chr++)
            {
                result.push_back(chr);
            }
            break;

        case 'o':
            {
                for (chr = 'A'; chr <= 'Z'; chr++)
                {
                    result.push_back(chr);
                }

                for (chr = 'a'; chr <= 'z'; chr++)
                {
                    result.push_back(chr);
                }
            }
            break;

        case 'd':
            for (chr = '0'; chr <= '9'; chr++)
            {
                result.push_back(chr);
            }
            break;

        case 'e':
            {
                for (chr = 'a'; chr <= 'z'; chr++)
                {
                    result.push_back(chr);
                }

                for (chr = 'A'; chr <= 'Z'; chr++)
                {
                    result.push_back(chr);
                }

                for (chr = '0'; chr <= '9'; chr++)
                {
                    result.push_back(chr);
                }
            }
            break;

        case 'f':
            {
                for (chr = '0'; chr <= '9'; chr++)
                {
                    result.push_back(chr);
                }

                for (chr = 'a'; chr <= 'z'; chr++)
                {
                    result.push_back(chr);
                }
            }
            break;

        case 's':
            for (chr = 0x20; chr <= 0x7e; chr++)
            {
                if ((chr >= '0' && chr <= '9') ||
                    chr >= 'A' && chr <= 'Z' ||
                    chr >= 'a' && chr <= 'z')
                {
                    continue;
                }

                result.push_back(chr);
            }
            break;

        case 't':
            for (chr = 0x20; chr <= 0x7e; chr++)
            {
                if (chr >= '0' && chr <= '9')
                {
                    continue;
                }

                result.push_back(chr);
            }
            break;

        case 'a':
            for (chr = 0x20; chr <= 0x7e; chr++)
            {
                result.push_back(chr);
            }
            break;

        default:
            throw std::runtime_error("Invalid charset id!");
        }

        return result;
    }
}
