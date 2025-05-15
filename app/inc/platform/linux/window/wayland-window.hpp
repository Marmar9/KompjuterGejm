#pragma once

#include "inc/common/window/window-base.hpp"

#include "inc/common/window/window-dims.hpp"
#include "protocol.h"
#include "xdg-shell-protocol.h"
#include <atomic>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>
#include <wayland-client-core.h>

namespace window {

struct WaylandContext {
  uint32_t width;
  uint32_t height;
  uint32_t stride;

  struct wl_display *disp;
  struct wl_registry *reg;
  struct wl_compositor *comp;

  struct wl_surface *srfc;
  struct wl_callback *cback;
  struct xdg_wm_base *xdg;
  struct xdg_surface *xdg_srfc;
  struct xdg_toplevel *xdg_toplevel;
};

WaylandContext *create_window();

class WaylandWindow : public window::Window {
public:
  WaylandContext *_ctx;
  WindowDims dims;
  WindowDims desiredDims;
  bool closed = false;

  std::atomic<bool> frameReady;
#ifdef DEBUG
  unsigned long lastTimestamp;
#endif // DEBUG

  static_assert(std::atomic<int>::is_always_lock_free,
                "Atomic<int> is not lock-free on this platform!");

  WaylandWindow();
  ~WaylandWindow();

  void pollEvents() override;
  void createWindowSurface(VkInstance instance,
                           VkSurfaceKHR *surface) const override;
  bool shouldClose() const noexcept override;
  WindowDims getDims() const noexcept override;
};
} // namespace window
