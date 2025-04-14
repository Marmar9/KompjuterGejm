#include "include/vulkan/buffer.hpp"

uint32_t engine::vulkan::findMemoryType(VkPhysicalDevice physicalDevice,
                                        uint32_t typeFilter,
                                        VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  /*LOG("Type filter: %d", typeFilter)*/
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    /*LOG("TypeIndex: %d, HeapIndex: %d, PropertyFlags: %d", i,*/
    /*    memProperties.memoryTypes[i].heapIndex,*/
    /*    memProperties.memoryTypes[i].propertyFlags)*/
  }

  for (uint32_t i = 0; i < memProperties.memoryHeapCount; i++) {
    /*LOG("HeapIndex: %d, Sz: %lu, HeapFlags: %d", i,*/
    /*    memProperties.memoryHeaps[i].size,
     * memProperties.memoryHeaps[i].flags)*/
  }

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags &
                                  properties) == properties) {
      // LOG("Chosen: %d", i)
      return i;
    }
  }
  THROW_EXCEPTION("Failed to find suitable memory type ");
}
