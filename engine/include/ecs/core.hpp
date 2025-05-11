#pragma once

#include <cstddef>
#include <cstdint>
namespace engine::ecs {
using entity_t = uint16_t;

constexpr uint16_t entity_max = 100;

using sign_t = uint16_t;

constexpr sign_t EcsSigDrawable = 1;
constexpr sign_t EcsSigColidable = 2;

} // namespace engine::ecs
