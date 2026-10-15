#pragma once

#include <raylib.h>

#include <vector>

#include "obstacle.h"

class Fly {
 public:
  Fly(Vector2 pos);

  void Update(const std::vector<Obstacle>& obstacles = {});
  void Draw() const;

  Vector2 position() const { return m_position; }
  Vector2 velocity() const { return m_velocity; }
  float radius() const { return COLLISION_RADIUS; }

  void Offset(Vector2 pos);
  void SetPos(Vector2 pos);
  void SetVel(Vector2 vel);

 private:
  Vector2 m_position;
  Vector2 m_velocity;  // Tracks speed and direction dynamically

  const float ACCELERATION = 80.0f;  // How fast the ball gains speed
  const float FRICTION = 40.0f;      // How fast the ball glides to a stop
  const float MAX_SPEED = 8.0f;      // Your speed cap
  const float COLLISION_RADIUS = 15.0f;
};
