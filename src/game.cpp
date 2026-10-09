#include <raylib.h>
#include <raymath.h>

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

IKSolver leg{};
IKSolver arm{};

float initialX = GetScreenWidth() * 0.5f;

struct Foot {
  Vector2 foot{};
  Vector2 footTarget{};

  Vector2 prevFoot{};
  bool isStepping = false;
  float strideLength = 80.0f;
  float legLength = 200.0f;
  float stepHeight = 50.0f;

  float stepDuration = 0.5f;
  float stepTimer = 0.0f;
};

Foot rightFoot{Vector2{initialX, 500.0f}, Vector2{initialX, 500.0f},
               Vector2{initialX, 500.0f}};

Foot leftFoot{Vector2{initialX - 50.0f, 500.0f},
              Vector2{initialX - 50.0f, 500.0f},
              Vector2{initialX - 50.0f, 500.0f}};

bool forward = true;

void UpdateFoot(Vector2 &hip, Foot &foot, bool forward) {
  if (fabs(foot.foot.x - hip.x) >= foot.strideLength && !foot.isStepping) {
    if (forward) {
      foot.footTarget.x = hip.x + foot.strideLength;
    } else {
      foot.footTarget.x = hip.x - foot.strideLength;
    }

    foot.prevFoot = foot.foot;
    foot.isStepping = true;
  }

  if (foot.isStepping) {
    foot.stepTimer += GetFrameTime();

    float t = foot.stepTimer / foot.stepDuration;

    foot.foot.x = (1.0f - t) * foot.prevFoot.x + t * foot.footTarget.x;
    foot.foot.y = foot.footTarget.y - foot.stepHeight * sinf(PI * t);

    if (foot.stepTimer >= foot.stepDuration) {
      foot.isStepping = false;
    }
  }

  if (!foot.isStepping) {
    foot.foot = foot.footTarget;
    foot.stepTimer = 0.0f;
  }
}

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

  initialX += (forward ? 150.0f : -150.0f) * GetFrameTime();
  if (initialX > GetScreenWidth() || initialX < 0.0f) {
    forward = !forward;
  }

  Vector2 hip = {initialX, 300};

  if (!leftFoot.isStepping) {
    UpdateFoot(hip, rightFoot, forward);
  }
  if (!rightFoot.isStepping) {
    UpdateFoot(hip, leftFoot, forward);
  }

  float horizontalOff = 0.0f;

  float minimumFootDistance = 150.0f;
  if (leftFoot.isStepping) {
    horizontalOff = hip.x - rightFoot.foot.x;

    float maxHorizontalOffset =
        sqrtf(fmax(0.0f, rightFoot.legLength * rightFoot.legLength -
                             minimumFootDistance * minimumFootDistance));

    horizontalOff =
        Clamp(horizontalOff, -maxHorizontalOffset, maxHorizontalOffset);

    hip.y = rightFoot.foot.y - sqrtf(rightFoot.legLength * rightFoot.legLength -
                                     horizontalOff * horizontalOff);
  } else {
    horizontalOff = hip.x - leftFoot.foot.x;

    float maxHorizontalOffset =
        sqrtf(fmax(0.0f, leftFoot.legLength * leftFoot.legLength -
                             minimumFootDistance * minimumFootDistance));

    horizontalOff =
        Clamp(horizontalOff, -maxHorizontalOffset, maxHorizontalOffset);

    hip.y = leftFoot.foot.y - sqrtf(leftFoot.legLength * leftFoot.legLength -
                                    horizontalOff * horizontalOff);
  }

  Vector2 armPole = {initialX - ((forward) ? 100.0f : -100.0f), 100};
  Vector2 legPole = {initialX + ((forward) ? 100.0f : -100.0f), 100};

  float radius = 100.0f;

  IKSolver::Input input{hip, hip + Vector2{0.0f, 100.0f},
                        hip + Vector2{0.0f, 200.0f}, legPole};

  Vector2 body = {initialX, hip.y - 20.0f};

  Vector2 shoulder = {initialX, body.y - 100.0f};
  Vector2 elbow = {initialX, shoulder.y + 90.0f};
  Vector2 hand = {initialX, elbow.y + 90.0f};

  float armRadius = Lerp(20.0f, 50.0f, Clamp(cos(GetTime()), 0.0f, 1.0f));
  Vector2 armTarget =
      shoulder + Vector2(cos(GetTime() * 2.5f) * armRadius * 5.5f,
                         sin(GetTime()) * armRadius * 2.2f);
  armTarget.y += 115.0f;

  IKSolver::Input armInput{shoulder, elbow, hand, armPole};

  IKSolver::Output legOutput = leg.solve(input, rightFoot.foot);
  IKSolver::Output leftLegOutput = leg.solve(input, leftFoot.foot);
  IKSolver::Output armOutput = arm.solve(armInput, armTarget);

  auto drawLimb = [](Vector2 root, IKSolver::Output limb, Color pointColor) {
    DrawLineV(root, limb.mid, BLUE);
    DrawLineV(limb.mid, limb.effect, BLUE);

    DrawCircleV(root, 10.0f, pointColor);
    DrawCircleV(limb.mid, 10.0f, pointColor);
    DrawCircleV(limb.effect, 10.0f, pointColor);
  };

  drawLimb(hip, leftLegOutput, DARKGREEN);
  // lower and upper body
  DrawCircleV(body, 45.0f, YELLOW);
  DrawCircleV(body - Vector2{0.0f, 90.0f}, 45.0f, YELLOW);
  drawLimb(hip, legOutput, GREEN);
  drawLimb(shoulder, armOutput, GREEN);

  DrawCircleV(armTarget, 10.0f, RED);
  DrawCircleV(legPole, 10.0f, PURPLE);
  DrawCircleV(armPole, 10.0f, BROWN);
  DrawCircleV(rightFoot.footTarget, 10.0f, RED);

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
