#include "inc/platform/android/window/android-window.hpp"
#include "inc/common/exception.hpp"
#include "inc/common/loger.h"
#include "vulkan/vulkan_android.h"
#include <android/choreographer.h>
#include <android/native_window.h>
#include <atomic>
namespace window {

#ifdef DEBUG
unsigned long lastTimestamp;
#endif // DEBUG

std::atomic<bool> frameReady;

static_assert(std::atomic<int>::is_always_lock_free,
              "Atomic<int> is not lock-free on this platform!");

void FrameCallback(long frameTimeNanos, void *data) {
  std::atomic<bool> &frameReady = *static_cast<std::atomic<bool> *>(data);

#ifdef DEBUG
  float fps = 1e9f / (float)(frameTimeNanos - lastTimestamp);
  lastTimestamp = frameTimeNanos;
  LOG("Fps-------------------: %.2f --------------------", fps);
#endif // DEBUG

  frameReady.store(true);
}

AndroidWindow::AndroidWindow(const android_app *app) : _app(app) {
  frameReady.store(false);
  AChoreographer_postFrameCallback64(AChoreographer_getInstance(),
                                     FrameCallback, &frameReady);
};

AndroidWindow::~AndroidWindow(){};
void AndroidWindow::pollEvents() {
  while (!frameReady.load()) {
    int outEvents;
    int outFd;
    ALooper_pollOnce(-1, &outFd, &outEvents, nullptr);
  }

  frameReady.store(false);
  AChoreographer_postFrameCallback64(AChoreographer_getInstance(),
                                     FrameCallback, &frameReady);
};

void AndroidWindow::createWindowSurface(VkInstance instance,
                                        VkSurfaceKHR *surface) const {
  while (!_app->window) {
    int outEvents;
    int outFd;
    ALooper_pollOnce(-1, &outFd, &outEvents, nullptr);
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
