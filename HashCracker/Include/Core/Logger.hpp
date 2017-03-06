#pragma once

namespace HonoursProject
{
    class HASH_CRACKER_PUBLIC Logger
    {
    public:

        static std::size_t error(const char *fmt, ...);

        static std::size_t info(const char *fmt, ...);

        static std::size_t read(const char *fmt, ...);

    private:

        static std::mutex m_console;

        static std::size_t write(std::ostream& stream, const char* fmt, va_list ap);

        static std::size_t read(std::istream& stream, const char* fmt, va_list ap);

    };
}
