#ifndef LOGER_H
#define LOGER_H

#include "ansi-colors.hpp"
#include "format-string.hpp"

#include <stdio.h>
// Logging

void logM(const char *);

#define LOG(msg, ...)                                                          \
  logM(utils::formatString("Log at %s:%d: " msg, __FILE__, __LINE__,           \
                           ##__VA_ARGS__));

#endif
