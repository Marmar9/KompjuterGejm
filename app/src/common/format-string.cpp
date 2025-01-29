#include "inc/common/format-string.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
char *utils::formatString(const char *format, ...) {
  // Set the initial buffer size (this is just a starting point)
  size_t bufferSize = 256;

  // Allocate memory for the buffer
  char *buffer = (char *)malloc(bufferSize);

  if (buffer == nullptr) {
    return nullptr; // Return nullptr if memory allocation fails
  }

  va_list args;
  va_start(args, format);

  // Try to use sprintf to write to the buffer
  int n = vsnprintf(buffer, bufferSize, format, args);
  va_end(args);

  // If the output doesn't fit, reallocate and try again
  if (n >= bufferSize) {
    bufferSize = n + 1; // Allocate enough space for the new output
    buffer = (char *)realloc(buffer, bufferSize);

    if (buffer == nullptr) {
      return nullptr; // Return nullptr if memory allocation fails
    }

    va_start(args, format);
    vsnprintf(buffer, bufferSize, format, args);
    va_end(args);
  }

  return buffer;
}
