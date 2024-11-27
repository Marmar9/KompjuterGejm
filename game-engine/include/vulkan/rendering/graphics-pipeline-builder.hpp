// #pragma once
// #include "utils/exeption.hpp"
// #include "vulkan/rendering/pipeline-builder.hpp"
// #include <memory>
// #include <vulkan/vulkan_core.h>
//
// namespace renderer {
// class GraphicsPipelineBuilder : PipelineBuilder {
//   std::unique_ptr<VkPipeline_T> graphicsPipeline;
//
//   struct {
//     const VkPipelineShaderStageCreateInfo &vertShaderStageInfo;
//     const VkPipelineShaderStageCreateInfo &fragShaderStageInfo;
//
//     const VkPipelineDynamicStateCreateInfo &dynamicState;
//
//     const VkPipelineVertexInputStateCreateInfo &vertexInputInfo;
//
//     const VkPipelineInputAssemblyStateCreateInfo &inputAssembly;
//
//     const VkPipelineViewportStateCreateInfo &viewportState;
//   } pConfig;
//
// public:
//   GraphicsPipelineBuilder() {
//
//     gPipelineWrap.vertShader =
//         createShaderModule(_device, loadShader(vertShadName));
//     gPipelineWrap.fragShader =
//         createShaderModule(_device, loadShader(fragShadName));
//
//     vertShaderStageInfo.sType =
//         VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//     vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//     vertShaderStageInfo.module = gPipelineWrap.vertShader;
//     vertShaderStageInfo.pName = "main";
//
//     fragShaderStageInfo.sType =
//         VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//     fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//     fragShaderStageInfo.module = gPipelineWrap.fragShader;
//     fragShaderStageInfo.pName = "main";
//
//     VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
//                                                       fragShaderStageInfo};
//     // Dynamic states
//     VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
//                                       VK_DYNAMIC_STATE_SCISSOR};
//
//     VkPipelineDynamicStateCreateInfo dynamicState{};
//     dynamicState.sType =
//     VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//     dynamicState.dynamicStateCount = STATIC_ARR_LEN(dynamicStates);
//     dynamicState.pDynamicStates = dynamicStates;
//     // Vertex buffer
//
//     // One buffer for now
//
//     VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//     vertexInputInfo.sType =
//         VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//     vertexInputInfo.vertexBindingDescriptionCount =
//     bindingDescription.size(); vertexInputInfo.pVertexBindingDescriptions =
//     bindingDescription.data();
//     vertexInputInfo.vertexAttributeDescriptionCount =
//         atributeDescription.size();
//     vertexInputInfo.pVertexAttributeDescriptions =
//     atributeDescription.data();
//
//     // Assembly topology
//
//     VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
//     inputAssembly.sType =
//         VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//     inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//     inputAssembly.primitiveRestartEnable = VK_FALSE;
//
//     // Vieports and scisors
//
//     VkViewport viewport{};
//     viewport.x = 0.0f;
//     viewport.y = 0.0f;
//     viewport.width = (float)extent.width;
//     viewport.height = (float)extent.height;
//     viewport.minDepth = 0.0f;
//     viewport.maxDepth = 1.0f;
//
//     VkRect2D scissor{};
//     scissor.offset = {0, 0};
//     scissor.extent = extent;
//
//     VkPipelineViewportStateCreateInfo viewportState{};
//     viewportState.sType =
//     VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//     viewportState.viewportCount = 1;
//     viewportState.scissorCount = 1;
//     viewportState.pViewports = &viewport;
//     viewportState.pScissors = &scissor;
//
//     // Rasterizer
//
//     VkPipelineRasterizationStateCreateInfo rasterizer{};
//     rasterizer.sType =
//         VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//     rasterizer.depthClampEnable = VK_FALSE;
//     rasterizer.rasterizerDiscardEnable = VK_FALSE;
//     rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//     rasterizer.lineWidth = 1.0f;
//     rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//     rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
//     rasterizer.depthBiasEnable = VK_FALSE;
//     rasterizer.depthBiasConstantFactor = 0.0f; // Optional
//     rasterizer.depthBiasClamp = 0.0f;          // Optional
//     rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional
//
//     // Multisampling
//
//     VkPipelineMultisampleStateCreateInfo multisampling{};
//     multisampling.sType =
//         VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//     multisampling.sampleShadingEnable = VK_FALSE;
//     multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//     multisampling.minSampleShading = 1.0f;          // Optional
//     multisampling.pSampleMask = nullptr;            // Optional
//     multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
//     multisampling.alphaToOneEnable = VK_FALSE;      // Optional
//                                                     //
//     // Color blend
//
//     VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//     colorBlendAttachment.colorWriteMask =
//         VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
//         VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//     colorBlendAttachment.blendEnable = VK_FALSE;
//
//     VkPipelineColorBlendStateCreateInfo colorBlending{};
//     colorBlending.sType =
//         VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//     colorBlending.logicOpEnable = VK_FALSE;
//     colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
//     colorBlending.attachmentCount = 1;
//     colorBlending.pAttachments = &colorBlendAttachment;
//
//     // Pipeline layout
//
//     // Creating a pipelnie finaly
//
//     VkGraphicsPipelineCreateInfo pipelineInfo{};
//     pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//     pipelineInfo.stageCount = 2;
//     pipelineInfo.pStages = shaderStages;
//
//     pipelineInfo.pVertexInputState = &vertexInputInfo;
//     pipelineInfo.pInputAssemblyState = &inputAssembly;
//     pipelineInfo.pViewportState = &viewportState;
//     pipelineInfo.pRasterizationState = &rasterizer;
//     pipelineInfo.pMultisampleState = &multisampling;
//     pipelineInfo.pDepthStencilState = nullptr; // Optional
//     pipelineInfo.pColorBlendState = &colorBlending;
//     pipelineInfo.pDynamicState = &dynamicState;
//
//     pipelineInfo.layout = pipelineLayout;
//
//     pipelineInfo.renderPass = renderPass;
//     pipelineInfo.subpass = 0;
//
//     pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
//     pipelineInfo.basePipelineIndex = -1;              // Optional
//
//     if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo,
//                                   nullptr,
//                                   &gPipelineWrap.pipeline) != VK_SUCCESS) {
//       THROW_EXEPTION("failed to create graphics pipeline!");
//     }
//   }
// };
// } // namespace renderer
