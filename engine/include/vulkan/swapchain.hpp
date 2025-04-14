#pragma once
#include "inc/common/window/window-base.hpp"
#include "inc/common/window/window-dims.hpp"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/swapchain-builder.hpp"
#include "structures.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>
namespace engine {

class Swapchain {
private:
  const window::Window &_window;
  const DeviceCapabilities &_deviceCapabilities;
  VkDevice _device;
  VkSurfaceKHR _surface;
  VkRenderPass _renderPass;

  uint32_t _imageCount;
  VkSurfaceFormatKHR _format;
  window::WindowDims _extent;
  VkPresentModeKHR _presentMode;
  uint32_t _currentImgIndx;

  std::unique_ptr<vulkan::SwapchainBuilder> _builder;

  struct {
    vulkan::Handle<VkSwapchainKHR> swapchain;
    vulkan::Handle<VkSwapchainKHR> oldSwapchain;
    vulkan::Handle<VkImageView *> imageViews;
    VkQueue presentQueue;
  } _d;
  bool _rebuildRequired = true;
  void _createImageViews();

public:
  Swapchain(const window::Window &, const DeviceCapabilities &, VkDevice,
            VkSurfaceKHR);
  ~Swapchain() noexcept;

  uint32_t acquireImage(VkSemaphore imgAvailableSem);
  uint32_t getImageCount() const;
  void present(VkSemaphore imgAvailableSem);
  void rebuild(window::WindowDims dims);
  bool rebuildRequired() const noexcept;
  const vulkan::Handle<VkImageView *> &getImageViews() const noexcept;

  VkSurfaceFormatKHR getFormat() const noexcept;
  window::WindowDims dims() const noexcept;
};

} // namespace engine
