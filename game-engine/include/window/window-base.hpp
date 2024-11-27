#pragma once
#include "include/vulkan/handle.hpp"
#include "window-dims.hpp"
#include <vulkan/vulkan.h>

namespace window {
class Window {

public:
  Window() = default;
  virtual ~Window();
  virtual void pollEvents() = 0;
  virtual void createWindowSurface(VkInstance instance,
                                   VkSurfaceKHR *surface) const = 0;
  virtual bool shouldClose() noexcept = 0;
  virtual WindowDims getDims() const noexcept = 0;
};
} // namespace window
