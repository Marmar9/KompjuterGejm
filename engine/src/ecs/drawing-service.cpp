#include "include/ecs/drawing-service.hpp"
#include "inc/common/loger.h"
#include "include/ecs/core.hpp"
#include "include/vulkan/vertex.hpp"
#include <span>

namespace engine::ecs {

template <typename A, typename B>
std::span<B> reinterpret_span(std::span<A> input) {
  static_assert(std::is_trivially_copyable_v<A>,
                "A must be trivially copyable");
  static_assert(std::is_trivially_copyable_v<B>,
                "B must be trivially copyable");
  static_assert(sizeof(B) % sizeof(A) == 0,
                "sizeof(B) must be a multiple of sizeof(A)");

  constexpr std::size_t N = sizeof(B) / sizeof(A);

  if (input.size() % N != 0) {
    throw std::runtime_error(
        "reinterpret_span: input.size() must be a multiple of "
        "sizeof(B)/sizeof(A)");
  }

  auto *ptr = reinterpret_cast<B *>(input.data());
  return {ptr, input.size() / N};
}

DrawingService::DrawingService(Renderer *renderer) : _pRenderer(renderer) {
  std::span<vulkan::Vertex> pVertex = renderer->getBuffer().data();

  _vData = reinterpret_span<vulkan::Vertex, InstanceData>(pVertex);
};

void DrawingService::registerEntity(entity_t id) { _vData[id]; }
void DrawingService::releaseEntity(entity_t id) { _vData[id]; }

void DrawingService::setPos(entity_t id, float posX, float posY) {
  LOG("Setting pos for id: %d", id);

  _vData[id] = {{{-0.5f + posX, 0.0f + posY},   // 0
                 {0.5f + posX, 0.0f + posY},    // 1
                 {-0.5f + posX, -0.25f + posY}, // 2
                 {0.5f + posX, -0.25f + posY}}};
};

void DrawingService::draw(entity_t id) { _pRenderer->render(id * 4, 4); }

} // namespace engine::ecs
