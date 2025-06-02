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
void engine::GameEngine::beginFrame() {
  window::WindowDims curDims = _window.getDims();
  _renderer->poll();
  _renderer->beginFrame(curDims);
};

void engine::GameEngine::render() { _renderer->endFrame(); };

engine::GameEngine::~GameEngine() {
  ecs::EntityManager::destroy();
  ecs::DrawingService::destroy();
}
} // namespace engine
