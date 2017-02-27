#include "Config.hpp"

#include "Core/Logger.hpp"

namespace HonoursProject
{
    std::mutex Logger::m_console;

    std::size_t Logger::error(const char * fmt, ...)

    {
        std::unique_lock<std::mutex> lk(m_console);

        va_list ap;

        va_start(ap, fmt);

        const std::size_t len = write(std::cerr, fmt, ap);

        va_end(ap);

        return len;
    }

    std::size_t Logger::info(const char * fmt, ...)
    {
        std::unique_lock<std::mutex> lk(m_console);

        va_list ap;

        va_start(ap, fmt);

        const std::size_t len = write(std::cout, fmt, ap);

        va_end(ap);

        return len;
    }

    std::size_t Logger::read(const char * fmt, ...)
    {
        std::unique_lock<std::mutex> lk(m_console);

        va_list ap;

        va_start(ap, fmt);

        const std::size_t len = read(std::cin, fmt, ap);

        va_end(ap);

        return len;
    }

    std::size_t Logger::write(std::ostream & stream, const char* fmt, va_list ap)
    {
        char s[4096] = { 0 };

        std::size_t max_len = sizeof(s);

        std::size_t len = vsnprintf(s, max_len, fmt, ap);

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
}
