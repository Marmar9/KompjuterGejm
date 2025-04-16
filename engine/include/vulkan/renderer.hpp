#include "inc/common/window/window-base.hpp"
#include "include/vulkan/buffer.hpp"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/structures.hpp"
#include "include/vulkan/swapchain.hpp"
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace engine {

const constexpr uint8_t framesInFlight = 3;

class Renderer {
  DeviceCapabilities _capabilities;

  const uint8_t _framesInFlight = 3;
  uint8_t _currentFrameIndex;

  using buf_t = glm::vec2;
  static constexpr uint32_t buf_b = 0;
  engine::vulkan::Buffer<buf_t> _vertBuf;

  static constexpr uint32_t _bindingCount = 1;
  VkBuffer _bindings[_bindingCount] = {};
  VkDeviceSize _offsets[_bindingCount] = {};

  struct {
    engine::vulkan::Handle<VkInstance> instance;
    VkPhysicalDevice physicalDevice;
    engine::vulkan::Handle<VkDevice> device;

    engine::vulkan::Handle<VkRenderPass> renderPass;
    engine::vulkan::Handle<VkFramebuffer *> swapchainFramebufs;

    engine::vulkan::Handle<VkShaderModule> _vertShad;
    engine::vulkan::Handle<VkShaderModule> _fragShad;
    engine::vulkan::Handle<VkPipelineLayout> _pipelineLay;
    engine::vulkan::Handle<VkPipeline> _gPipeline;

    std::unique_ptr<VkCommandBuffer[]> cmdBufs;
    VkCommandBuffer currCmdBuf;
    engine::vulkan::Handle<VkCommandPool> cmdPool;

    engine::vulkan::Handle<VkSemaphore *> imgAvailableSems;
    engine::vulkan::Handle<VkSemaphore *> renderFiniSems;
    engine::vulkan::Handle<VkFence *> inFlightFenses;
  } _v;

  struct {
    engine::vulkan::Handle<VkDebugUtilsMessengerEXT> debugMessenger;
    engine::vulkan::Handle<VkSurfaceKHR> surface;
  } _khr;

  const window::Window &_window;
  Swapchain *_swapchain;
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

  void beginFrame(window::WindowDims dims);
  void endFrame();
};
} // namespace engine
