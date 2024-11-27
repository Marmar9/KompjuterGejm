#pragma once
#include <vulkan/vulkan.h>

namespace renderer {

struct SurfaceFormat {
  VkSurfaceFormatKHR *list;
  uint32_t count;
};

struct PresentMode {
  VkPresentModeKHR *list;
  uint32_t count;
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  PresentMode presentModes;
  SurfaceFormat formats;
};

struct QueueFamilyIndices {
  uint32_t graphicsFamily;
  uint8_t graphicsFamilyExists;
  uint32_t presentationFamily;
  uint8_t presentationFamilyExists;

  bool isComplete() { return presentationFamilyExists && graphicsFamilyExists; }
};

struct DeviceCapabilities {
  SwapChainSupportDetails chainSupInf;
  QueueFamilyIndices indices;
};

} // namespace renderer
