#pragma once

#include "include/window/window-base.hpp"

#include "include/window/window-base.hpp"
#include "include/window/window-dims.hpp"
#include "protocol.h"
#include "xdg-shell-protocol.h"
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
  struct xdg_wm_base *xdg;
  struct xdg_surface *xdg_srfc;
  struct xdg_toplevel *xdg_toplevel;
};

WaylandContext *create_window();

class WaylandWindow : public window::Window {
private:
  WaylandContext *_ctx;

public:
  WindowDims dims;
  WindowDims desiredDims;
  bool closed = false;

  WaylandWindow();
  ~WaylandWindow();

  void pollEvents() override;
  void createWindowSurface(VkInstance instance,
                           VkSurfaceKHR *surface) const override;
  bool shouldClose() noexcept override;
  WindowDims getDims() const noexcept override;
};
} // namespace window
