#include "fly.h"

#include <raymath.h>

#include <cmath>

#include "obstacle.h"
#include "raylib.h"

Fly::Fly(Vector2 pos) : m_position{pos}, m_velocity{0.0f, 0.0f} {};

void Fly::Update(const std::vector<Obstacle> &obstacles) {
  float dt =
      GetFrameTime(); // Time passed since last frame (around 0.016s at 60FPS)

  // 1. Calculate target direction based on inputs
  Vector2 inputDirection = {0.0f, 0.0f};
  if (IsKeyDown(KEY_D))
    inputDirection.x += 1.0f;
  if (IsKeyDown(KEY_A))
    inputDirection.x -= 1.0f;
  if (IsKeyDown(KEY_S))
    inputDirection.y += 1.0f;
  if (IsKeyDown(KEY_W))
    inputDirection.y -= 1.0f;

  // Normalize input to prevent moving faster diagonally
  if (inputDirection.x != 0.0f && inputDirection.y != 0.0f) {
    inputDirection = Vector2Normalize(inputDirection);
  }

  // 2. Apply acceleration over time
  m_velocity.x += inputDirection.x * ACCELERATION * dt;
  m_velocity.y += inputDirection.y * ACCELERATION * dt;

  // 3. Apply friction/drag
  float frictionDelta = FRICTION * dt;

  if (Vector2LengthSqr(m_velocity) > frictionDelta * frictionDelta) {
    Vector2 normalized_vel = Vector2Normalize(m_velocity);

    m_velocity -= normalized_vel * frictionDelta;
  } else {
    m_velocity.x = 0;
    m_velocity.y = 0;
  }

  // 4. 💡 Limit maximum speed (Speed Cap)
  if (Vector2LengthSqr(m_velocity) > MAX_SPEED * MAX_SPEED) {
    m_velocity = Vector2Normalize(m_velocity) * MAX_SPEED;
  }

  // 5. Update position using our calculated velocity
  m_position += m_velocity;
}

void Fly::Draw() const { DrawCircleV(m_position, 15, MAROON); }

void Fly::Offset(Vector2 pos) { m_position += pos; }

void Fly::SetPos(Vector2 pos) { m_position = pos; }

void Fly::SetVel(Vector2 vel) { m_velocity = vel; }
