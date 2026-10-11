#include "fly.h"

#include <raymath.h>

#include <cmath>

#include "raylib.h"

Fly::Fly(Vector2 pos) : m_position{pos}, m_velocity{0.0f, 0.0f} {};

void Fly::Update() {
  float dt =
      GetFrameTime();  // Time passed since last frame (around 0.016s at 60FPS)

  // 1. Calculate target direction based on inputs
  Vector2 inputDirection = {0.0f, 0.0f};
  if (IsKeyDown(KEY_D)) inputDirection.x += 1.0f;
  if (IsKeyDown(KEY_A)) inputDirection.x -= 1.0f;
  if (IsKeyDown(KEY_S)) inputDirection.y += 1.0f;
  if (IsKeyDown(KEY_W)) inputDirection.y -= 1.0f;

  // Normalize input to prevent moving faster diagonally
  if (inputDirection.x != 0.0f && inputDirection.y != 0.0f) {
    inputDirection = Vector2Normalize(inputDirection);
  }

  // 2. Apply acceleration over time
  m_velocity.x += inputDirection.x * ACCELERATION * dt;
  m_velocity.y += inputDirection.y * ACCELERATION * dt;

  // 3. Apply friction/drag when no keys are pressed
  float frictionDelta = FRICTION * dt;

  if (inputDirection.x == 0.0f) {
    if (std::fabs(m_velocity.x) <= frictionDelta) {
      m_velocity.x = 0.0f;
    } else {
      m_velocity.x += (m_velocity.x > 0.0f) ? -frictionDelta : frictionDelta;
    }
  }

  if (inputDirection.y == 0.0f) {
    if (std::fabs(m_velocity.y) <= frictionDelta) {
      m_velocity.y = 0.0f;
    } else {
      m_velocity.y += (m_velocity.y > 0.0f) ? -frictionDelta : frictionDelta;
    }
  }

  // 4. 💡 Limit maximum speed (Speed Cap)
  // Vector2Clamp caps the length of the vector between 0 and MAX_SPEED
  m_velocity = Vector2Clamp(m_velocity, Vector2{-MAX_SPEED, -MAX_SPEED},
                            Vector2{MAX_SPEED, MAX_SPEED});

  // 5. Update position using our calculated velocity
  m_position.x += m_velocity.x;
  m_position.y += m_velocity.y;
}

void Fly::Draw() const { DrawCircleV(m_position, 20, MAROON); }