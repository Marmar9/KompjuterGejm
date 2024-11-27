#pragma once

#include "include/vulkan/handle.hpp"
#include "vulkan/renderer.hpp"
#include "vulkan/swapchain-builder.hpp"
#include "window/window-base.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>

class InitRenderer;

namespace re {
class GameEngine {
private:
  std::unique_ptr<vulkan::SwapchainBuilder> _swapchainBuilder;

  std::unique_ptr<window::Window> _window;
  std::unique_ptr<renderer::Renderer> _renderer;

  std::unique_ptr<renderer::Swapchain> _swapchain;

  vulkan::Handle<VkDeviceMemory> _devMem;

  vulkan::Handle<VkBuffer> _vertBuff;

  vulkan::Handle<VkShaderModule> _vertShad;
  vulkan::Handle<VkShaderModule> _fragShad;

  vulkan::Handle<VkRenderPass> _renderPass;
  vulkan::Handle<VkPipeline> _graphicsPipelie;
  vulkan::Handle<VkPipelineLayout> _pipelineLayout;

public:
  explicit GameEngine();
  ~GameEngine();

  void loopStart();
};
} // namespace re
