#include "include/window/window.hpp"

#include "include/window/wayland/wayland-window.hpp"

using namespace window;

Window *window::createWindow() { return new WaylandWindow(); };
