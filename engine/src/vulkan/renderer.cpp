#include "include/vulkan/renderer.hpp"

#include "inc/common/loger.h"
#include "inc/common/window/window-base.hpp"
#include <array>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <span>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <glm/glm.hpp>

#include "inc/common/exception.hpp"
#include "inc/common/static-arr-len.hpp"
#include "include/vulkan/buffer.hpp"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/rendering/graphics-pipeline-builder.hpp"
#include "include/vulkan/structures.hpp"
#include "include/vulkan/swapchain.hpp"
#include "include/vulkan/vertex.hpp"
#include "vulkan/vulkan_core.h"

#ifdef ANDROID

#include "vulkan/vulkan_android.h"

#endif

#ifndef ANDROID

#include "vulkan/vulkan_wayland.h"
#endif

// Utility
const char *extensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,

#ifdef ANDROID
    VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#endif
#ifndef ANDROID
    VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#endif

#ifdef DEBUG

// Disable debug utils extention's custom messanger
// VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif // DEBUG

};

const char *deviceExtentions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
using namespace engine;

#define E_VK_VERSION VK_API_VERSION_1_0

#ifdef DEBUG

const std::array vLayers = {"VK_LAYER_KHRONOS_validation"};

int check_layers() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, NULL);

  VkLayerProperties *availableLayers =
      (VkLayerProperties *)malloc(layerCount * sizeof(VkLayerProperties));

  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
  for (int i = 0; i < vLayers.size(); i++) {
    int layerFound = 0;

    for (uint32_t j = 0; j < layerCount; j++) {
      if (strcmp(vLayers.data()[i], availableLayers[j].layerName) == 0) {
        layerFound = 1;
        break;
      }
    }

    if (!layerFound) {
      free(availableLayers);
      return 0;
    }
  }

  free(availableLayers);
  return 1;
}

// Disable debug utils extention's custom messanger
// static VKAPI_ATTR VkBool32 VKAPI_CALL
// debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//               VkDebugUtilsMessageTypeFlagsEXT messageType,
//               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
//               void *pUserData) {
//   switch (messageSeverity) {
//   case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
//     LOG("%s\n", pCallbackData->pMessage);
//     break;
//   case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
//     LOG("%s\n", pCallbackData->pMessage);
//     break;
//   case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
//     LOG("%s\n", pCallbackData->pMessage);
//     break;
//   case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
//     LOG("%s\n", pCallbackData->pMessage);
//     break;
//   default:
//     break;
//   }
//   return VK_FALSE;
// }
//
// VkResult CreateDebugUtilsMessengerEXT(
//     VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT
//     *pCreateInfo, const VkAllocationCallbacks *pAllocator,
//     VkDebugUtilsMessengerEXT *pDebugMessenger) {
//   PFN_vkCreateDebugUtilsMessengerEXT func =
//       (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
//           instance, "vkCreateDebugUtilsMessengerEXT");
//
//   if (func != 0) {
//     return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
//   } else {
//     return VK_ERROR_EXTENSION_NOT_PRESENT;
//   }
// }

#endif // DEBUG

int check_extensions(const char **requiredExtensions,
                     int requiredExtensionCount) {
  uint32_t extensionCount;

  // Get the number of available extensions
  vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

  // Allocate memory for the available extensions
  VkExtensionProperties *availableExtensions = (VkExtensionProperties *)malloc(
      extensionCount * sizeof(VkExtensionProperties));

  // Retrieve the available extensions
  vkEnumerateInstanceExtensionProperties(NULL, &extensionCount,
                                         availableExtensions);

  // Iterate over each required extension
  for (int i = 0; i < requiredExtensionCount; i++) {
    int extensionFound = 0;

    // Check if the required extension is in the list of available extensions
    for (uint32_t j = 0; j < extensionCount; j++) {
      if (strcmp(requiredExtensions[i], availableExtensions[j].extensionName) ==
          0) {
        extensionFound = 1;
        break;
      }
    }

    // If any required extension is not found, free memory and return 0 (false)
    if (!extensionFound) {
      free(availableExtensions);
      LOG("Extention %s not found", requiredExtensions[i]);
      return 0;
    }
  }

  // Free memory and return 1 (true) if all required extensions were found
  free(availableExtensions);
  return 1;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  VkExtensionProperties *availableExtensions = (VkExtensionProperties *)calloc(
      extensionCount, sizeof(VkExtensionProperties));

  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions);

  for (int i = 0; i < STATIC_ARR_LEN(deviceExtentions); i++)
    for (int j = 0; j < extensionCount; j++) {
      bool isFound = false;

      if (strcmp(deviceExtentions[i], availableExtensions[j].extensionName)) {
        isFound = true;
        break;
      }

      if (!isFound) {
        free(availableExtensions);
        return false;
      }
    }

  free(availableExtensions);
  return true;
}

