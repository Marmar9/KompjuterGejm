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
  std::span<InstanceData> _curVBuff;
  std::span<InstanceData> _prevVBuff;

  Renderer *_pRenderer;

  DrawingService(Renderer *renderer);

public:
  void registerEntity(entity_t id);
  void releaseEntity(entity_t id);
  void setPerspective(float near, float far, float fovY, float aspect) {
    glm::mat4 &perspective = _pRenderer->getUniBuffer()[0].perspective;
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = tan(fovY / 2.f);

    perspective[0][0] = 1.f / (aspect * tanHalfFovy);
    perspective[1][1] = 1.f / (tanHalfFovy);
    perspective[2][2] = far / (far - near);
    perspective[2][3] = 1.f;
    perspective[3][2] = -(far * near) / (far - near);
  }

  void setCameraDirection(glm::vec3 position, glm::vec3 direction) {
    glm::mat4 &camera = _pRenderer->getUniBuffer()[0].camera;
    glm::vec3 up(.0, -1., .0);

    const glm::vec3 w = glm::normalize(direction);
    const glm::vec3 u = glm::normalize(glm::cross(w, up));
    const glm::vec3 v = glm::cross(w, u);

    camera[0] = glm::vec4(u.x, v.x, w.x, 0);
    camera[1] = glm::vec4(u.y, v.y, w.y, 0);
    camera[2] = glm::vec4(u.z, v.z, w.z, 0);
    camera[3] = glm::vec4(-glm::dot(u, position), -glm::dot(v, position),
                          -glm::dot(w, position), 1);
  }

  void setPos(entity_t id, float posX, float posY);
  void draw(entity_t id);
};

} // namespace ecs
} // namespace engine
