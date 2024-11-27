#include "include/vulkan/handle.hpp"
#include "include/utils/loger.h"
#include <vulkan/vulkan_core.h>

using namespace vulkan;

VkInstance Deleter::_instance = nullptr;
VkDevice Deleter::_device = nullptr;

void Deleter::operator()(VkInstance resource) noexcept {
  vkDestroyInstance(resource, nullptr);
};

void Deleter::operator()(VkDevice resource) noexcept {
  vkDestroyDevice(resource, nullptr);
};

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  PFN_vkDestroyDebugUtilsMessengerEXT func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != 0) {
    func(instance, debugMessenger, pAllocator);
  }
}

void Deleter::operator()(VkDebugUtilsMessengerEXT resource) noexcept {
  DestroyDebugUtilsMessengerEXT(_instance, resource, nullptr);
};

void Deleter::operator()(VkSwapchainKHR resource) noexcept {
  vkDestroySwapchainKHR(_device, resource, nullptr);
};

void Deleter::operator()(VkDeviceMemory resource) noexcept {
  vkFreeMemory(_device, resource, nullptr);
};

void Deleter::operator()(VkBuffer resource) noexcept {
  vkDestroyBuffer(_device, resource, nullptr);
};
void Deleter::operator()(VkSurfaceKHR resource) noexcept {
  vkDestroySurfaceKHR(_instance, resource, nullptr);
};
void Deleter::operator()(VkSemaphore resource) noexcept {
  vkDestroySemaphore(_device, resource, nullptr);
};

void Deleter::operator()(VkFence resource) noexcept {
  vkDestroyFence(_device, resource, nullptr);
};
void Deleter::operator()(VkPipelineLayout resource) noexcept {
  vkDestroyPipelineLayout(_device, resource, nullptr);
};

void Deleter::operator()(VkRenderPass resource) noexcept {
  vkDestroyRenderPass(_device, resource, nullptr);
};
void Deleter::operator()(VkPipeline resource) noexcept {
  vkDestroyPipeline(_device, resource, nullptr);
};

void Deleter::operator()(VkShaderModule resource) noexcept {
  vkDestroyShaderModule(_device, resource, nullptr);
};
void Deleter::operator()(VkCommandPool resource) noexcept {
  vkDestroyCommandPool(_device, resource, nullptr);
};

void Deleter::operator()(VkImageView resource) noexcept {
  vkDestroyImageView(_device, resource, nullptr);
};

void Deleter::operator()(VkFramebuffer resource) noexcept {
  vkDestroyFramebuffer(_device, resource, nullptr);
};
