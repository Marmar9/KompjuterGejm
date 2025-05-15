#include "inc/common/exception.hpp"
#include "inc/common/loger.h"
#include "inc/common/window/window-dims.hpp"
#include "include/ecs/drawing-service.hpp"
#include "include/game-engine.hpp"
#include <glm/ext/vector_float2.hpp>
#include <unistd.h>
#include <vulkan/vulkan_core.h>
namespace engine {

engine::GameEngine::GameEngine(window::Window &window,
                               const engine::GameEngineParams &params)
    : _window(window) {
  _renderer.reset(new engine::Renderer(_window));

  ecs::DrawingService::init(_renderer.get());
  ecs::DrawingService &dService = ecs::DrawingService::getInstance();
  ecs::EntityManager::init(dService);
}

void engine::GameEngine::loopStart() {
  if (!this->onRefreshCallback) {
    THROW_EXCEPTION("Renderer::onRefreshCallback is not initialized");
  }

  // _loopCtx.manager = &ecs::EntityManager::getInstance();
  //
  _loopCtx.dService = &ecs::DrawingService::getInstance();
  while (!_window.shouldClose()) {
    window::WindowDims curDims = _window.getDims();
    _renderer->poll();

    _renderer->beginFrame(curDims);
    this->onRefreshCallback(_loopCtx);
    _renderer->endFrame();

    _window.pollEvents();
  }
}

void GameEngine::setContext(LoopContext ctx) { _loopCtx = ctx; };

engine::GameEngine::~GameEngine() {
  ecs::EntityManager::destroy();
  ecs::DrawingService::destroy();
}
} // namespace engine
