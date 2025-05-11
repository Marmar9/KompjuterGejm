#include "include/vulkan/rendering/graphics-pipeline-builder.hpp"
#include "cstdlib"
#include "inc/common/exception.hpp"
#include "inc/common/loger.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#define SHADER_INCLUDE(fileName)                                               \
  extern "C" {                                                                 \
  extern uint8_t _binary_##fileName##_o_spv_end;                               \
  extern uint8_t _binary_##fileName##_o_spv_start;                             \
  }

#define SHADER_GET_CODE(fileName) &_binary_##fileName##_o_spv_start

#define SHADER_GET_SZ(fileName)                                                \
  &_binary_##fileName##_o_spv_end - &_binary_##fileName##_o_spv_start

SHADER_INCLUDE(triangle_frag)
SHADER_INCLUDE(triangle_vert)

VkShaderModule engine::vulkan::createShaderModule(VkDevice device,
                                                  std::span<uint32_t> code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<uint32_t *>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    THROW_EXCEPTION("failed to create shader module!");
  }
  return shaderModule;
}

namespace engine::vulkan {

GraphicsPipelineBuilder::GraphicsPipelineBuilder(VkDevice device,
                                                 VkRenderPass renderPass)
    : _device(device), _renderPass(renderPass), _structures{} {}

GraphicsPipelineBuilder *GraphicsPipelineBuilder::setPipelineLayout(
    const VkPipelineLayoutCreateInfo &layInfo) {
  _structures.pipelineLayoutInfo = layInfo;
  return this;
}

GraphicsPipelineBuilder *GraphicsPipelineBuilder::setDynamicStates(
    std::span<VkDynamicState> dynamicStates) {
  _structures.dynamicState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  _structures.dynamicState.dynamicStateCount = dynamicStates.size();
  _structures.dynamicState.pDynamicStates = dynamicStates.data();
  return this;
}

GraphicsPipelineBuilder *GraphicsPipelineBuilder::setBindings(
    std::span<VkVertexInputBindingDescription> bindDesc,
    std::span<VkVertexInputAttributeDescription> attrDesc) {
  _structures.vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  _structures.vertexInputInfo.vertexBindingDescriptionCount = bindDesc.size();
  _structures.vertexInputInfo.pVertexBindingDescriptions = bindDesc.data();
  _structures.vertexInputInfo.vertexAttributeDescriptionCount = attrDesc.size();
  _structures.vertexInputInfo.pVertexAttributeDescriptions = attrDesc.data();
  return this;
}

GraphicsPipelineBuilder *GraphicsPipelineBuilder::setExtent(VkExtent2D extent) {
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  _structures.viewportState.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  _structures.viewportState.viewportCount = 1;
  _structures.viewportState.scissorCount = 1;
  _structures.viewportState.pViewports = &viewport;
  _structures.viewportState.pScissors = &scissor;

  return this;
}

GraphicsPipelineBuilder *GraphicsPipelineBuilder::setAssemblyTopo(
    VkPipelineInputAssemblyStateCreateInfo inputAssembly) {
  _structures.inputAssembly = inputAssembly;
  return this;
}

void GraphicsPipelineBuilder::build(VkShaderModule *vertShad,
                                    VkShaderModule *fragShad,
                                    VkPipelineLayout *pipelineLayout,
                                    VkPipeline *pipeline) const {

  if (vkCreatePipelineLayout(_device, &_structures.pipelineLayoutInfo, nullptr,
                             pipelineLayout) != VK_SUCCESS) {
    THROW_EXCEPTION("failed to create pipeline layout!");
  }

  *vertShad = createShaderModule(
      _device, std::span((uint32_t *)SHADER_GET_CODE(triangle_vert),
                         SHADER_GET_SZ(triangle_vert)));

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = *vertShad;
  vertShaderStageInfo.pName = "main";

  *fragShad = createShaderModule(
      _device, std::span((uint32_t *)SHADER_GET_CODE(triangle_frag),
                         SHADER_GET_SZ(triangle_frag)));

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = *fragShad;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  // rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.cullMode = VK_CULL_MODE_NONE;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &_structures.vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &_structures.inputAssembly;
  pipelineInfo.pViewportState = &_structures.viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &_structures.dynamicState;
  pipelineInfo.layout = *pipelineLayout;
  pipelineInfo.renderPass = _renderPass;
  LOG("--------------------- Before --------------------");

  if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, pipeline) != VK_SUCCESS) {
    THROW_EXCEPTION("failed to create graphics pipeline!");
  }

  LOG("--------------------- After --------------------");
}
} // namespace engine::vulkan
