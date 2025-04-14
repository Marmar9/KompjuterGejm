#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <inc/common/exception.hpp>
#include <span>
#include <vulkan/vulkan_core.h>

#include "include/vulkan/handle.hpp"

namespace engine::vulkan {

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);

template <typename T> class Buffer {
  size_t _sz;
  VkDevice _dev;
  engine::vulkan::Handle<VkDeviceMemory> _mem;
  engine::vulkan::Handle<VkBuffer> _buf;

public:
  Buffer() = default;
  ~Buffer() = default;

  void create(size_t sz, VkDevice dev, VkPhysicalDevice physicalDevice) {
    _sz = sz;
    _dev = dev;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(T) * _sz;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(dev, &bufferInfo, nullptr, &_buf) != VK_SUCCESS) {
      THROW_EXCEPTION("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_dev, _buf.get(), &memRequirements);

    VkMemoryAllocateInfo memAllocInfo{};

    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memRequirements.size;
    memAllocInfo.memoryTypeIndex =
        findMemoryType(physicalDevice, memRequirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    ;

    if (vkAllocateMemory(dev, &memAllocInfo, nullptr, &_mem) != VK_SUCCESS) {
      THROW_EXCEPTION("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(dev, _buf.get(), _mem.get(), 0);
  }

  VkBuffer get() { return _buf.get(); }

  void setData(std::span<T> data) {
    if (data.size() != _sz) {
      THROW_EXCEPTION("Data size is difrent than buffer size");
    }

    void *mMem;
    vkMapMemory(_dev, _mem.get(), 0, _sz, 0, &mMem);
    std::memcpy(mMem, data.data(), sizeof(T) * _sz);
    vkUnmapMemory(_dev, _mem.get());
  }

  VkVertexInputBindingDescription getBindingDesc(uint32_t binding,
                                                 VkVertexInputRate inputRate) {
    VkVertexInputBindingDescription bindingDesc;
    bindingDesc.binding = binding;
    bindingDesc.stride = sizeof(T);
    bindingDesc.inputRate = inputRate;
    return bindingDesc;
  }
};
} // namespace engine::vulkan
