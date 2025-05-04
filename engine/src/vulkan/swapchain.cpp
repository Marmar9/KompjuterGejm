#include "include/vulkan/swapchain.hpp"
#include "cstdlib"
#include "inc/common/exception.hpp"
#include "inc/common/loger.h"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/swapchain-builder.hpp"
#include <span>
#include <utility>
#include <vulkan/vulkan_core.h>

using namespace engine;

static int clamp(int value, int min, int max) {
  if (value < min) {
    return min;
  } else if (value > max) {
    return max;
  } else {
    return value;
  }
}

VkPresentModeKHR
chooseSwapPresentMode(const SwapChainSupportDetails &details) noexcept {
  for (int i = 0; i < details.presentModes.count; i++) {
    if (details.presentModes.list[i] == VK_PRESENT_MODE_FIFO_KHR) {

      return details.presentModes.list[i];
    }
  }
  LOG("Chosing default present mode")
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR
chooseSwapSurfaceFormat(const SwapChainSupportDetails &details) noexcept {
  for (int i = 0; i < details.formats.count; i++) {

    // LOG("Format: %d, ColorSpace: %d", details->formats.list[i].format,
    //     details->formats.list[i].colorSpace)
    if (details.formats.list[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
        details.formats.list[i].colorSpace ==
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return details.formats.list[i];
    }
  }

  LOG("Chosing Default format")
  return details.formats.list[0];
}

window::WindowDims chooseSwapExtent(const SwapChainSupportDetails &details,
                                    const window::Window &window) noexcept {

  if (details.capabilities.currentExtent.width != UINT32_MAX) {
    return window::WindowDims{details.capabilities.maxImageExtent.width,
                              details.capabilities.maxImageExtent.height};

  } else {
    window::WindowDims extent = {};
    window::WindowDims dims = window.getDims();

    extent.width = clamp(dims.width, details.capabilities.minImageExtent.width,
                         details.capabilities.maxImageExtent.width);
    extent.height =
        clamp(dims.height, details.capabilities.minImageExtent.height,
              details.capabilities.maxImageExtent.height);

    return extent;
  }
}

void Swapchain::_createImageViews() {
  _d.imageViews =
      std::span<VkImageView>(new VkImageView[_imageCount](), _imageCount);

  VkImage *images = new VkImage[_imageCount]();

  vkGetSwapchainImagesKHR(_device, _d.swapchain.get(), &_imageCount, images);

  for (size_t i = 0; i < _imageCount; i++) {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = images[i];

    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = _format.format;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(_device, &createInfo, nullptr, &_d.imageViews[i]) !=
        VK_SUCCESS) {
      delete[] images;
      THROW_EXCEPTION("failed to create image views!");
    }
  }

  delete[] images;
}

Swapchain::Swapchain(const window::Window &window,
                     const DeviceCapabilities &capabilities, VkDevice device,
                     VkSurfaceKHR surface)
    : _window(window), _deviceCapabilities(capabilities), _device(device),
      _surface(surface), _d{} {

  vkGetDeviceQueue(_device, _deviceCapabilities.indices.presentationFamily, 0,
                   &_d.presentQueue);

  _format = chooseSwapSurfaceFormat(_deviceCapabilities.chainSupInf);

  _presentMode = chooseSwapPresentMode(_deviceCapabilities.chainSupInf);

  _extent = chooseSwapExtent(_deviceCapabilities.chainSupInf, _window);

  _imageCount =
      _deviceCapabilities.chainSupInf.capabilities.minImageCount; //+ 1;

  if (_deviceCapabilities.chainSupInf.capabilities.maxImageCount > 0 &&
      _imageCount >
          _deviceCapabilities.chainSupInf.capabilities.maxImageCount) {
    _imageCount = _deviceCapabilities.chainSupInf.capabilities.maxImageCount;
  }

  uint32_t queueFamilies[2] = {};
  queueFamilies[0] = _deviceCapabilities.indices.graphicsFamily;
  queueFamilies[1] = _deviceCapabilities.indices.presentationFamily;

  _builder.reset(new vulkan::SwapchainBuilder(_surface, _device));

  _builder->setFormat(_format)
      ->setExtent(_extent)
      ->setPresentMode(_presentMode)
      ->setImageCount(_imageCount)
      ->setTransform(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
      ->setImageUsage(
          (VkImageUsageFlagBits)(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_DST_BIT))
      ->setQueueFamilies(queueFamilies);

  _builder->build(&_d.swapchain);

  vkGetSwapchainImagesKHR(_device, _d.swapchain.get(), &_imageCount, nullptr);

  // Create image views
  _createImageViews();

  _rebuildRequired = false;
}

void Swapchain::rebuild(window::WindowDims dims) {
  _extent = dims;
  _d.imageViews = std::span<VkImageView, 0>();

  _d.oldSwapchain.move(std::move(_d.swapchain));

  _d.oldSwapchain.reset(nullptr);

  _builder->setExtent(dims);

  _builder->setOldSwapchain(_d.oldSwapchain.get());
  _builder->build(&_d.swapchain);

  _createImageViews();

  _rebuildRequired = false;
}

bool Swapchain::rebuildRequired() const noexcept { return _rebuildRequired; }

uint32_t Swapchain::acquireImage(VkSemaphore imgAvailableSem) {

  if (vkAcquireNextImageKHR(_device, _d.swapchain.get(), UINT64_MAX,
                            imgAvailableSem, VK_NULL_HANDLE,
                            &_currentImgIndx) == VK_ERROR_OUT_OF_DATE_KHR) {
    _rebuildRequired = true;
  };

  return _currentImgIndx;
}

window::WindowDims Swapchain::dims() const noexcept { return _extent; };

VkSurfaceFormatKHR Swapchain::getFormat() const noexcept { return _format; };

uint32_t Swapchain::getImageCount() const { return _imageCount; };

std::span<VkImageView> Swapchain::getImageViews() const noexcept {
  return std::span<VkImageView>(_d.imageViews.get(), _imageCount);
};

Swapchain::~Swapchain() noexcept {
  // Destroy the swapchain and associated framebuffers and image views
}
