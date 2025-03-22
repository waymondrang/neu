#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

namespace Logger
{
    // format string info
    const std::string INFO = "\033[1;32m[INFO]\033[0m ";
    const std::string WARN = "\033[1;33m[WARN]\033[0m ";
    const std::string ERROR = "\033[1;31m[ERROR]\033[0m ";
    const std::string DEBUG = "\033[1;34m[DEBUG]\033[0m ";

    void info(const char *format, ...);
    void warn(const char *format, ...);
    void error(const char *format, ...);
    void debug(const char *format, ...);
}

#endif