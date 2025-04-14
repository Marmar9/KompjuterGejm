#pragma once

#include "include/vulkan/handle.hpp"
#include "structures.hpp"
#include <memory>
#include <vulkan/vulkan_core.h>
const constexpr uint8_t framesInFlight = 3;

namespace engine {
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
    engine::vulkan::Handle<VkInstance> instance;
    VkPhysicalDevice physicalDevice;
    engine::vulkan::Handle<VkDevice> device;

    std::unique_ptr<VkCommandBuffer[]> cmdBufs;
    engine::vulkan::Handle<VkCommandPool> cmdPool;

    engine::vulkan::Handle<VkSemaphore *> imgAvailableSems;
    engine::vulkan::Handle<VkSemaphore *> renderFiniSems;
    engine::vulkan::Handle<VkFence *> inFlightFenses;

    engine::vulkan::Handle<VkFramebuffer *> swapchainFramebufs;
  } _v;
  struct {
    engine::vulkan::Handle<VkDebugUtilsMessengerEXT> debugMessenger;
    engine::vulkan::Handle<VkSurfaceKHR> surface;
  } _khr;
};
} // namespace engine
