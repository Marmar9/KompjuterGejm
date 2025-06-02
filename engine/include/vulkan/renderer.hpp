#pragma once
#define GLM_FORCE_RADIANS
#include "inc/common/window/window-base.hpp"
#include "include/ecs/core.hpp"
#include "include/vulkan/buffer.hpp"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/structures.hpp"
#include "include/vulkan/swapchain.hpp"
#include "include/vulkan/vertex.hpp"
#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <span>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace engine {
struct TransformUBO {
  glm::mat4 model;
  glm::mat4 camera;
  glm::mat4 perspective;
};

using RectangleVBuff =
    vulkan::VertexBuffer<vulkan::Vertex,
                         vulkan::InstanceData::VertCount * ecs::entity_max>;

using TransformUBuff = vulkan::UniformBuffer<TransformUBO, 1>;

class Renderer {
  DeviceCapabilities _capabilities;

  static constexpr uint8_t FramesInFlight = 3;
  static constexpr uint32_t _VertexBuffPerFrame = 1;
  uint8_t _currentFrameIndex = 1;

  struct {
    engine::vulkan::Handle<VkInstance> instance;
    VkPhysicalDevice physicalDevice;
    engine::vulkan::Handle<VkDevice> device;

    engine::vulkan::Handle<VkRenderPass> renderPass;
    engine::vulkan::Handle<std::span<VkFramebuffer>> swapchainFramebufs;

    engine::vulkan::Handle<VkShaderModule> _vertShad;
    engine::vulkan::Handle<VkShaderModule> _fragShad;
    engine::vulkan::Handle<VkDescriptorSetLayout> descriptorSetLay;
    engine::vulkan::Handle<VkPipelineLayout> _pipelineLay;
    engine::vulkan::Handle<VkPipeline> _gPipeline;

    vulkan::Handle<VkDescriptorPool> descrPool;
    std::array<VkDescriptorSet, FramesInFlight> descrSets;

    engine::vulkan::Handle<VkCommandPool> cmdPool;
    std::array<VkCommandBuffer, FramesInFlight> cmdBufs;

    engine::vulkan::Handle<std::array<VkSemaphore, FramesInFlight>>
        imgAvailableSems;
    engine::vulkan::Handle<std::array<VkSemaphore, FramesInFlight>>
        renderFiniSems;
    engine::vulkan::Handle<std::array<VkFence, FramesInFlight>> inFlightFenses;
    engine::vulkan::Handle<std::array<VkFence, FramesInFlight>>
        vertexReadDoneFence;

  } _v;

  std::array<std::unique_ptr<RectangleVBuff>, FramesInFlight> _vertBufs;
  std::array<std::unique_ptr<TransformUBuff>, FramesInFlight> _uniformBufs;

  VkDeviceSize _offsets[_VertexBuffPerFrame] = {};

  struct {
    engine::vulkan::Handle<VkDebugUtilsMessengerEXT> debugMessenger;
    engine::vulkan::Handle<VkSurfaceKHR> surface;
  } _khr;

  const window::Window &_window;
  std::unique_ptr<Swapchain> _swapchain;
  VkQueue _graphicsQueue;

  void _createCommandPool();
  void _createCommandBuffers();
  void _createSyncObjects();

  void _createRenderPasses();
  void _createGraphicsPipeline();
  void _createDescriptorStorage();
  void _createSwapchainFramebufs(window::WindowDims dims);
  void _destroySwapchainFramebufs();

  void _createDescriptorLayout();

public:
  Renderer(const window::Window &);
  ~Renderer();
  void init(Swapchain *swapchain);
  void poll();
  void beginFrame(window::WindowDims dims);

  void render(uint32_t firstV, uint32_t vCount);
  void endFrame();

  std::span<engine::vulkan::Vertex> const getVertBuffer() noexcept;
  std::span<engine::TransformUBO> const getUniBuffer() noexcept;
};
} // namespace engine
