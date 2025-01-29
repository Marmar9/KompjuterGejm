#pragma once

#include "android/native_window.h"
#include "inc/common/window/window-base.hpp"
namespace window {
class AndroidWindow : public Window {
  const ANativeWindow *_window;

public:
  AndroidWindow(const ANativeWindow *window);
  ~AndroidWindow();
  void pollEvents() const override;
  void createWindowSurface(VkInstance instance,
                           VkSurfaceKHR *surface) const override;
  bool shouldClose() const noexcept override;
  WindowDims getDims() const noexcept override;
};
} // namespace window
