#include "include/window/wayland/wayland-window.hpp"
#include "include/utils/exeption.hpp"
#include "include/utils/loger.h"
#include "include/window/wayland/wayland-deleter.hpp"
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <poll.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_wayland.h>

using namespace window;

static short poll_single(int fd, short events, int timeout) {
  pollfd pfd{.fd = fd, .events = events, .revents = 0};
  if (0 > poll(&pfd, 1, timeout)) {
    THROW_EXEPTION("poll() failed");
  }

  return pfd.revents;
}

static void global(void *data, struct wl_registry *reg, uint32_t name,
                   const char *intf, uint32_t version) {

  WaylandContext *ctx = static_cast<WaylandContext *>(data);

  if (!strcmp(intf, wl_compositor_interface.name)) {

    ctx->comp = (struct wl_compositor *)wl_registry_bind(
        reg, name, &wl_compositor_interface, 4);
  } else if (!strcmp(intf, xdg_wm_base_interface.name)) {

    ctx->xdg = (struct xdg_wm_base *)wl_registry_bind(
        reg, name, &xdg_wm_base_interface, 1);
  }
};

static void global_remove(void *data, struct wl_registry *wl_registry,
                          uint32_t name) {};

struct wl_registry_listener reg_list = {.global = global,
                                        .global_remove = global_remove};
// Xdg surface listener
static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
                                  uint32_t serial) {
  WaylandWindow *self = static_cast<WaylandWindow *>(data);
  if (self->desiredDims != self->dims) {
    self->dims = self->desiredDims;
  }
  xdg_surface_ack_configure(xdg_surface, serial);
}

struct xdg_surface_listener srfc_list = {.configure = xdg_surface_configure};

// Xdg toplevel listener
static void xdg_toplevel_configure(void *data, struct xdg_toplevel *, int nw,

                                   int nh, struct wl_array *states) {
  WaylandWindow *self = static_cast<WaylandWindow *>(data);

  if (!nw && !nh) {
    return;
  }

  if (self->desiredDims.width != nw || self->desiredDims.height != nh) {
    self->desiredDims.width = nw;
    self->desiredDims.height = nh;
    // resz(ctx);
  }
}

static void xdg_toplevel_close(void *data, struct xdg_toplevel *tLvl) {
  WaylandWindow *self = static_cast<WaylandWindow *>(data);
  self->closed = true;
}

struct xdg_toplevel_listener toplevel_list = {
    .configure = xdg_toplevel_configure, .close = xdg_toplevel_close};

WaylandWindow::WaylandWindow() : dims{1, 1}, desiredDims{1, 1} {
  _ctx = (WaylandContext *)calloc(1, sizeof(WaylandContext));

  _ctx->height = 1;
  _ctx->width = 1;
  _ctx->stride = _ctx->width * 4;
  _ctx->disp = wl_display_connect(0);
  _ctx->reg = wl_display_get_registry(_ctx->disp);
  // wl_display_flush(disp);

  // printf("%d\n", wl_proxy_get_id((wl_proxy *)reg));

  wl_registry_add_listener(_ctx->reg, &reg_list, _ctx);
  // To remember
  //   wl_display_flush(disp);
  //  printf("%d\n", wl_display_prepare_read(disp));
  //  sleep(1);
  //  wl_display_read_events(disp);
  //  wl_display_dispatch_pending(disp);

  wl_display_roundtrip(_ctx->disp);

  _ctx->srfc = wl_compositor_create_surface(_ctx->comp);
  _ctx->xdg_srfc = xdg_wm_base_get_xdg_surface(_ctx->xdg, _ctx->srfc);
  xdg_surface_add_listener(_ctx->xdg_srfc, &srfc_list, this);
  _ctx->xdg_toplevel = xdg_surface_get_toplevel(_ctx->xdg_srfc);
  // Needed for true window dimentions
  xdg_toplevel_add_listener(_ctx->xdg_toplevel, &toplevel_list, this);

  wl_surface_commit(_ctx->srfc);

  // Wait untill window dimentions become available
  wl_display_roundtrip(_ctx->disp);
}

void WaylandWindow::pollEvents() {
  while (wl_display_prepare_read(_ctx->disp)) {
    wl_display_dispatch_pending(_ctx->disp);
  }

  while (wl_display_flush(_ctx->disp) < 0 && EAGAIN == errno) {
    poll_single(wl_display_get_fd(_ctx->disp), POLLOUT, -1);
  }

  if (POLLIN & poll_single(wl_display_get_fd(_ctx->disp), POLLIN, 0)) {
    wl_display_read_events(_ctx->disp);
    wl_display_dispatch_pending(_ctx->disp);
  } else {
    wl_display_cancel_read(_ctx->disp);
  }

  if (wl_display_get_error(_ctx->disp)) {
    THROW_EXEPTION("Wayland protocol error");
  }
};

void WaylandWindow::createWindowSurface(VkInstance instance,
                                        VkSurfaceKHR *surface) const {
  VkWaylandSurfaceCreateInfoKHR createInf{};

  createInf.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
  createInf.display = _ctx->disp;
  createInf.surface = _ctx->srfc;
  if (vkCreateWaylandSurfaceKHR(instance, &createInf, nullptr, surface) !=
      VK_SUCCESS) {
    THROW_EXEPTION("Failed to create vulkan surface");
  };
};

WindowDims WaylandWindow::getDims() const noexcept { return dims; }

bool WaylandWindow::shouldClose() noexcept { return this->closed; };

WaylandWindow::~WaylandWindow() {
  const WaylandDeleter deleter;
  destroy_wl_obj(_ctx->srfc, deleter);
  destroy_wl_obj(_ctx->xdg_toplevel, deleter);
  destroy_wl_obj(_ctx->comp, deleter);
  destroy_wl_obj(_ctx->reg, deleter);
  destroy_wl_obj(_ctx->disp, deleter);

  LOG("Destroyed wl objects")
}
