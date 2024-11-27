#pragma once
#include <vulkan/vulkan_core.h>

namespace renderer {
class PipelineBuilder {
public:
  virtual ~PipelineBuilder() = default;
};
} // namespace renderer
