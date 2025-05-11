#pragma once
#include "include/ecs/core.hpp"
#include "include/ecs/utils/singleton.hpp"
#include "include/vulkan/renderer.hpp"
#include "include/vulkan/vertex.hpp"
#include <span>
namespace engine {

namespace ecs {
class DrawingService : public Singleton<DrawingService> {
  friend class Singleton<DrawingService>;

  struct InstanceData {
    vulkan::Vertex data[4];
  };

  // Posible cache optimizations
  std::span<InstanceData> _vData;

  Renderer *_pRenderer;

  DrawingService(Renderer *renderer);

public:
  void registerEntity(entity_t id);
  void releaseEntity(entity_t id);

  void setPos(entity_t id, float posX, float posY);
  void draw(entity_t id);
};

} // namespace ecs
} // namespace engine
