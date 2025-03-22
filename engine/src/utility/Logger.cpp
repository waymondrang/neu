#include "../core.h"

#include <cstdarg>

namespace Logger
{
    void info(const char *format, ...)
    {
        const std::string formatString = INFO + format;

        va_list args;
        va_start(args, format);
        vprintf(formatString.c_str(), args);
        va_end(args);
    }

    void warn(const char *format, ...)
    {
        const std::string formatString = WARN + format;

        va_list args;
        va_start(args, format);
        vprintf(formatString.c_str(), args);
        va_end(args);
    }

    void error(const char *format, ...)
    {
        const std::string formatString = ERROR + format;

        va_list args;
        va_start(args, format);
        vprintf(formatString.c_str(), args);
        va_end(args);
    }

    void debug(const char *format, ...)
    {
        const std::string formatString = DEBUG + format;

        va_list args;
        va_start(args, format);
        vprintf(formatString.c_str(), args);
        va_end(args);
    }
}