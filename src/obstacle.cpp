#include "obstacle.h"

#include <raylib.h>
#include <raymath.h>

#include <cmath>
#include <algorithm>

Vector2 Obstacle::CollisionOffset(Vector2 pos, float rad) const {
  if (IsDestroyed()) {
    return Vector2Zero();
  }
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
  if (m_State == ObstacleState::Destroyed) {
    return;
  }

  constexpr float cracked_start = 0.66f;
  const float progress = static_cast<float>(m_CurrentHitCounter) /
                         static_cast<float>(std::max(m_Hits, 1));
  Color tint = WHITE;
  if (progress < cracked_start) {
    tint = ColorLerp(WHITE, ORANGE, progress / cracked_start);
  } else {
    // Keep the cracked tint as the midpoint, then continue toward the
    // destroyed tint for the remaining hits.
    const float destroyed_progress =
        (progress - cracked_start) / (1.0f - cracked_start);
    tint = ColorLerp(ORANGE, RED, destroyed_progress);
  }
  if (m_IsColored) {
    DrawRectangleRec(m_Area, ColorTint(m_Color, tint));
  } else {
    DrawTextureRec(m_Image, m_Area, m_Position, tint);
  }
}

void Obstacle::IncrementHitCounter() {
  if (m_Type == ObstacleType::Indestructible || IsDestroyed()) {
    return;
  }
  m_CurrentHitCounter = std::min(m_CurrentHitCounter + 1, m_Hits);
  const float progress = static_cast<float>(m_CurrentHitCounter) /
                         static_cast<float>(std::max(m_Hits, 1));
  m_State = progress >= 1.0f ? ObstacleState::Destroyed
           : progress >= 0.66f ? ObstacleState::Cracked
                               : ObstacleState::Healthy;
}

void Obstacle::Reset() {
  m_CurrentHitCounter = 0;
  m_State = ObstacleState::Healthy;
}
