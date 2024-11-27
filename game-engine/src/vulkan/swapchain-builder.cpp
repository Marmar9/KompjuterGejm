#include "include/vulkan/swapchain-builder.hpp"
#include "include/utils/exeption.hpp"
#include "include/utils/loger.h"
#include <cstring>

using namespace vulkan;

SwapchainBuilder::SwapchainBuilder(VkSurfaceKHR surface, VkDevice device)

    : _device(device), _structs{} {

  _structs.createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  _structs.createInfo.surface = surface;

  _structs.createInfo.imageArrayLayers = 1;

  // This may change

  _structs.createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  // _structs.createInfo.presentMode = _presentMode;
  _structs.createInfo.clipped = VK_TRUE;
}

SwapchainBuilder::~SwapchainBuilder(){

};
SwapchainBuilder *
SwapchainBuilder::setImageCount(uint32_t imageCount) noexcept {

  _structs.createInfo.minImageCount = imageCount;

  return this;
}

SwapchainBuilder *
SwapchainBuilder::setFormat(VkSurfaceFormatKHR format) noexcept {
  _structs.createInfo.imageFormat = format.format;
  _structs.createInfo.imageColorSpace = format.colorSpace;
  return this;
}

SwapchainBuilder *
SwapchainBuilder::setPresentMode(const VkPresentModeKHR &presentMode) noexcept {
  _structs.createInfo.presentMode = presentMode;
  return this;
}

SwapchainBuilder *SwapchainBuilder::setExtent(VkExtent2D vkExtent) noexcept {
  _structs.createInfo.imageExtent = vkExtent;
  return this;
}

SwapchainBuilder *
SwapchainBuilder::setImageUsage(VkImageUsageFlagBits vkExtent) noexcept {
  _structs.createInfo.imageUsage = vkExtent;
  return this;
}

SwapchainBuilder *SwapchainBuilder::setQueueFamilies(
    std::span<uint32_t> queueFamilyIndices) noexcept {
  _structs.createInfo.queueFamilyIndexCount = queueFamilyIndices.size();

  if (!_pQueueFamilies.get()) {
    _pQueueFamilies.reset(new uint32_t[queueFamilyIndices.size()]());
  }

  std::memcpy(_pQueueFamilies.get(), queueFamilyIndices.data(),
              (sizeof(uint32_t) * queueFamilyIndices.size()));

  _structs.createInfo.pQueueFamilyIndices = _pQueueFamilies.get();

  if (queueFamilyIndices.size() == 1) {
    _structs.createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  } else {
    _structs.createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
  }

  return this;
}

SwapchainBuilder *SwapchainBuilder::setTransform(
    VkSurfaceTransformFlagBitsKHR transform) noexcept {
  _structs.createInfo.preTransform = transform;

  return this;
}

VkSwapchainKHR SwapchainBuilder::build(VkSwapchainKHR oldSwapchain) {
  LOG("Building with: G: %u, P: %u", _structs.createInfo.pQueueFamilyIndices[0],
      _structs.createInfo.pQueueFamilyIndices[1])
  if (oldSwapchain) {
    _structs.createInfo.oldSwapchain = oldSwapchain;
  }

  VkSwapchainKHR swapchain;
  VkResult res =
      vkCreateSwapchainKHR(_device, &_structs.createInfo, nullptr, &swapchain);
  if (res != VK_SUCCESS) {
    THROW_EXEPTION("Failed to create swapchain: %d", res);
  }

  return swapchain;
}
