#pragma once
#include "include/vulkan/builder.hpp"
#include <memory>
#include <span>
#include <vulkan/vulkan_core.h>

namespace vulkan {

class SwapchainBuilder : Buildable<VkSwapchainKHR> {
  VkDevice _device;
  VkSwapchainCreateInfoKHR _createInfo;
  std::unique_ptr<uint32_t> _pQueueFamilies;

public:
  SwapchainBuilder(VkSurfaceKHR surface, VkDevice device);
  ~SwapchainBuilder();

  SwapchainBuilder *setImageCount(uint32_t imageCount) noexcept;

  SwapchainBuilder *setFormat(VkSurfaceFormatKHR format) noexcept;

  SwapchainBuilder *
  setPresentMode(const VkPresentModeKHR &presentMode) noexcept;

  SwapchainBuilder *setExtent(VkExtent2D vkExtent) noexcept;
  SwapchainBuilder *setImageUsage(VkImageUsageFlagBits vkExtent) noexcept;

  SwapchainBuilder *
  setQueueFamilies(std::span<uint32_t> queueFamilyIndices) noexcept;

  SwapchainBuilder *
  setTransform(VkSurfaceTransformFlagBitsKHR transform) noexcept;

  SwapchainBuilder *setOldSwapchain(VkSwapchainKHR oldSwapchain);

  void build(VkSwapchainKHR *swapchain) const override;
};
} // namespace vulkan
