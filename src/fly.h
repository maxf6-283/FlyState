#pragma once 

#include <raylib.h>

class Fly {
 public:
  Fly(Vector2 pos);

  void Update();
  void Draw() const;

  Vector2 position() const { return m_position; };

 private:
  Vector2 m_position;
  Vector2 m_velocity;  // Tracks speed and direction dynamically

  const float ACCELERATION = 80.0f;  // How fast the ball gains speed
  const float FRICTION = 40.0f;       // How fast the ball glides to a stop
  const float MAX_SPEED = 8.0f;      // Your speed cap
};
