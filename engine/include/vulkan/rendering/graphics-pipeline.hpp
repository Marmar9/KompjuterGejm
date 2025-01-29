#pragma once

#include "inc/common/loger.h"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/rendering/graphics-pipeline-builder.hpp"
namespace vulkan {
class GraphicsPipeline {

  vulkan::Handle<VkShaderModule> _vertShad;
  vulkan::Handle<VkShaderModule> _fragShad;
  vulkan::Handle<VkPipelineLayout> _pipelineLay;
  vulkan::Handle<VkPipeline> _pipeline;

public:
  GraphicsPipeline(const vulkan::GraphicsPipelineBuilder &builder) {
    builder.build(&_vertShad, &_fragShad, &_pipelineLay, &_pipeline);
  };
  ~GraphicsPipeline(){LOG("Destroyed pipeline")}

  VkPipeline get() {
    return _pipeline.get();
  }

  GraphicsPipeline(const GraphicsPipeline &) = delete;
  GraphicsPipeline(const GraphicsPipeline &&) = delete;
};
} // namespace vulkan
