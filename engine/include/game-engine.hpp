#pragma once
#include "include/ecs/core.hpp"

#include "include/ecs/drawing-service.hpp"

#include "vulkan/swapchain-builder.hpp"

#include <csignal>
#include <glm/ext/vector_float2.hpp>
#include <include/ecs/entity-manager.hpp>
#include <memory>
#include <vulkan/vulkan_core.h>

class InitRenderer;

namespace engine {

struct GameEngineParams {
  char **vulkanExt;
};

struct LoopContext {
  ecs::entity_t entities[ecs::entity_max];
  ecs::EntityManager *manager;
  ecs::DrawingService *dService;
};

class GameEngine {
private:
  std::unique_ptr<vulkan::SwapchainBuilder> _swapchainBuilder;

  const window::Window &_window;
  std::unique_ptr<engine::Renderer> _renderer;

  std::unique_ptr<engine::Swapchain> _swapchain;

  LoopContext _loopCtx;
  using OnRefreshCallback_T = void (*)(LoopContext &ctx);

public:
  OnRefreshCallback_T onRefreshCallback = nullptr;
  explicit GameEngine(const window::Window &window,
                      const GameEngineParams &params);
  ~GameEngine();
  void setContext(LoopContext ctx);
  void loopStart();
};

} // namespace engine
