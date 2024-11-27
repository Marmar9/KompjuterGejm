#include "include/vulkan/renderer-base.hpp"
#include "include/vulkan/handle.hpp"
#include <vulkan/vulkan_core.h>

#include <cstdlib>

using namespace renderer;

RendererBase::RendererBase() : _v{}, _khr{}, _currentFrameIndex(0) {
  _v.cmdBufs.reset(new VkCommandBuffer[_framesInFlight]());
  _v.imgAvailableSems.reset(new VkSemaphore[_framesInFlight](),
                            _framesInFlight);
  _v.renderFiniSems.reset(new VkSemaphore[_framesInFlight](), _framesInFlight);
  _v.inFlightFenses.reset(new VkFence[_framesInFlight](), _framesInFlight);
}
