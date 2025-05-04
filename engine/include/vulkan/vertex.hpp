#pragma once
#include <array>
#include <concepts>
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

struct MyVertex {
  float pos[2];

  static constexpr VkVertexInputBindingDescription describeBinding() {
    return {/* .binding   = */ 0,
            /* .stride    = */ sizeof(MyVertex),
            /* .inputRate = */ VK_VERTEX_INPUT_RATE_VERTEX};
  }

  static constexpr auto describeAttributes() {
    static constexpr std::array<VkVertexInputAttributeDescription, 2> descs = {{
        {/* .location = */ 0,
         /* .binding  = */ 0,
         /* .format   = */ VK_FORMAT_R32G32_SFLOAT,
         /* .offset   = */ offsetof(MyVertex, pos)},
    }};

    return std::span{descs};
  }
};
} // namespace engine::vulkan
