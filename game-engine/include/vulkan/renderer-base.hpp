#pragma once

#include "include/vulkan/handle.hpp"
#include "swapchain.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>

const constexpr uint8_t framesInFlight = 3;

namespace renderer {
class RendererBase {
protected:
  RendererBase();
  RendererBase(const RendererBase &) = delete;
  RendererBase(RendererBase &&) noexcept = delete;
  ~RendererBase() = default;

  RendererBase &operator=(const RendererBase &) = delete;
  RendererBase &operator=(RendererBase &&) noexcept = delete;

  DeviceCapabilities _capabilities;

  const uint8_t _framesInFlight = 3;
  uint8_t _currentFrameIndex;

  struct {
    vulkan::Handle<VkInstance> instance;
    VkPhysicalDevice physicalDevice;
    vulkan::Handle<VkDevice> device;

    std::unique_ptr<VkCommandBuffer[]> cmdBufs;
    vulkan::Handle<VkCommandPool> cmdPool;

    vulkan::Handle<VkSemaphore *> imgAvailableSems;
    vulkan::Handle<VkSemaphore *> renderFiniSems;
    vulkan::Handle<VkFence *> inFlightFenses;

    vulkan::Handle<VkFramebuffer *> swapchainFramebufs;
  } _v;
  struct {
    vulkan::Handle<VkDebugUtilsMessengerEXT> debugMessenger;
    vulkan::Handle<VkSurfaceKHR> surface;
  } _khr;
};
} // namespace renderer