static void findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface,
                              QueueFamilyIndices &indices) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, 0);

  VkQueueFamilyProperties *queueFamilies =
      new VkQueueFamilyProperties[queueFamilyCount]();

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies);

  for (int i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      indices.graphicsFamilyExists = 1;
    }

    VkBool32 presentSupport = 0;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (presentSupport) {
      indices.presentationFamily = i;
      indices.presentationFamilyExists = 1;
    }

    if (indices.presentationFamilyExists && indices.graphicsFamilyExists &&
        indices.presentationFamily | indices.graphicsFamily) {
      delete[] queueFamilies;
      return;
    }
  }

  delete[] queueFamilies;
}

void createInstance(VkInstance *instance,
                    VkDebugUtilsMessengerEXT *debugMessenger) {
  uint32_t instanceVersion = 0;
  if (vkEnumerateInstanceVersion(&instanceVersion) == VK_SUCCESS) {
    LOG("Vulkan Loader supports API version: %d.%d.%d\n",
        VK_VERSION_MAJOR(instanceVersion), VK_VERSION_MINOR(instanceVersion),
        VK_VERSION_PATCH(instanceVersion));
  } else {
    LOG("Failed to get Vulkan instance version.\n");
  }

  if (!check_extensions(extensions, STATIC_ARR_LEN(extensions))) {
    THROW_EXCEPTION("Extention requested but not available!");
  }

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Game engine";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = E_VK_VERSION;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = STATIC_ARR_LEN(extensions);
  createInfo.ppEnabledExtensionNames = extensions;

#ifdef DEBUG
  LOG("Enabling layers")

  // Disable debug utils extention's custom messanger
  // VkDebugUtilsMessengerCreateInfoEXT debugMessangerCreateInfo = {};

  // debugMessangerCreateInfo.sType =
  //     VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  // debugMessangerCreateInfo.messageSeverity =
  //     VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
  //     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
  //     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  // debugMessangerCreateInfo.messageType =
  //     VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
  //     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
  //     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  // debugMessangerCreateInfo.pfnUserCallback = debugCallback;
  // debugMessangerCreateInfo.pUserData = 0; // Optional
  //

  if (!check_layers()) {
    THROW_EXCEPTION("Validation layers requested, but not available!");
  }

  createInfo.enabledLayerCount = vLayers.size();
  createInfo.ppEnabledLayerNames = vLayers.data();
  createInfo.pNext = nullptr;

  // Disable debug utils extention's custom messanger
  //  createInfo.pNext =
  //      (VkDebugUtilsMessengerCreateInfoEXT *)&debugMessangerCreateInfo;

#endif // DEBUG

  VkResult res = vkCreateInstance(&createInfo, 0, instance);
  if (res != VK_SUCCESS) {
    THROW_EXCEPTION("Vulkan create instance failed, %d", res);
  }

#ifdef DEBUG

  // Disable debug utils extention's custom messanger
  // if (CreateDebugUtilsMessengerEXT(*instance, &debugMessangerCreateInfo, 0,
  //                                  debugMessenger) != VK_SUCCESS) {
  //   THROW_EXCEPTION("failed to set up debug messenger!");
  // }

#endif // DEBUG
}

void createLogicalDevice(VkPhysicalDevice physicalDevice,
                         DeviceCapabilities *capabilities,
                         vulkan::Handle<VkDevice> &device) {

  VkPhysicalDeviceFeatures deviceFeatures = {};
  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  float queuePriority = 1.0f;

  if (capabilities->indices.graphicsFamily ==
      capabilities->indices.presentationFamily) {
    VkDeviceQueueCreateInfo queueCreateInfo[1] = {};
    // Structure for graphics queue
    queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].queueFamilyIndex = capabilities->indices.graphicsFamily;
    queueCreateInfo[0].queueCount = 1;
    queueCreateInfo[0].pQueuePriorities = &queuePriority;

    createInfo.pQueueCreateInfos = queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

  } else {
    VkDeviceQueueCreateInfo queueCreateInfo[2] = {};
    queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].queueFamilyIndex = capabilities->indices.graphicsFamily;
    queueCreateInfo[0].queueCount = 1;
    queueCreateInfo[0].pQueuePriorities = &queuePriority;

    // Structure for presentationQueue
    queueCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[1].queueFamilyIndex =
        capabilities->indices.presentationFamily;
    queueCreateInfo[1].queueCount = 1;

    queueCreateInfo[1].pQueuePriorities = &queuePriority;

    createInfo.pQueueCreateInfos = queueCreateInfo;
    createInfo.queueCreateInfoCount = 2;
  }

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = STATIC_ARR_LEN(deviceExtentions);
  createInfo.ppEnabledExtensionNames = deviceExtentions;

