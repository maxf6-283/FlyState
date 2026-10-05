#include <raylib.h>

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

void UpdateAndDraw() {
  if (WindowShouldClose()) {
#if RAYLIB_JAM_WEB
    // The browser owns the page lifetime; stop drawing after a close request.
    emscripten_cancel_main_loop();
#else
    return;
#endif
  }

  BeginDrawing();
  ClearBackground(Color{190, 190, 190, 255});
  DrawText("Raylib Game Jam", 32, 32, 28, DARKGRAY);
  DrawText("Start working on the game!", 32, 72, 20, GRAY);
  EndDrawing();
}
} // namespace

int main() {
  TraceLog(LOG_INFO, "Raylib Game Jam Template started: (%s)\n", BACKEND_STR);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib game jam");
  SetTargetFPS(60);

#if RAYLIB_JAM_WEB
  // Emscripten calls this function from the browser event loop.
  emscripten_set_main_loop(UpdateAndDraw, 0, 1);
#else
  while (!WindowShouldClose()) {
    UpdateAndDraw();
  }
  CloseWindow();
#endif

  return 0;
}
