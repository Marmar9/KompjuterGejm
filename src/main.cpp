#include "include/game-engine.hpp"
#include "include/utils/exeption.hpp"
#include <vulkan/vulkan_core.h>

// __attribute__((no_stack_protector))

int main(int argc, char *argv[]) {

  try {
    re::GameEngine engine;
    engine.loopStart();
  } catch (utils::EngineExeption e) {
    e.tell();
    return 1;
  }

  return 0;
}