#ifdef DEBUG
  createInfo.enabledLayerCount = vLayers.size();
  createInfo.ppEnabledLayerNames = vLayers.data();

#endif // DEBUG

  VkDevice deviceP;
  if (vkCreateDevice(physicalDevice, &createInfo, 0, &deviceP) != VK_SUCCESS) {
    THROW_EXCEPTION("Failed to create physical device");
  }
  device.reset(deviceP);
}

void Renderer::_createCommandPool() {
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  poolInfo.queueFamilyIndex = _capabilities.indices.graphicsFamily;

  if (vkCreateCommandPool(_v.device.get(), &poolInfo, nullptr, &_v.cmdPool) !=
      VK_SUCCESS) {
    THROW_EXCEPTION("failed to create command pool!");
  }
}

void Renderer::_createCommandBuffers() {
  // Command buffers
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = _v.cmdPool.get();
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = FramesInFlight;

  if (vkAllocateCommandBuffers(_v.device.get(), &allocInfo,
                               _v.cmdBufs.data()) != VK_SUCCESS) {
    THROW_EXCEPTION("failed to allocate command buffers!");
  }
}

void querySwapChainSupport(VkPhysicalDevice physicalDevice,
                           VkSurfaceKHR surface,
                           SwapChainSupportDetails &details) {
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                            &details.capabilities);

  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                       &details.formats.count, nullptr);
  details.formats.list = (VkSurfaceFormatKHR *)calloc(
      details.formats.count, sizeof(VkSurfaceFormatKHR));

  if (details.formats.count != 0) {
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice, surface, &details.formats.count, details.formats.list);
  }

  vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDevice, surface, &details.presentModes.count, nullptr);
  details.presentModes.list = (VkPresentModeKHR *)calloc(
      details.presentModes.count, sizeof(VkPresentModeKHR));
  if (details.presentModes.count != 0) {
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                              &details.presentModes.count,
                                              details.presentModes.list);
  }
};

bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                      DeviceCapabilities &capabilities) {
  findQueueFamilies(physicalDevice, surface, capabilities.indices);
  LOG("QueueFamilies %d,%d", capabilities.indices.graphicsFamily,
      capabilities.indices.presentationFamily)

  querySwapChainSupport(physicalDevice, surface, capabilities.chainSupInf);

  if (!checkDeviceExtensionSupport(physicalDevice)) {
    return false;
  };

  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

  LOG("Device API version: %d.%d.%d=%d\n",
      VK_VERSION_MAJOR(deviceProperties.apiVersion),
      VK_VERSION_MINOR(deviceProperties.apiVersion),
      VK_VERSION_PATCH(deviceProperties.apiVersion),
      deviceProperties.apiVersion);

  if (deviceProperties.apiVersion < E_VK_VERSION) {
    return false;
  }

  if ((capabilities.chainSupInf.formats.list == nullptr) ||
      (capabilities.chainSupInf.presentModes.list == nullptr)) {
    return false;
  };

  return capabilities.indices.isComplete();
}

void Renderer::_createSyncObjects() {
  // Sync objs
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  for (int i = 0; i < FramesInFlight; i++) {
    if (vkCreateSemaphore(_v.device.get(), &semaphoreInfo, nullptr,
                          &_v.imgAvailableSems[i]) != VK_SUCCESS ||
        vkCreateSemaphore(_v.device.get(), &semaphoreInfo, nullptr,
                          &_v.renderFiniSems[i]) != VK_SUCCESS ||
        vkCreateFence(_v.device.get(), &fenceInfo, nullptr,
                      &_v.inFlightFenses[i]) != VK_SUCCESS) {
      THROW_EXCEPTION("failed to create semaphores!");
    }
  }
}
void Renderer::_createRenderPasses() {

  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = _swapchain->getFormat().format;

  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  // VkSubpassDependency dependency{};
  // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  // dependency.dstSubpass = 0;

  // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // dependency.srcAccessMask = 0;

  // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  // enginenderPassInfo.dependencyCount = 1;
  // enginenderPassInfo.pDependencies = &dependency;
  renderPassInfo.dependencyCount = 0;
  renderPassInfo.pDependencies = nullptr;

  if (vkCreateRenderPass(_v.device.get(), &renderPassInfo, nullptr,
                         &_v.renderPass) != VK_SUCCESS) {
    THROW_EXCEPTION("failed to create enginender pass!");
  }
}

