#include "android/log.h"
#include "inc/common/loger.h"

#define LOG_TAG "NativeKompjuterGejm"

void logM(const char *msg) {
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", msg);
};
