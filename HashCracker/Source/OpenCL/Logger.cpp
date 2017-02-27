#include "PCH.h"

#include "Logger.h"

std::mutex Logger::m_console;

std::size_t Logger::write(std::ostream & stream, const char* fmt, va_list ap)
{
    char s[4096] = { 0 };

    std::size_t max_len = sizeof(s);

    std::size_t len = vsprintf(s, fmt, ap);

    if (len > max_len)
    {
        len = max_len;
    }

    stream << s;

    stream.flush();

    return len;
}

std::size_t Logger::read(std::istream & stream, const char* fmt, va_list ap)
{
    char s[4096] = { 0 };

    std::size_t max_len = sizeof(s);

    stream >> s;

    std::size_t len = vsscanf(s, fmt, ap);

    if (len > max_len)
    {
        len = max_len;
    }

    return len;
}