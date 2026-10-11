#include <raylib.h>
#include <raymath.h>

#include "Human.h"
#include "IKSolver.h"

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

  Vector2 humanPos = testHuman.GetPosition();
  humanPos.y = 300.0f;

  bool forward = testHuman.GetForward();

  if (IsKeyDown(KEY_D)) {
    humanPos.x += 150.0f * GetFrameTime();
    forward = true;
  }

  if (IsKeyDown(KEY_A)) {
    humanPos.x -= 150.0f * GetFrameTime();
    forward = false;
  }

  testHuman.SetForward(forward);

  if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
    testHuman.StabilizeBody();
  } else {
    Vector2 rightFoot = testHuman.GetRightFoot();
    Vector2 leftFoot = testHuman.GetLeftFoot();

    float offset = forward ? 60.0f : -60.0f;

    testHuman.rightArmTo(Vector2{leftFoot.x + offset, humanPos.y + 30.0f},
                         0.1f);

    testHuman.leftArmTo(Vector2{rightFoot.x + offset, humanPos.y + 30.0f},
                        0.1f);

    testHuman.SetPosition(humanPos);
  }

  // testHuman.leftArmTo(
  //     Vector2{cosf(GetTime() * 10.0f) * 100.0f, sinf(GetTime()) * 50.0f},
  //     0.1f);

  testHuman.Update();

  initialX = humanPos.x;

  Human::LimbOutput leftLeg = testHuman.GetLeftLeg();
  Human::LimbOutput rightLeg = testHuman.GetRightLeg();

  Human::LimbOutput leftArm = testHuman.GetLeftArm();
  Human::LimbOutput rightArm = testHuman.GetRightArm();

  Vector2 lowerBody = {initialX, leftLeg.root.y};
  Vector2 upperBody = {initialX, leftLeg.root.y};

  Vector2 armPole = {initialX - ((forward) ? 100.0f : -100.0f), 100};
  Vector2 legPole = {initialX + ((forward) ? 100.0f : -100.0f), 100};

  Vector2 shoulder = {upperBody.x, upperBody.y - 100.0f};
  Vector2 elbow = {initialX, shoulder.y + 90.0f};
  Vector2 hand = {initialX, elbow.y + 90.0f};

  auto drawLimb = [](Vector2 root, IKSolver::Output limb, Color pointColor) {
    DrawLineV(root, limb.mid, BLUE);
    DrawLineV(limb.mid, limb.effect, BLUE);

    DrawCircleV(root, 10.0f, pointColor);
    DrawCircleV(limb.mid, 10.0f, pointColor);
    DrawCircleV(limb.effect, 10.0f, pointColor);
  };

  drawLimb(leftLeg.root, {leftLeg.mid, leftLeg.effect}, DARKGREEN);
  drawLimb(leftArm.root, {leftArm.mid, leftArm.effect}, DARKGREEN);
  // lower and upper body
  DrawCircleV(lowerBody, 45.0f, YELLOW);
  DrawCircleV(upperBody - Vector2{0.0f, 90.0f}, 45.0f, YELLOW);

  drawLimb(rightLeg.root, {rightLeg.mid, rightLeg.effect}, GREEN);
  drawLimb(rightArm.root, {rightArm.mid, rightArm.effect}, GREEN);

  DrawCircleV(legPole, 10.0f, PURPLE);
  DrawCircleV(armPole, 10.0f, BROWN);

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
