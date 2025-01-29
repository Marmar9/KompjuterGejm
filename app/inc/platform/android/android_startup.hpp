#include "android_native_app_glue.h"
#include "inc/common/exception.hpp"
#include "inc/common/loger.h"
#include "inc/game/game.hpp"
#include "inc/platform/android/window/android-window.hpp"
#include "include/game-engine.hpp"

bool flag = true;
void onAppCmd(struct android_app *app, int msg) {
  switch (msg) {
  case APP_CMD_INIT_WINDOW:
    flag = false;
    break;
  }
}

// adb logcat NativeActivityDemo:* *:S
void android_main(struct android_app *app) {
  LOG("Logs begin here ----------")
  app->onAppCmd = onAppCmd;
  while (flag) {
    int outEvents;
    int outFd;
    int res = ALooper_pollOnce(-1, &outFd, &outEvents, nullptr);

    app->cmdPollSource.process(app, &app->cmdPollSource);
  }

  window::AndroidWindow window = window::AndroidWindow(app->window);
  re::GameEngineParams params{};

  game::entryPoint(window, params);
}
