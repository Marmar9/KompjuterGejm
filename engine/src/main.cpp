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
  while (!_window.shouldClose()) {
    // this->onRefreshCallback();

    window::WindowDims curDims = _window.getDims();
    _renderer->beginFrame(curDims);

    _renderer->endFrame();

    _window.pollEvents();
  }
}

engine::GameEngine::~GameEngine() {}
