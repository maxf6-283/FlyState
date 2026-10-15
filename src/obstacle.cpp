#include "obstacle.h"

#include <raylib.h>
#include <raymath.h>

#include <cmath>

Vector2 Obstacle::CollisionOffset(Vector2 pos, float rad) const {
  const Vector2 clamped_pos =
      Vector2Clamp(pos, {m_Area.x, m_Area.y},
                   {m_Area.x + m_Area.width, m_Area.y + m_Area.height});

  const Vector2 clamped_displacement = pos - clamped_pos;
  const float len_sq = Vector2LengthSqr(clamped_displacement);
  if (len_sq > rad * rad || len_sq < 1e-6) {
    return Vector2{0, 0};
  }
  const float len = std::sqrt(len_sq);
  return clamped_displacement / len * (rad - len);
}

void Obstacle::Draw() const {
  DrawTexture(m_Image, m_Position.x, m_Position.y, WHITE);
}