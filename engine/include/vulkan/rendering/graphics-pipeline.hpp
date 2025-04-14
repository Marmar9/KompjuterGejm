#pragma once

#include "inc/common/loger.h"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/rendering/graphics-pipeline-builder.hpp"
namespace engine::vulkan {
class GraphicsPipeline {

  engine::vulkan::Handle<VkShaderModule> _vertShad;
  engine::vulkan::Handle<VkShaderModule> _fragShad;
  engine::vulkan::Handle<VkPipelineLayout> _pipelineLay;
  engine::vulkan::Handle<VkPipeline> _pipeline;

public:
  GraphicsPipeline(const engine::vulkan::GraphicsPipelineBuilder &builder) {
    builder.build(&_vertShad, &_fragShad, &_pipelineLay, &_pipeline);
  };
  ~GraphicsPipeline(){LOG("Destroyed pipeline")}

  VkPipeline get() {
    return _pipeline.get();
  }

  GraphicsPipeline(const GraphicsPipeline &) = delete;
  GraphicsPipeline(const GraphicsPipeline &&) = delete;
};
} // namespace engine::vulkan
