#include "include/ecs/entity-manager.hpp"
#include "inc/common/loger.h"
#include "include/ecs/core.hpp"
#include "include/ecs/drawing-service.hpp"

namespace engine::ecs {

EntityManager::EntityManager(DrawingService &dService)
    : _drawingService(dService) {
  for (entity_t i = entity_max - 1; i-- > 0;) {
    _entities.push(i);
  }
};

entity_t EntityManager::getEntity(sign_t signature) {

  entity_t entity = _entities.pop();
  if (signature & EcsSigDrawable) {
    _drawingService.registerEntity(entity);
  } else if (signature & EcsSigColidable) {
  }
  _signatures[entity] = signature;
  return entity;
}

void EntityManager::relaseEntity(entity_t id) {
  _signatures[id] = 0;

  _entities.push(id);
}

// Potential optimization
sign_t EntityManager::getEntitySign(entity_t id) const {
  return _signatures[id];
}

} // namespace engine::ecs
