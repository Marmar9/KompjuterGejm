#pragma once
#include "inc/common/window/window-dims.hpp"
#include <future>
#include <vulkan/vulkan.h>

namespace window {

class Window {

public:
  Window() = default;
  virtual ~Window() = default;
  virtual void pollEvents() = 0;
  virtual void createWindowSurface(VkInstance instance,
                                   VkSurfaceKHR *surface) const = 0;
  virtual bool shouldClose() const noexcept = 0;
  virtual WindowDims getDims() const noexcept = 0;
};
} // namespace window
