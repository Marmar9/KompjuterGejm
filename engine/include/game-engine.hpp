#pragma once
#include "vulkan/renderer.hpp"
#include "vulkan/swapchain-builder.hpp"
#include <csignal>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <vulkan/vulkan_core.h>

class InitRenderer;

namespace engine {

struct GameEngineParams {
  char **vulkanExt;
};

class GameEngine {
private:
  std::unique_ptr<vulkan::SwapchainBuilder> _swapchainBuilder;

  const window::Window &_window;
  std::unique_ptr<engine::Renderer> _renderer;

  std::unique_ptr<engine::Swapchain> _swapchain;

  using OnRefreshCallback_T = void (*)();

public:
  OnRefreshCallback_T onRefreshCallback = nullptr;
  explicit GameEngine(const window::Window &window,
                      const GameEngineParams &params);
  ~GameEngine();

  void loopStart();
};

} // namespace engine
