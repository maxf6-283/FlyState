#pragma once

#include <raylib.h>
#include <raymath.h>

#include <vector>

#include "obstacle.h"

class AssetMap;

class Fly {
public:
  Fly(Vector2 pos);

  void Update(const std::vector<Obstacle> &obstacles = {});
  void Draw() const;

  Vector2 position() const { return m_position; }
  Vector2 velocity() const { return m_velocity; }
  float radius() const { return COLLISION_RADIUS; }

  void Offset(Vector2 pos);
  void SetPos(Vector2 pos);
  void SetVel(Vector2 vel);

  bool IsBuzzing() const;
  bool CanBuzz() const;
  void SetCanBuzz(bool canBuzz);
  bool CanDash() const;
  bool CanParry() const;
  bool IsParrying() const;
  bool IsInvincible() const;

  int Money() const { return m_Money; }
  int HumansDestroyedCounter() const { return m_HumansDestroyedCounter; }
  void AddMoney(int value) { m_Money += value; }
  void AddHumanDestroyed() { ++m_HumansDestroyedCounter; }

  void Slapped();
  bool IsDead() const;

  void Reset();
  void SetBuzzAsset(Sound buzz) { m_Buzz = buzz; }

private:
  Vector2 m_position;
  Vector2 m_velocity; // Tracks speed and direction dynamically

  bool m_Dead = false;
  bool m_CanBuzz = false;
  int m_Money = 0;
  int m_HumansDestroyedCounter = 0;

  bool m_CanDash = true;
  bool m_Dashing = false;
  float m_DashCooldown = 0.7f;
  float m_DashDuration = 0.45f;
  float m_DashTimer = 0.0f;
  Vector2 m_DashDirection = Vector2Zero();

  bool m_CanParry = true;
  bool m_Parrying = false;
  float m_ParryCooldown = 0.7f;
  float m_ParryDuration = 0.3f;
  float m_ParryTimer = 0.0f;

  Sound m_Buzz;

  const float ACCELERATION = 80.0f; // How fast the ball gains speed
  const float FRICTION = 40.0f;     // How fast the ball glides to a stop
  const float MAX_SPEED = 4.0f;     // Your speed cap
  const float COLLISION_RADIUS = 15.0f;

  float currentMaxSpeed = MAX_SPEED;
};
