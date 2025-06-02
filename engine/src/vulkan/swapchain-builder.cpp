#include "include/vulkan/swapchain-builder.hpp"
#include "inc/common/exception.hpp"
#include "inc/common/loger.h"
#include <cstring>

using namespace engine::vulkan;

SwapchainBuilder::SwapchainBuilder(VkSurfaceKHR surface, VkDevice device)

    : _device(device), _createInfo{} {

  _createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  _createInfo.surface = surface;

  _createInfo.imageArrayLayers = 1;

  // This may change
  // _createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  _createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

  _createInfo.clipped = VK_TRUE;
}

SwapchainBuilder::~SwapchainBuilder(){

};
SwapchainBuilder *
SwapchainBuilder::setImageCount(uint32_t imageCount) noexcept {

  _createInfo.minImageCount = imageCount;

  return this;
}

SwapchainBuilder *
SwapchainBuilder::setFormat(VkSurfaceFormatKHR format) noexcept {
  _createInfo.imageFormat = format.format;
  _createInfo.imageColorSpace = format.colorSpace;
  return this;
}

SwapchainBuilder *
SwapchainBuilder::setPresentMode(const VkPresentModeKHR &presentMode) noexcept {
  _createInfo.presentMode = presentMode;
  return this;
}

SwapchainBuilder *SwapchainBuilder::setExtent(VkExtent2D vkExtent) noexcept {
  _createInfo.imageExtent = vkExtent;
  return this;
}

SwapchainBuilder *
SwapchainBuilder::setImageUsage(VkImageUsageFlagBits vkExtent) noexcept {
  _createInfo.imageUsage = vkExtent;
  return this;
}

SwapchainBuilder *SwapchainBuilder::setQueueFamilies(
    std::span<uint32_t> queueFamilyIndices) noexcept {
  _createInfo.queueFamilyIndexCount = queueFamilyIndices.size();

  if (!_pQueueFamilies.get()) {
    _pQueueFamilies.reset(new uint32_t[queueFamilyIndices.size()]());
  }

  std::memcpy(_pQueueFamilies.get(), queueFamilyIndices.data(),
              (sizeof(uint32_t) * queueFamilyIndices.size()));

  _createInfo.pQueueFamilyIndices = _pQueueFamilies.get();

  if (queueFamilyIndices[0] == queueFamilyIndices[1]) {
    _createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  } else {
    _createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
  }

  return this;
}

SwapchainBuilder *SwapchainBuilder::setTransform(
    VkSurfaceTransformFlagBitsKHR transform) noexcept {
  _createInfo.preTransform = transform;

  return this;
}

SwapchainBuilder *
SwapchainBuilder::setOldSwapchain(VkSwapchainKHR oldSwapchain) {
  _createInfo.oldSwapchain = oldSwapchain;
  return this;
};

void SwapchainBuilder::build(VkSwapchainKHR *swapchain) const {
  //  LOG("Building with: G: %u, P: %u", _createInfo.pQueueFamilyIndices[0],
  //      _createInfo.pQueueFamilyIndices[1])

  if (vkCreateSwapchainKHR(_device, &_createInfo, nullptr, swapchain) !=
      VK_SUCCESS) {
    THROW_EXCEPTION("Failed to create swapchain");
  }
}
