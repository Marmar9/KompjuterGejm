#include "include/ecs/core.hpp"
#include "include/ecs/drawing-service.hpp"
#include "include/ecs/utils/array-stack.hpp"
#include "include/ecs/utils/singleton.hpp"

namespace engine {
namespace ecs {

class EntityManager : public Singleton<EntityManager> {
  friend class Singleton<EntityManager>;

  ArrayStack<entity_t, entity_max> _entities;

  // std::array<sign_t, entity_max> _signData;
  std::array<sign_t, entity_max> _signatures;
  // KeyValueArrayMapper<sign_t, entity_max> _signatures;

  DrawingService &_drawingService;

  EntityManager(DrawingService &dService);

public:
  entity_t getEntity(sign_t signature);

  void relaseEntity(entity_t id);

  sign_t getEntitySign(entity_t id) const;
};
}; // namespace ecs
} // namespace engine
