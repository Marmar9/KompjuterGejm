#include "inc/common/window/window-base.hpp"
#include "include/vulkan/buffer.hpp"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/structures.hpp"
#include "include/vulkan/swapchain.hpp"
#include <array>
#include <glm/glm.hpp>
#include <memory>
#include <span>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace engine {

class Renderer {
  DeviceCapabilities _capabilities;

  static constexpr uint8_t FramesInFlight = 3;
  uint8_t _currentFrameIndex = 1;

  struct {
    engine::vulkan::Handle<VkInstance> instance;
    VkPhysicalDevice physicalDevice;
    engine::vulkan::Handle<VkDevice> device;

    engine::vulkan::Handle<VkRenderPass> renderPass;
    engine::vulkan::Handle<std::span<VkFramebuffer>> swapchainFramebufs;

    engine::vulkan::Handle<VkShaderModule> _vertShad;
    engine::vulkan::Handle<VkShaderModule> _fragShad;
    engine::vulkan::Handle<VkPipelineLayout> _pipelineLay;
    engine::vulkan::Handle<VkPipeline> _gPipeline;

    std::array<VkCommandBuffer, FramesInFlight> cmdBufs;

    VkCommandBuffer currCmdBuf;
    engine::vulkan::Handle<VkCommandPool> cmdPool;

    engine::vulkan::Handle<std::array<VkSemaphore, FramesInFlight>>
        imgAvailableSems;
    engine::vulkan::Handle<std::array<VkSemaphore, FramesInFlight>>
        renderFiniSems;
    engine::vulkan::Handle<std::array<VkFence, FramesInFlight>> inFlightFenses;
    engine::vulkan::Handle<std::array<VkFence, FramesInFlight>>
        vertexReadDoneFence;

  } _v;

  static constexpr uint32_t _vertexBuffCount = 1;
  std::unique_ptr<vulkan::VertexBuffer<vulkan::MyVertex>> _vertBuf;

  VkBuffer _vBuffers[_vertexBuffCount] = {};
  VkDeviceSize _offsets[_vertexBuffCount] = {};

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
  void _createSwapchainFramebufs(window::WindowDims dims);
  void _destroySwapchainFramebufs();

public:
  Renderer(const window::Window &);
  ~Renderer();
  void init(Swapchain *swapchain);
  void poll();
  void beginFrame(window::WindowDims dims);

  void endFrame();
};
} // namespace engine