void Renderer::_createSwapchainFramebufs(window::WindowDims dims) {
  std::span<VkImageView> imgViews = _swapchain->getImageViews();

  _v.swapchainFramebufs =
      std::span(new VkFramebuffer[imgViews.size()], imgViews.size());

  for (size_t i = 0; i < _swapchain->getImageCount(); i++) {
    VkImageView attachments[] = {imgViews[i]};
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = _v.renderPass.get();
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = dims.width;
    framebufferInfo.height = dims.height;
    framebufferInfo.layers = 1;
    if (vkCreateFramebuffer(_v.device.get(), &framebufferInfo, nullptr,
                            &_v.swapchainFramebufs[i]) != VK_SUCCESS) {
      THROW_EXCEPTION("failed to create framebuffer!");
    }
  }
}
void Renderer::_destroySwapchainFramebufs() {
  _v.swapchainFramebufs = std::span<VkFramebuffer>();
};

void Swapchain::present(VkSemaphore imgAvailableSem) {
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &imgAvailableSem;

  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &_d.swapchain;
  presentInfo.pImageIndices = &_currentImgIndx;
  presentInfo.pResults = nullptr; // Optional

  VkResult result = vkQueuePresentKHR(_d.presentQueue, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    _rebuildRequired = true;
  } else if (result != VK_SUCCESS) {
    THROW_EXCEPTION("Failed to present image");
  }
}

void Renderer::_createGraphicsPipeline() {

  // Binding for buffer 1
  _vertBuf.reset(new RectangleVBuff(_v.device.get(), _v.physicalDevice));

  _vBuffers[0] = _vertBuf->handle();

  std::array<VkVertexInputBindingDescription, 1> bindingDescription{
      _vertBuf->binding()};

  std::array<VkVertexInputAttributeDescription, 1> attrDescription{
      _vertBuf->attributes()[0]};

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                    VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;            // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

  vulkan::GraphicsPipelineBuilder gPipBuilder =
      vulkan::GraphicsPipelineBuilder(_v.device.get(), _v.renderPass.get());

  gPipBuilder.setExtent(_window.getDims())
      ->setBindings(bindingDescription, attrDescription)
      ->setAssemblyTopo(inputAssembly)
      ->setDynamicStates(dynamicStates)
      ->setPipelineLayout(pipelineLayoutInfo);

  gPipBuilder.build(&_v._vertShad, &_v._fragShad, &_v._pipelineLay,
                    &_v._gPipeline);
}

Renderer::Renderer(const window::Window &window)
    : _currentFrameIndex(0), _window(window) {

  createInstance(&_v.instance, &_khr.debugMessenger);

  window.createWindowSurface(_v.instance.get(), &_khr.surface);

  uint32_t deviceCount = 0;

  vkEnumeratePhysicalDevices(_v.instance.get(), &deviceCount, 0);

  if (deviceCount == 0) {
    THROW_EXCEPTION("No physicalDevices with vulkan support!");
  }

  std::unique_ptr<VkPhysicalDevice[]> physicalDevices =
      std::make_unique<VkPhysicalDevice[]>(deviceCount);

  vkEnumeratePhysicalDevices(_v.instance.get(), &deviceCount,
                             physicalDevices.get());

  for (int i = 0; i < deviceCount; i++) {
    if (isDeviceSuitable(physicalDevices.get()[i], _khr.surface.get(),
                         _capabilities)) {
      _v.physicalDevice = physicalDevices[i];
      break;
    }
  }

  if (_v.physicalDevice == VK_NULL_HANDLE) {
    THROW_EXCEPTION("failed to find a suitable GPU!");
  }
  createLogicalDevice(_v.physicalDevice, &_capabilities, _v.device);
  vulkan::Deleter::Init(_v.instance.get(), _v.device.get());

  // After setup
  _swapchain.reset(new engine::Swapchain(_window, _capabilities,
                                         _v.device.get(), _khr.surface.get()));

  _createRenderPasses();
  _createSwapchainFramebufs(_swapchain->dims());

  _createGraphicsPipeline();

  _createCommandPool();

  _createCommandBuffers();

  _createSyncObjects();

  // Get queues
  vkGetDeviceQueue(_v.device.get(), _capabilities.indices.graphicsFamily, 0,
                   &_graphicsQueue);
};

