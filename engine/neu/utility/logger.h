#pragma once

#include <iostream>

namespace Neu {
class Logger {
 public:
  static void info(const char* format, ...);
  static void warn(const char* format, ...);
  static void error(const char* format, ...);
  static void debug(const char* format, ...);
};
};  // namespace Neu