#pragma once

namespace vulkan {
template <typename... Args> class Buildable {
public:
  Buildable() = default;
  Buildable(const Buildable &&) = delete;
  virtual ~Buildable() = default;

  virtual void build(Args *...args) const = 0;
};
} // namespace vulkan
