#include "inc/common/exception.hpp"
#include "inc/common/static-arr-len.hpp"
#include "inc/common/window/window-dims.hpp"
#include "include/game-engine.hpp"
#include <array>
#include <cstdlib>
#include <cstring>
#include <glm/ext/vector_float2.hpp>
#include <span>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

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

  // enginenderPassInfo.dependencyCount = 1;
  // enginenderPassInfo.pDependencies = &dependency;
  renderPassInfo.dependencyCount = 0;
  renderPassInfo.pDependencies = nullptr;

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, renderPass) !=
      VK_SUCCESS) {
    THROW_EXCEPTION("failed to create enginender pass!");
  }
}

engine::GameEngine::GameEngine(const window::Window &window,
                               const engine::GameEngineParams &params)
    : _window(window) {
  _renderer.reset(new engine::Renderer(_window));
  vulkan::Deleter::Init(_renderer->getInstance(), _renderer->getDevice());

  _swapchain.reset(new engine::Swapchain(_window, _renderer->getDevCaps(),
                                         _renderer->getDevice(),
                                         _renderer->getSurface()));
  _renderer->init(_swapchain.get());
  createRenderPasses(_renderer->getDevice(), _swapchain->getFormat(),
                     &_renderPass);

  buf_t verticies[] = {
      buf_t{0.0f, -0.5f}, buf_t{0.5f, 0.5f}, buf_t{-0.5f, 0.5f},
      // buf_t{0.0f, 0.5f},
  };

  // Binding for buffer 1
  _vertBuf.create(STATIC_ARR_LEN(verticies), _renderer->getDevice(),
                  _renderer->getPhysicalDevice());

  _vertBuf.setData(verticies);

  _bindings[buf_b] = _vertBuf.get();

  std::array<VkVertexInputBindingDescription, 1> bindingDescription{};

  bindingDescription[0] =
      _vertBuf.getBindingDesc(buf_b, VK_VERTEX_INPUT_RATE_VERTEX);

  std::array<VkVertexInputAttributeDescription, 1> attrDescription{};
  attrDescription[0].binding = buf_b;
  attrDescription[0].location = 0;
  attrDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
  attrDescription[0].offset = 0;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                    VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;            // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

  vulkan::GraphicsPipelineBuilder gPipBuilder = vulkan::GraphicsPipelineBuilder(
      _renderer->getDevice(), _renderPass.get());

  gPipBuilder.setExtent(_window.getDims())
      ->setBindings(bindingDescription, attrDescription)
      ->setAssemblyTopo(inputAssembly)
      ->setDynamicStates(dynamicStates)
      ->setPipelineLayout(pipelineLayoutInfo);

  _gPipeline.reset(new vulkan::GraphicsPipeline(gPipBuilder));

  _renderer->createSwapchainFramebufs(_swapchain->dims(), _renderPass.get());
}

void engine::GameEngine::loopStart() {
  while (!_window.shouldClose()) {
    VkCommandBuffer cmdBuf = _renderer->beginFrame();

    window::WindowDims curDims = _window.getDims();
    if (_swapchain->rebuildRequired() || _swapchain->dims() != curDims) {
      vkDeviceWaitIdle(_renderer->getDevice());
      _renderer->destroySwapchainFramebufs();
      _swapchain->rebuild(curDims);
      _renderer->createSwapchainFramebufs(curDims, _renderPass.get());
    }

    _renderer->beginRenderPass(_renderPass.get());

    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _gPipeline.get()->get());

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

    vkCmdBindVertexBuffers(cmdBuf, 0, _bindingCount, _bindings, _offsets);

    vkCmdDraw(cmdBuf, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmdBuf);

    _renderer->endFrame();

    _window.pollEvents();
  }
}

engine::GameEngine::~GameEngine() { vkDeviceWaitIdle(_renderer->getDevice()); }
