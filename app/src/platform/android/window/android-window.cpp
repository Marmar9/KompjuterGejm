#include "inc/platform/android/window/android-window.hpp"
#include "inc/common/exception.hpp"
#include "unistd.h"
#include "vulkan/vulkan_android.h"

namespace window {

AndroidWindow::AndroidWindow(const ANativeWindow *window) : _window(window){};

AndroidWindow::~AndroidWindow(){};
void AndroidWindow::pollEvents() const { usleep(100); };

void AndroidWindow::createWindowSurface(VkInstance instance,
                                        VkSurfaceKHR *surface) const {

  VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo{};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.window = const_cast<ANativeWindow *>(_window);

  if (vkCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr,
                                surface) != VK_SUCCESS) {
    THROW_EXCEPTION("Failed to create android window");
  }
};

bool AndroidWindow::shouldClose() const noexcept { return false; };
WindowDims AndroidWindow::getDims() const noexcept { return {200, 200}; };

} // namespace window
