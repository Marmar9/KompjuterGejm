#pragma once

#include "include/vulkan/buffer.hpp"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/rendering/graphics-pipeline.hpp"
#include "vulkan/renderer.hpp"
#include "vulkan/swapchain-builder.hpp"
#include <csignal>
#include <cstdint>
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

  static constexpr uint32_t _bindingCount = 1;
  VkBuffer _bindings[_bindingCount] = {};
  VkDeviceSize _offsets[_bindingCount] = {};

  engine::vulkan::Handle<VkRenderPass> _renderPass;
  std::unique_ptr<engine::vulkan::GraphicsPipeline> _gPipeline;

  using buf_t = glm::vec2;
  static constexpr uint32_t buf_b = 0;
  engine::vulkan::Buffer<buf_t> _vertBuf;

  using OnRefreshCallback_T = void (*)();

  OnRefreshCallback_T onRefreshCallback;

public:
  explicit GameEngine(const window::Window &window,
                      const GameEngineParams &params);
  ~GameEngine();

  void setBefRefreshCallback(OnRefreshCallback_T callback);

  void loopStart();
};

} // namespace engine
