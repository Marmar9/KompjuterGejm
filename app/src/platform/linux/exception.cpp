#include "inc/common/exception.hpp"
#include <cstdio>

utils::Error::Error(const char *msg) : msg(msg) {}

const char *utils::Error::what() const noexcept { return msg; }

void utils::Error::tell() const noexcept { fprintf(stderr, "%s\n\n", what()); }
