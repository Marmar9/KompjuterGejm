#include "include/ecs/drawing-service.hpp"
#include "inc/common/loger.h"
#include "include/ecs/core.hpp"
#include <cstddef>
#include <span>

namespace engine::ecs {

template <typename B, typename A>
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

DrawingService::DrawingService(Renderer *renderer)
    : _pRenderer(renderer){
          // _prevVBuff = reinterpret_span<InstanceData>(renderer->getBuffer());
      };

// Usefull later for cache opt
void DrawingService::registerEntity(entity_t id) {}
void DrawingService::releaseEntity(entity_t id) {}

void DrawingService::setPos(entity_t id, float posX, float posY) {
  float posZ = 1.f;
  // LOG("Setting pos for id: %d", id);
  _curVBuff = reinterpret_span<InstanceData>(_pRenderer->getVertBuffer());

  _curVBuff[id] = {{{-0.5f + posX, 0.25f + posY, posZ},  // 0
                    {0.5f + posX, 0.25f + posY, posZ},   // 1
                    {-0.5f + posX, -0.25f + posY, posZ}, // 2
                    {0.5f + posX, -0.25f + posY, posZ}}};
};

void DrawingService::draw(entity_t id) { _pRenderer->render(id * 4, 4); }

} // namespace engine::ecs
