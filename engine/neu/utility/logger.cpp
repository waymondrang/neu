#include <cstdarg>

#include "core.h"

using namespace Neu;

void Logger::info(const char* format, ...) {
  const std::string formatString =
      "\033[1;32m[INFO]\033[0m " + std::string(format);

  va_list args;
  va_start(args, format);
  vprintf(formatString.c_str(), args);
  va_end(args);
}

void Logger::warn(const char* format, ...) {
  const std::string formatString =
      "\033[1;33m[WARN]\033[0m " + std::string(format);

  va_list args;
  va_start(args, format);
  vprintf(formatString.c_str(), args);
  va_end(args);
}

void Logger::error(const char* format, ...) {
  const std::string formatString =
      "\033[1;31m[ERROR]\033[0m " + std::string(format);

  va_list args;
  va_start(args, format);
  vprintf(formatString.c_str(), args);
  va_end(args);
}

void Logger::debug(const char* format, ...) {
  const std::string formatString =
      "\033[1;34m[DEBUG]\033[0m " + std::string(format);

  va_list args;
  va_start(args, format);
  vprintf(formatString.c_str(), args);
  va_end(args);
}