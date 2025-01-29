#include "inc/game/game.hpp"
#include "inc/common/exception.hpp"
#include "inc/common/window/window-base.hpp"

int game::entryPoint(const window::Window &window,
                     const re::GameEngineParams &params) {

  try {
    re::GameEngine engine(window, params);
    engine.loopStart();
    return 0;
  } catch (utils::Error e) {
    e.tell();
    return 1;
  }
}
