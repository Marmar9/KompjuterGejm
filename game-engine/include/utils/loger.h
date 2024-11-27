#ifndef LOGER_H
#define LOGER_H

#include "ansi-colors.hpp"

#include <stdio.h>
// Logging

#define LOG(msg, ...)                                                          \
  printf(RESET_COLOR "Logger at %s:%d: " msg "\n\n", __FILE__, __LINE__,       \
         ##__VA_ARGS__);

#endif
