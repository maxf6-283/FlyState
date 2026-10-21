#include <raylib.h>
#include <raymath.h>

#include "Human.h"
#include "IKSolver.h"

#include "screen_manager.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define RAYLIB_JAM_WEB 1
#define BACKEND_STR "WEB"
#else
#define RAYLIB_JAM_WEB 0
#define BACKEND_STR "DESKTOP"
#endif

namespace {
constexpr int SCREEN_WIDTH = 960;
constexpr int SCREEN_HEIGHT = 540;

float initialX = GetScreenWidth() * 0.5f;

Human::Foot rightFoot{Vector2{initialX, 500.0f}, Vector2{initialX, 500.0f},
                      Vector2{initialX, 500.0f}};

Human::Foot leftFoot{Vector2{initialX - 50.0f, 500.0f},
                     Vector2{initialX - 50.0f, 500.0f},
                     Vector2{initialX - 50.0f, 500.0f}};

Human testHuman(leftFoot, rightFoot, 0.0f);

IKSolver arm;

Vector2 bodyTarget;
Vector2 bodyDestination;

void UpdateAndDraw() {
  if (WindowShouldClose()) {
#if RAYLIB_JAM_WEB
    // The browser owns the page lifetime; stop drawing after a close request.
    emscripten_cancel_main_loop();
#else
    return;
#endif
  }
  ScreenManager &screenManager = ScreenManager::Get();

  screenManager.Update();

  BeginDrawing();
  ClearBackground(Color{190, 190, 190, 255});

  screenManager.Draw();

  EndDrawing();
}
} // namespace

int main() {
  TraceLog(LOG_INFO, "FlyState started: (%s)\n", BACKEND_STR);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fly State");
  SetTargetFPS(60);

  InitAudioDevice();

#if RAYLIB_JAM_WEB
  // Emscripten calls this function from the browser event loop.
  emscripten_set_main_loop(UpdateAndDraw, 0, 1);
#else
  while (!WindowShouldClose()) {
    UpdateAndDraw();
  }
  CloseAudioDevice();
  CloseWindow();
#endif

  return 0;
}
