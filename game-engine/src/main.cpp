#include "include/game-engine.hpp"
#include "include/utils/exeption.hpp"
#include "include/utils/loger.h"
#include "include/utils/static-arr-len.hpp"
#include "include/vulkan/handle.hpp"
#include "include/vulkan/swapchain.hpp"
#include "include/window/window-dims.hpp"
#include "include/window/window.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <glm/ext/vector_float2.hpp>
#include <span>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

std::span<uint32_t> loadShader(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    THROW_EXEPTION("Failed to open the file");
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  uint32_t *buffer = nullptr;

  size_t aligned_size = (file_size + 3) & ~static_cast<size_t>(3);

  buffer = reinterpret_cast<uint32_t *>(
      aligned_alloc(sizeof(uint32_t), aligned_size));

  if (buffer == 0) {
    fclose(file);
    THROW_EXEPTION("Failed to allocate aligned memory");
  }

  size_t read_size = fread(buffer, 1, file_size, file);
  if (read_size != file_size) {
    free(buffer);
    fclose(file);
    THROW_EXEPTION("Failed to read the entire file");
  }

  fclose(file);

  return std::span<uint32_t>(buffer, file_size);
}

VkShaderModule createShaderModule(VkDevice device, std::span<uint32_t> code) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<uint32_t *>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }
  return shaderModule;
}

void createRenderPasses(VkDevice device, VkSurfaceFormatKHR const &format,
                        VkRenderPass *renderPass) {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = format.format;

  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  // VkSubpassDependency dependency{};
  // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  // dependency.dstSubpass = 0;

  // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // dependency.srcAccessMask = 0;

  // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  // renderPassInfo.dependencyCount = 1;
  // renderPassInfo.pDependencies = &dependency;
  renderPassInfo.dependencyCount = 0;
  renderPassInfo.pDependencies = nullptr;

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, renderPass) !=
      VK_SUCCESS) {
    THROW_EXEPTION("failed to create render pass!");
  }
}

template <typename T>
VkBuffer createVertexBuffer(VkDevice device, VkDeviceSize sz) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(T) * sz;
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  VkBuffer vertexBuffer;
  if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) !=
      VK_SUCCESS) {
    THROW_EXEPTION("failed to create vertex buffer!");
  }

  return vertexBuffer;
}

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  // for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
  //   LOG("TypeIndex: %d, HeapIndex: %d, PropertyFlags: %d", i,
  //       memProperties.memoryTypes[i].heapIndex,
  //       memProperties.memoryTypes[i].propertyFlags)
  // }

  // for (uint32_t i = 0; i < memProperties.memoryHeapCount; i++) {
  //   LOG("HeapIndex: %d, Sz: %lu, HeapFlags: %d", i,
  //       memProperties.memoryHeaps[i].size,
  //       memProperties.memoryHeaps[i].flags)
  // }

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if (typeFilter & (1 << i) &&
        (memProperties.memoryTypes[i].propertyFlags & properties)) {
      return i;
    }
  }
  THROW_EXEPTION("Failed to find suitable memory type ");
}

