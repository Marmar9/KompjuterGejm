#pragma once
#include "include/vulkan/builder.hpp"
#include <span>
#include <vulkan/vulkan_core.h>

namespace engine::vulkan {

std::span<uint32_t> loadShader(const char *filename);

VkShaderModule createShaderModule(VkDevice device, std::span<uint32_t> code);

std::span<uint32_t> loadShader(const char *filename);
VkShaderModule createShaderModule(VkDevice device, std::span<uint32_t> code);

class GraphicsPipelineBuilder
    : engine::vulkan::Buildable<VkShaderModule, VkShaderModule,
                                VkPipelineLayout, VkPipeline> {
  VkDevice _device;
  VkRenderPass _renderPass;
  struct {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    VkPipelineViewportStateCreateInfo viewportState{};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  } _structures;

public:
  GraphicsPipelineBuilder(VkDevice device, VkRenderPass renderPass);
  GraphicsPipelineBuilder *
  setPipelineLayout(const VkPipelineLayoutCreateInfo &layInfo);
  GraphicsPipelineBuilder *
  setDynamicStates(std::span<VkDynamicState> dynamicStates);
  GraphicsPipelineBuilder *
  setBindings(std::span<VkVertexInputBindingDescription> bindDesc,
              std::span<VkVertexInputAttributeDescription> attrDesc);
  GraphicsPipelineBuilder *setExtent(VkExtent2D extent);
  GraphicsPipelineBuilder *
  setAssemblyTopo(VkPipelineInputAssemblyStateCreateInfo inputAssembly);
  void build(VkShaderModule *vertShad, VkShaderModule *fragShad,
             VkPipelineLayout *pipelineLayout,
             VkPipeline *pipeline) const override;
};

} // namespace engine::vulkan
