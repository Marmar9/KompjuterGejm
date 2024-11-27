#include "include/vulkan/structures.hpp"
#include "include/window/window-base.hpp"
#include "renderer-base.hpp"
#include <vulkan/vulkan_core.h>

namespace renderer {

class Renderer : private RendererBase {
private:
  void _createCommandPool();
  void _createCommandBuffers();
  void _createSyncObjects();
  void _createRenderPass();

  const window::Window &_window;
  Swapchain *_swapchain;
  VkQueue _graphicsQueue;

public:
  Renderer(const window::Window &);
  ~Renderer();
  void init(Swapchain *swapchain);

  VkPhysicalDevice getPhysicalDevice() const noexcept;
  VkDevice getDevice() const noexcept;
  VkInstance getInstance() const noexcept;
  VkSurfaceKHR getSurface() const noexcept;

  void createSwapchainFramebufs(window::WindowDims dims,
                                const VkRenderPass renderPass);
  void destroySwapchainFramebufs();

  VkCommandBuffer beginFrame();
  void endFrame();
  uint32_t beginRenderPass(VkRenderPass renderPass);
  void endRenderPass(VkRenderPass renderPass);

  DeviceCapabilities getDevCaps() const noexcept;
};
} // namespace renderer