re::GameEngine::GameEngine() {
  _window.reset(window::createWindow());
  _renderer.reset(new renderer::Renderer(*_window));
  vulkan::Deleter::Init(_renderer->getInstance(), _renderer->getDevice());
  _swapchain.reset(
      new renderer::Swapchain(_window.get(), _renderer->getDevCaps(),
                              _renderer->getDevice(), _renderer->getSurface()));

  _renderer->init(_swapchain.get());

  const size_t posLen = 3;
  glm::vec2 pos[posLen] = {
      glm::vec2{0.0f, -0.5f},
      glm::vec2{0.5f, 0.5f},
      glm::vec2{-0.5f, 0.5f},
  };

  _vertBuff.reset(
      createVertexBuffer<glm::vec2>(_renderer->getDevice(), posLen));

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(_renderer->getDevice(), _vertBuff.get(),
                                &memRequirements);

  VkMemoryAllocateInfo memAllocInfo{};

  memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memAllocInfo.allocationSize = memRequirements.size;
  memAllocInfo.memoryTypeIndex = findMemoryType(
      _renderer->getPhysicalDevice(), memRequirements.memoryTypeBits,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(_renderer->getDevice(), &memAllocInfo, nullptr,
                       &_devMem) != VK_SUCCESS) {
    THROW_EXEPTION("failed to allocate vertex buffer memory!");
  }

  vkBindBufferMemory(_renderer->getDevice(), _vertBuff.get(), _devMem.get(), 0);

  void *data;
  vkMapMemory(_renderer->getDevice(), _devMem.get(), 0, posLen, 0, &data);
  memcpy(data, pos, sizeof(glm::vec2) * posLen);
  vkUnmapMemory(_renderer->getDevice(), _devMem.get());

  std::array<VkVertexInputBindingDescription, 1> bindingDescription{};
  bindingDescription[0].binding = 0;
  bindingDescription[0].stride = sizeof(glm::vec2);
  bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::array<VkVertexInputAttributeDescription, 1> attrDescription{};
  attrDescription[0].binding = 0;
  attrDescription[0].location = 0;
  attrDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
  attrDescription[0].offset = 0;

  createRenderPasses(_renderer->getDevice(), _swapchain->getFormat(),
                     &_renderPass);

  _renderer->createSwapchainFramebufs(_swapchain->dims(), _renderPass.get());

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;            // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

  if (vkCreatePipelineLayout(_renderer->getDevice(), &pipelineLayoutInfo,
                             nullptr, &_pipelineLayout) != VK_SUCCESS) {
    THROW_EXEPTION("failed to create pipeline layout!");
  }

  _vertShad.reset(
      createShaderModule(_renderer->getDevice(),
                         loadShader("game-engine/build/triangle.vert.spv")));
  _fragShad.reset(
      createShaderModule(_renderer->getDevice(),
                         loadShader("game-engine/build/triangle.frag.spv")));

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};

  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = _vertShad.get();
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};

  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = _fragShad.get();
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};
  // Dynamic states
  VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                    VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = STATIC_ARR_LEN(dynamicStates);
  dynamicState.pDynamicStates = dynamicStates;
  // Vertex buffer

  // One buffer for now

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = bindingDescription.size();
  vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
  vertexInputInfo.vertexAttributeDescriptionCount = attrDescription.size();
  vertexInputInfo.pVertexAttributeDescriptions = attrDescription.data();

  // Assembly topology

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // Vieports and scisors

  VkExtent2D extent = _swapchain->dims();

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.pScissors = &scissor;

  // Rasterizer

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f;          // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

  // Multisampling

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;          // Optional
  multisampling.pSampleMask = nullptr;            // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE;      // Optional
                                                  //
  // Color blend

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;

  // Pipeline layout

  // Creating a pipelnie finaly

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;

  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = nullptr; // Optional
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;

  pipelineInfo.layout = _pipelineLayout.get();

  pipelineInfo.renderPass = _renderPass.get();
  pipelineInfo.subpass = 0;

  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex = -1;              // Optional

  if (vkCreateGraphicsPipelines(_renderer->getDevice(), VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &_graphicsPipelie) != VK_SUCCESS) {
    THROW_EXEPTION("failed to create graphics pipeline!");
  }
}

void re::GameEngine::loopStart() {

  while (!_window->shouldClose()) {
    VkCommandBuffer cmdBuf = _renderer->beginFrame();

    window::WindowDims curDims = _window->getDims();
    if (_swapchain->rebuildRequired() || _swapchain->dims() != curDims) {
      vkDeviceWaitIdle(_renderer->getDevice());
      _renderer->destroySwapchainFramebufs();
      _swapchain->rebuild(curDims);
      _renderer->createSwapchainFramebufs(curDims, _renderPass.get());
    }

    _renderer->beginRenderPass(_renderPass.get());

    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _graphicsPipelie.get());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(_swapchain->dims().width);
    viewport.height = static_cast<float>(_swapchain->dims().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmdBuf, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = _swapchain->dims();
    vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {_vertBuff.get()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);

    vkCmdDraw(cmdBuf, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmdBuf);

    _renderer->endFrame();

    _window->pollEvents();
  }
}

re::GameEngine::~GameEngine() { vkDeviceWaitIdle(_renderer->getDevice()); }
