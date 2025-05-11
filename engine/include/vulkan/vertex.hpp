#pragma once
#include <array>
#include <concepts>
#include <cstdint>
#include <span>
#include <vulkan/vulkan.h>
namespace engine::vulkan {
template <typename T>
concept CVertex = requires {
  { T::describeBinding() } -> std::same_as<VkVertexInputBindingDescription>;
  {
    T::describeAttributes()
  } -> std::convertible_to<std::span<const VkVertexInputAttributeDescription>>;
};
struct Vertex {
  float posX;
  float posY;

  static constexpr VkVertexInputBindingDescription describeBinding() {
    return {/* .binding   = */ 0,
            /* .stride    = */ sizeof(Vertex),
            /* .inputRate = */ VK_VERTEX_INPUT_RATE_VERTEX};
  }

  static constexpr auto describeAttributes() {
    static constexpr std::array<VkVertexInputAttributeDescription, 1> descs = {{
        {/* .location = */ 0,
         /* .binding  = */ 0,
         /* .format   = */ VK_FORMAT_R32G32_SFLOAT,
         /* .offset   = */ 0},
    }};

    return std::span{descs};
  }
};

struct InstanceData {
  static constexpr uint32_t VertCount = 4;
  Vertex data[VertCount];
};
} // namespace engine::vulkan
