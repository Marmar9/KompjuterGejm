#pragma once

#include "inc/common/window/window-base.hpp"
#include "inc/platform/android/android_native_app_glue.h"

namespace window {
class AndroidWindow : public Window {
  const android_app *_app;

public:
  AndroidWindow(const android_app *app);
  ~AndroidWindow();
  void pollEvents() const override;
  void createWindowSurface(VkInstance instance,
                           VkSurfaceKHR *surface) const override;
  bool shouldClose() const noexcept override;
  WindowDims getDims() const noexcept override;
};
} // namespace window
