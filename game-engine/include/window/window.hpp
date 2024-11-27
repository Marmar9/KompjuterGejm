#pragma once
#include "window-base.hpp"

#define WINDOW_LIB_EXTENTIONS                                                  \
  VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME

namespace window {
Window *createWindow();
} // namespace window
