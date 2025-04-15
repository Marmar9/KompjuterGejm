#include "inc/game/game.hpp"
#include "include/game-engine.hpp"
#include "window/wayland-window.hpp"

#define WINDOW_EXTENTIONS VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME

int main(int argc, char *argv[]) {
  window::WaylandWindow window = window::WaylandWindow();
  engine::GameEngineParams params{};

  return game::entryPoint(window, params);
}
