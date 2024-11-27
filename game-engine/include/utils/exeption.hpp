#pragma once
#include "ansi-colors.hpp"
#include <cstdio>

// Need to support errno someday, change to perror

char *formatString(const char *format, ...);

namespace utils {
class EngineExeption {
  const char *msg;

public:
  EngineExeption(const char *msg) : msg(msg){};
  const char *what() const noexcept { return msg; }
  void tell() const noexcept { fprintf(stderr, "%s", what()); }
};
} // namespace utils

#define THROW_EXEPTION(msg, ...)                                               \
  throw utils::EngineExeption(formatString(RED_COLOR "ERROR at %s:%d: " msg    \
                                                     "\n\n",                   \
                                           __FILE__, __LINE__, ##__VA_ARGS__))
