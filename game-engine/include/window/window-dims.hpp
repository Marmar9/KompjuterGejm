#pragma once
#include "stdint.h"
#include <vulkan/vulkan_core.h>

namespace window {

struct WindowDims : VkExtent2D {
  bool operator==(const WindowDims &b) {
    return (width == b.width && height == b.height);
  };
  bool operator!=(const WindowDims &b) {
    return !(width == b.width && height == b.height);
  };
};
} // namespace window
