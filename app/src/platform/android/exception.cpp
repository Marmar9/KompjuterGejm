#include "inc/common/exception.hpp"
#include "android/log.h"

#define LOG_TAG "NativeKompjuterGejm"

utils::Error::Error(const char *msg) : msg(msg) {}

const char *utils::Error::what() const noexcept { return msg; }

void utils::Error::tell() const noexcept {
  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s", what());
}
