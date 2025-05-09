#pragma once
#include "protocol.h"
#include "wayland-cursor.h"
#include "xdg-shell-protocol.h"

struct WaylandDeleter {
  void operator()(wl_buffer *wl_buffer) const noexcept {
    wl_buffer_destroy(wl_buffer);
  }

  void operator()(wl_compositor *wl_compositor) const noexcept {
    wl_compositor_destroy(wl_compositor);
  }

  void operator()(wl_cursor_theme *wl_cursor_theme) const noexcept {
    wl_cursor_theme_destroy(wl_cursor_theme);
  }

  void operator()(wl_display *display) const noexcept {
    wl_display_disconnect(display);
  }

  void operator()(wl_keyboard *wl_keyboard) const noexcept {
    wl_keyboard_release(wl_keyboard);
  }

  void operator()(wl_pointer *wl_pointer) const noexcept {
    wl_pointer_release(wl_pointer);
  }

  void operator()(wl_registry *wl_registry) const noexcept {
    wl_registry_destroy(wl_registry);
  }

  void operator()(wl_seat *wl_seat) const noexcept { wl_seat_release(wl_seat); }

  void operator()(wl_shm *wl_shm) const noexcept { wl_shm_destroy(wl_shm); }

  void operator()(wl_shm_pool *wl_shm_pool) const noexcept {
    wl_shm_pool_destroy(wl_shm_pool);
  }

  void operator()(wl_surface *wl_surface) const noexcept {
    wl_surface_destroy(wl_surface);
  }

  void operator()(xdg_surface *xdg_surface) const noexcept {
    xdg_surface_destroy(xdg_surface);
  }

  void operator()(xdg_toplevel *xdg_toplevel) const noexcept {
    xdg_toplevel_destroy(xdg_toplevel);
  }

  void operator()(xdg_wm_base *xdg_wm_base) const noexcept {
    xdg_wm_base_destroy(xdg_wm_base);
  }
};

// Concept for WaylandDeletable
template <typename T>
concept WaylandDeletable =
    requires(T *t, const WaylandDeleter &d) { requires noexcept(d(t)); };

// WaylandPointer as a raw pointer
template <WaylandDeletable T> using WaylandPointer = T *;

// Manual cleanup function to use with raw pointer
template <WaylandDeletable T>
void destroy_wl_obj(T *ptr, const WaylandDeleter &deleter) noexcept {
  if (ptr) {
    deleter(ptr); // Calls the appropriate deleter for the type
  }
}
