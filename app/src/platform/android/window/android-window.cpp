#include "inc/platform/android/window/android-window.hpp"
#include "inc/common/exception.hpp"
#include "unistd.h"
#include "vulkan/vulkan_android.h"
#include <android/native_window.h>
namespace window {

AndroidWindow::AndroidWindow(const android_app *app) : _app(app){};

AndroidWindow::~AndroidWindow(){};
void AndroidWindow::pollEvents() const { usleep(100); };

void AndroidWindow::createWindowSurface(VkInstance instance,
                                        VkSurfaceKHR *surface) const {
  while (!_app->window) {
    int outEvents;
    int outFd;
    int res = ALooper_pollOnce(-1, &outFd, &outEvents, nullptr);
    _app->cmdPollSource.process(
        const_cast<android_app *>(_app),
        const_cast<android_poll_source *>(&_app->cmdPollSource));
  }

  VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo{};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.window = const_cast<ANativeWindow *>(_app->window);

  if (vkCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr,
                                surface) != VK_SUCCESS) {
    THROW_EXCEPTION("Failed to create android window");
  }
};

bool AndroidWindow::shouldClose() const noexcept { return false; };
WindowDims AndroidWindow::getDims() const noexcept { return {200, 200}; };

} // namespace window
