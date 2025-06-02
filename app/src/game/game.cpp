#include "inc/game/game.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "inc/common/exception.hpp"
#include "inc/common/window/window-base.hpp"
#include "inc/common/window/window-dims.hpp"
#include "include/ecs/core.hpp"
#include "include/game-engine.hpp"

const constexpr float near = .1F;
const constexpr float far = 1.F;

using namespace engine;
int game::entryPoint(window::Window &window,
                     const engine::GameEngineParams &params) {

  try {
    engine::GameEngine engine(window, params);

    ecs::EntityManager &eManager = engine::ecs::EntityManager::getInstance();
    ecs::DrawingService &dService = ecs::DrawingService::getInstance();

    ecs::entity_t e1 = eManager.getEntity(engine::ecs::EcsSigDrawable);
    ecs::entity_t e2 = eManager.getEntity(engine::ecs::EcsSigDrawable);
    ecs::entity_t e3 = eManager.getEntity(engine::ecs::EcsSigDrawable);

    // Move the loop outside

    while (!window.shouldClose()) {
      window::WindowDims dims = window.getDims();
      float aspectR = (float)dims.width / dims.height;

      engine.beginFrame();

      dService.setCameraDirection(glm::vec3(.0, .0, .0), glm::vec3(.0, .0, 1.));
      dService.setPerspective(near, far, glm::radians(45.f), aspectR);

      dService.setPos(e1, 0.0, 0.0);
      dService.setPos(e2, 0.7, 0.7);
      dService.setPos(e3, 0.5, 0.5);

      dService.draw(e1);
      dService.draw(e2);
      dService.draw(e3);
      engine.render();

      window.pollEvents();
    };

    return 0;
  } catch (utils::Error e) {
    e.tell();
    return 1;
  }
}
