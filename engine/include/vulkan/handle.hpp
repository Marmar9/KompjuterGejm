#pragma once

#include <cstddef>

#include <vulkan/vulkan_core.h>
namespace engine::vulkan {

class Deleter {
  static VkInstance _instance;
  static VkDevice _device;

public:
  static void Init(VkInstance instance, VkDevice device) {
    _instance = instance;
    _device = device;
  }

  static void operator()(VkInstance resource) noexcept;
  static void operator()(VkDevice resource) noexcept;
  static void operator()(VkDebugUtilsMessengerEXT resource) noexcept;
  static void operator()(VkSwapchainKHR resource) noexcept;
  static void operator()(VkDeviceMemory resource) noexcept;
  static void operator()(VkBuffer resource) noexcept;
  static void operator()(VkSurfaceKHR resource) noexcept;
  static void operator()(VkSemaphore resource) noexcept;
  static void operator()(VkFence resource) noexcept;
  static void operator()(VkPipelineLayout resource) noexcept;
  static void operator()(VkRenderPass resource) noexcept;
  static void operator()(VkPipeline resource) noexcept;
  static void operator()(VkShaderModule resource) noexcept;
  static void operator()(VkCommandPool resource) noexcept;
  static void operator()(VkImageView resource) noexcept;
  static void operator()(VkFramebuffer resource) noexcept;
};

template <typename T> class Handle {};

template <typename T> class Handle<T *> {
  T *_pointer;

public:
  Handle() { _pointer = nullptr; };
  Handle(const Handle &&) = delete;
  Handle(const Handle &) = delete;
  const Handle &operator=(size_t) = delete;

  ~Handle() {
    if (_pointer) {
      Deleter::operator()(_pointer);
      _pointer = nullptr;
    }
  }

  T **operator&() noexcept { return &_pointer; }

  void move(Handle &&source) noexcept {
    _pointer = source._pointer;
    source._pointer = nullptr;
  }

  void reset(T *source) noexcept {
    if (_pointer) {
      Deleter::operator()(_pointer);
    }
    _pointer = source;
  }

  T *get() const noexcept { return _pointer; }
};

template <typename T> class Handle<T **> {
  size_t _sz;
  T **_pointer;

public:
  Handle() { _pointer = nullptr; };
  Handle(const Handle &&) = delete;
  Handle(const Handle &) = delete;
  const Handle &operator=(size_t) = delete;

  ~Handle() {
    if (_pointer) {
      for (size_t i = 0; i < _sz; ++i) {
        if (_pointer[i]) {
          Deleter::operator()(_pointer[i]);
          _pointer[i] = nullptr;
        }
      }
      _pointer = nullptr;
    }
  }

  // Access array elements
  T *&operator[](size_t i) const noexcept { return _pointer[i]; }

  void move(Handle &&source) noexcept {
    _pointer = source._pointer;
    _sz = source._sz;
    source._pointer = nullptr;
  }

  void reset(T **source, size_t sz) noexcept {
    if (_pointer) {
      for (size_t i = 0; i < _sz; ++i) {
        if (_pointer[i]) {
          Deleter::operator()(_pointer[i]);
        }
      }
    }
    _pointer = source;
    _sz = sz;
  }

  T **get() const noexcept { return _pointer; }
};

} // namespace engine::vulkan
