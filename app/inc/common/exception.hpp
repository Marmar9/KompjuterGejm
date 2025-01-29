#pragma once
#include "ansi-colors.hpp"
#include "format-string.hpp"
// Need to support errno someday, change to perror

namespace utils {

class Error {
  const char *msg;

public:
  Error(const char *msg);
  const char *what() const noexcept;
  void tell() const noexcept;
};

} // namespace utils

#define THROW_EXCEPTION(msg, ...)                                              \
  throw utils::Error(utils::formatString("ERROR at %s:%d: " msg, __FILE__,     \
                                         __LINE__, ##__VA_ARGS__))
