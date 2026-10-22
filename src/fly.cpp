#include "fly.h"

#include <raymath.h>

#include <cmath>

#include "obstacle.h"
#include "raylib.h"

Fly::Fly(Vector2 pos) : m_position{pos}, m_velocity{0.0f, 0.0f} {}

void Fly::Update(const std::vector<Obstacle> &obstacles) {
  if (m_CanBuzz && IsKeyPressed(KEY_SPACE) && !IsSoundPlaying(m_Buzz)) {
    PlaySound(m_Buzz);
  }

  if (IsKeyPressed(KEY_Q) && m_CanParry && m_CanDash) {
    m_CanParry = false;
    m_Parrying = true;
    m_ParryTimer = 0.0f;
    m_velocity = Vector2Zero();
  }

  if (!m_CanParry && !m_Parrying) {
    m_ParryTimer += GetFrameTime();
    if (m_ParryTimer >= m_ParryCooldown) {
      m_ParryTimer = 0.0f;
      m_CanParry = true;
    }
  }

  if (m_Parrying) {
    currentMaxSpeed = 2.0f;
    m_ParryTimer += GetFrameTime();
    if (m_ParryTimer >= m_ParryDuration) {
      m_ParryTimer = 0.0f;
      currentMaxSpeed = MAX_SPEED;
      m_Parrying = false;
    }
  }

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

  if (IsKeyPressed(KEY_LEFT_SHIFT) && m_CanDash && m_CanParry) {
    m_velocity = Vector2Zero();
    m_CanDash = false;
    m_DashTimer = 0.0f;
    m_Dashing = true;
    m_DashDirection = inputDirection;
    if (m_DashDirection == Vector2Zero()) {
      m_DashDirection = {1.0f, 0.0f};
    }
  }

  if (!m_CanDash && !m_Dashing) {
    m_DashTimer += GetFrameTime();
    if (m_DashTimer >= m_DashCooldown) {
      m_DashTimer = 0.0f;
      m_CanDash = true;
    }
  }

  if (m_Dashing) {
    currentMaxSpeed = 10.0f;
    inputDirection = m_DashDirection;

    m_DashTimer += GetFrameTime();
    if (m_DashTimer >= m_DashDuration) {
      m_DashTimer = 0.0f;
      currentMaxSpeed = MAX_SPEED;
      m_Dashing = false;
    }
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
  if (Vector2LengthSqr(m_velocity) > currentMaxSpeed * currentMaxSpeed) {
    m_velocity = Vector2Normalize(m_velocity) * currentMaxSpeed;
  }

  // 5. Update position using our calculated velocity
  m_position += m_velocity;
}

bool Fly::IsBuzzing() const { return IsSoundPlaying(m_Buzz); }
bool Fly::CanBuzz() const { return m_CanBuzz; }
void Fly::SetCanBuzz(bool canBuzz) { m_CanBuzz = canBuzz; }
bool Fly::CanDash() const { return !m_Dashing && m_CanDash; }
bool Fly::IsInvincible() const { return m_Dashing; }

void Fly::Draw() const {
    DrawTexturePro(m_FlySprite, {0, 0, (float)m_FlySprite.width, (float)m_FlySprite.height}, { m_position.x - 15, m_position.y - 15, 30, 30}, Vector2Zero(), 0.0f, WHITE);
  if (m_Parrying) {
    DrawCircleLinesEx(m_position, 20.0f, 2.0f, ColorBrightness(MAROON, -0.5));
  }
}

void Fly::Offset(Vector2 pos) { m_position += pos; }

void Fly::SetPos(Vector2 pos) { m_position = pos; }

void Fly::SetVel(Vector2 vel) { m_velocity = vel; }

void Fly::Slapped() {
  m_Dead = true;

  float halfWidth = GetScreenWidth() * 0.5f;
  float halfHeight = GetScreenHeight() * 0.5f;
  m_position = Vector2{halfWidth, halfHeight};
}
bool Fly::IsDead() const { return m_Dead; }

bool Fly::CanParry() const { return !m_Parrying && m_CanParry; }

bool Fly::IsParrying() const { return m_Parrying; }

void Fly::Reset() {
  StopSound(m_Buzz);
  m_Dead = false;
  m_Money = 0;
  m_HumansDestroyedCounter = 0;
  m_position = {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
  m_velocity = Vector2Zero();
  m_CanDash = true;
  m_Dashing = false;
  m_DashTimer = 0.0f;
  m_DashDirection = Vector2Zero();
  m_CanParry = true;
  m_Parrying = false;
  m_ParryTimer = 0.0f;
  currentMaxSpeed = MAX_SPEED;
}
