#pragma once

#include "inc/common/loger.h"
#include "include/vulkan/handle.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <inc/common/exception.hpp>
#include <include/vulkan/vertex.hpp>
#include <vulkan/vulkan_core.h>
namespace engine::vulkan {

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);

// constexpr uint32_t offset = 394;
template <CVertex V, size_t Count> class VertexBuffer {
  VkDevice _dev;
  engine::vulkan::Handle<VkBuffer> _buf;
  engine::vulkan::Handle<VkDeviceMemory> _mem;
  std::span<V> _data;

public:
  VertexBuffer(VkDevice dev, VkPhysicalDevice phys) : _dev(dev) {
    VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    LOG("Creating vertex buffer");
    bi.size = sizeof(V) * Count;
    bi.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vkCreateBuffer(dev, &bi, nullptr, &_buf);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_dev, _buf.get(), &memRequirements);

    VkMemoryAllocateInfo memAllocInfo{};

    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memRequirements.size;
    memAllocInfo.memoryTypeIndex =
        findMemoryType(phys, memRequirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    ;

    if (vkAllocateMemory(dev, &memAllocInfo, nullptr, &_mem) != VK_SUCCESS) {
      THROW_EXCEPTION("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(dev, _buf.get(), _mem.get(), 0);

    void *mMem;
    vkMapMemory(_dev, _mem.get(), 0, Count * sizeof(V), 0, &mMem);

    _data = std::span<V>(reinterpret_cast<V *>(mMem), Count);

    // Remains broken
    // vulkan::Vertex verticies[4] = {{-0.5f, 0.0f},   // 0
    //                                {0.5f, 0.0f},    // 1
    //                                {-0.5f, -0.25f}, // 2
    //                                {0.5f, -0.25f}};

    // memcpy(_data.data() + offset, verticies, sizeof(vulkan::Vertex) * 4);

    // printf("%p----------", &_data[393]);
  }

  const std::span<V> &data() const noexcept { return _data; }

  VkBuffer &handle() noexcept { return _buf.get(); }
  static constexpr size_t count() noexcept { return Count; }
  static constexpr auto binding() { return V::describeBinding(); }
  static constexpr auto attributes() { return V::describeAttributes(); }

  ~VertexBuffer() { vkUnmapMemory(_dev, _mem.get()); }
};

template <typename U, size_t Count> class UniformBuffer {
  VkDevice _dev;
  engine::vulkan::Handle<VkBuffer> _buf;
  engine::vulkan::Handle<VkDeviceMemory> _mem;
  std::span<U> _data;

public:
  UniformBuffer(VkDevice dev, VkPhysicalDevice phys) : _dev(dev) {
    VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bi.size = sizeof(U) * Count;
    bi.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    vkCreateBuffer(dev, &bi, nullptr, &_buf);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_dev, _buf.get(), &memRequirements);

    VkMemoryAllocateInfo memAllocInfo{};

    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memRequirements.size;
    memAllocInfo.memoryTypeIndex =
        findMemoryType(phys, memRequirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    ;

    if (vkAllocateMemory(dev, &memAllocInfo, nullptr, &_mem) != VK_SUCCESS) {
      THROW_EXCEPTION("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(dev, _buf.get(), _mem.get(), 0);

    void *mMem;
    vkMapMemory(_dev, _mem.get(), 0, Count * sizeof(U), 0, &mMem);

    _data = std::span<U>(reinterpret_cast<U *>(mMem), Count);
  }

  const std::span<U> &data() const noexcept { return _data; }

  VkBuffer handle() noexcept { return _buf.get(); }
  static constexpr size_t count() noexcept { return Count; }
  static constexpr auto binding() { return U::describeBinding(); }
  static constexpr auto attributes() { return U::describeAttributes(); }

  ~UniformBuffer() { vkUnmapMemory(_dev, _mem.get()); }
};

} // namespace engine::vulkan
