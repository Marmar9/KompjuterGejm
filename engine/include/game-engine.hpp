#pragma once
#include "vulkan/swapchain-builder.hpp"

#include <glm/ext/vector_float2.hpp>
#include <include/ecs/entity-manager.hpp>
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

  window::Window &_window;
  std::unique_ptr<engine::Renderer> _renderer;

  std::unique_ptr<engine::Swapchain> _swapchain;

public:
  explicit GameEngine(window::Window &window, const GameEngineParams &params);
  ~GameEngine();
  void beginFrame();
  void render();
};

} // namespace engine