void Renderer::poll() {
  VkFence waitFences[1]{_v.inFlightFenses[_currentFrameIndex]};

  vkWaitForFences(_v.device.get(), 1, waitFences, VK_TRUE, UINT64_MAX);
  vkResetFences(_v.device.get(), 1, waitFences);
}

void Renderer::beginFrame(window::WindowDims dims) {

  vkResetCommandBuffer(_v.cmdBufs[_currentFrameIndex], 0);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;
  if (vkBeginCommandBuffer(_v.cmdBufs[_currentFrameIndex], &beginInfo) !=
      VK_SUCCESS) {
    THROW_EXCEPTION("failed to begin recording command buffer!");
  }

  _v.currCmdBuf = _v.cmdBufs[_currentFrameIndex];

  if (_swapchain->rebuildRequired() || _swapchain->dims() != dims) {
    vkDeviceWaitIdle(_v.device.get());
    _destroySwapchainFramebufs();
    _swapchain->rebuild(dims);
    _createSwapchainFramebufs(dims);
  }

  uint32_t imgIndx =
      _swapchain->acquireImage(_v.imgAvailableSems[_currentFrameIndex]);
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = _v.renderPass.get();
  renderPassInfo.framebuffer = _v.swapchainFramebufs[imgIndx];

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = _swapchain->dims();

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  // VkBufferMemoryBarrier bufBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
  // bufBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
  // bufBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
  // bufBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  // bufBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  // bufBarrier.buffer = _vertBuf.get()->handle();
  // bufBarrier.offset = 396 * sizeof(vulkan::Vertex);
  // bufBarrier.size = 4 * sizeof(vulkan::Vertex);

  // vkCmdPipelineBarrier(_v.cmdBufs[_currentFrameIndex],
  //                      VK_PIPELINE_STAGE_HOST_BIT,
  //                      VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1,
  //                      &bufBarrier, 0, nullptr);

  vkCmdBeginRenderPass(_v.cmdBufs[_currentFrameIndex], &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(_v.currCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    _v._gPipeline.get());

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(_swapchain->dims().width);
  viewport.height = static_cast<float>(_swapchain->dims().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(_v.currCmdBuf, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = _swapchain->dims();
  vkCmdSetScissor(_v.currCmdBuf, 0, 1, &scissor);

  // Will be moved outside

  vkCmdBindVertexBuffers(_v.currCmdBuf, 0, _vertexBuffCount, _vBuffers,
                         _offsets);
};
void Renderer::render(uint32_t firstV, uint32_t vCount) {
  // vkCmdDraw(_v.currCmdBuf, 4, 1, vulkan::offset, 0);
  LOG("Drawing at: %d", firstV);
  vkCmdDraw(_v.currCmdBuf, vCount, 1, firstV, 0);
};

void Renderer::endFrame() {
  vkCmdEndRenderPass(_v.currCmdBuf);

  if (vkEndCommandBuffer(_v.cmdBufs[_currentFrameIndex]) != VK_SUCCESS) {
    THROW_EXCEPTION("failed to record command buffer!");
  }

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSems[1]{_v.imgAvailableSems[_currentFrameIndex]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSems;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &_v.cmdBufs[_currentFrameIndex];

  VkSemaphore signalSems[1]{_v.renderFiniSems[_currentFrameIndex]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSems;

  if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo,
                    _v.inFlightFenses[_currentFrameIndex]) != VK_SUCCESS) {
    THROW_EXCEPTION("failed to submit draw command buffer!");
  }

  _swapchain->present(_v.renderFiniSems[_currentFrameIndex]);

  _currentFrameIndex = (_currentFrameIndex + 1) % FramesInFlight;

  _v.currCmdBuf = nullptr;
};

RectangleVBuff &Renderer::getBuffer() { return *_vertBuf.get(); };

Renderer::~Renderer() { vkDeviceWaitIdle(_v.device.get()); }
