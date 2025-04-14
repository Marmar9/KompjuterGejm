#include "android_native_app_glue.h"
#include "inc/common/exception.hpp"
#include "inc/common/loger.h"
#include "inc/game/game.hpp"
#include "inc/platform/android/window/android-window.hpp"
#include "include/game-engine.hpp"
#include <jni.h>
#include <pthread.h>

void android_main(struct android_app *app) {
  LOG("Logs begin here ----------")

  app->onAppCmd = [](struct android_app *app, int msg) {};
  window::AndroidWindow window = window::AndroidWindow(app);

  pthread_mutex_lock(&app->mutex);
  app->running = 1;
  pthread_mutex_unlock(&app->mutex);

  pthread_cond_signal(&app->cond);

  engine::GameEngineParams params{};
  game::entryPoint(window, params);
}
