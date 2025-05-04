#include "inc/common/exception.hpp"
#include "inc/common/window/window-dims.hpp"
#include "include/game-engine.hpp"
#include <cstdlib>
#include <cstring>
#include <glm/ext/vector_float2.hpp>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

engine::GameEngine::GameEngine(const window::Window &window,
                               const engine::GameEngineParams &params)
    : _window(window) {
  _renderer.reset(new engine::Renderer(_window));
}

void engine::GameEngine::loopStart() {
  if (!this->onRefreshCallback) {
    THROW_EXCEPTION("Renderer::onRefreshCallback is not initialized");
  }

  while (!_window.shouldClose()) {

    window::WindowDims curDims = _window.getDims();
    _renderer->poll();
    this->onRefreshCallback();

    _renderer->beginFrame(curDims);

    _renderer->endFrame();

    _window.pollEvents();
  }
}

engine::GameEngine::~GameEngine() {}
