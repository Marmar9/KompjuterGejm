#include "inc/game/game.hpp"
#include "inc/common/exception.hpp"
#include "inc/common/window/window-base.hpp"
#include "include/ecs/core.hpp"
#include "include/game-engine.hpp"

using namespace engine;
int game::entryPoint(const window::Window &window,
                     const engine::GameEngineParams &params) {

  try {
    engine::GameEngine engine(window, params);

    ecs::EntityManager &eManager = engine::ecs::EntityManager::getInstance();
    ecs::DrawingService &dService = ecs::DrawingService::getInstance();

    LoopContext ctx = {};
    ctx.entities[0] = eManager.getEntity(engine::ecs::EcsSigDrawable);
    ctx.entities[1] = eManager.getEntity(engine::ecs::EcsSigDrawable);
    ctx.entities[2] = eManager.getEntity(engine::ecs::EcsSigDrawable);

    ecs::entity_t e1 = ctx.entities[0];
    ecs::entity_t e2 = ctx.entities[1];
    ecs::entity_t e3 = ctx.entities[2];
    dService.setPos(e1, 0.0, 0.0);
    dService.setPos(e2, 0.7, 0.7);
    dService.setPos(e3, 0.5, 0.5);

    engine.setContext(ctx);

    engine.onRefreshCallback = [](LoopContext &ctx) -> void {
      ctx.dService->draw(ctx.entities[0]);
      ctx.dService->draw(ctx.entities[1]);
      ctx.dService->draw(ctx.entities[2]);
    };

    engine.loopStart();
    return 0;
  } catch (utils::Error e) {
    e.tell();
    return 1;
  }
}
