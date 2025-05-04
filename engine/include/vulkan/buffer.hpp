#pragma once

#include "include/vulkan/handle.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <inc/common/exception.hpp>
#include <include/vulkan/vertex.hpp>
#include <vulkan/vulkan_core.h>
namespace engine::vulkan {

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);

template <CVertex V> class VertexBuffer {
  VkDevice _dev;
  engine::vulkan::Handle<VkBuffer> _buf;
  engine::vulkan::Handle<VkDeviceMemory> _mem;
  size_t _count;
  std::span<V> _data;

public:
  VertexBuffer(VkDevice dev, VkPhysicalDevice phys, std::size_t count)
      : _dev(dev), _count(count) {
    VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bi.size = sizeof(V) * _count;
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
    vkMapMemory(_dev, _mem.get(), 0, _count * sizeof(V), 0, &mMem);

    _data = std::span<V>(reinterpret_cast<V *>(mMem), _count);
  }

  const std::span<V> &data() const noexcept { return _data; }

  VkBuffer handle() const noexcept { return _buf.get(); }
  size_t count() const noexcept { return _count; }
  static constexpr auto binding() { return V::describeBinding(); }
  static constexpr auto attributes() { return V::describeAttributes(); }

  ~VertexBuffer() { vkUnmapMemory(_dev, _mem.get()); }
};

} // namespace engine::vulkan
