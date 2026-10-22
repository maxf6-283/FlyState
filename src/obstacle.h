#pragma once

#include <raylib.h>

enum class ObstacleState { Healthy, Cracked, Destroyed };
enum class ObstacleType { Indestructible, Destructible };

class Obstacle {
public:
  Obstacle(Rectangle area, Texture2D img,
           int hits = 1, int value = 0,
           ObstacleType type = ObstacleType::Indestructible)
      : m_Area(area), m_Position({area.x, area.y}), m_Image(img),
        m_Hits(hits), m_Value(value), m_Type(type) {}

  Obstacle(Rectangle area, Color color,
           int hits = 1, int value = 0,
           ObstacleType type = ObstacleType::Indestructible)
      : m_Area(area), m_Position({area.x, area.y}), m_Color(color),
        m_Hits(hits), m_Value(value), m_Type(type), m_IsColored(true) {}

  Vector2 CollisionOffset(Vector2 pos, float rad) const;

  void Draw() const;

  void Reset();
  void IncrementHitCounter();

  Vector2 position() const { return m_Position; }

  Rectangle area() const { return m_Area; }
  int value() const { return m_Value; }
  int currentHitCounter() const { return m_CurrentHitCounter; }
  int hits() const { return m_Hits; }
  ObstacleState state() const { return m_State; }
  bool IsDestroyed() const { return m_State == ObstacleState::Destroyed; }
  bool IsDestructible() const {
    return m_Type == ObstacleType::Destructible;
  }

private:
  Rectangle m_Area;
  Vector2 m_Position;

  Texture2D m_Image;
  Color m_Color = WHITE;
  int m_Hits = 1;
  int m_CurrentHitCounter = 0;
  int m_Value = 0;
  ObstacleState m_State = ObstacleState::Healthy;
  ObstacleType m_Type = ObstacleType::Indestructible;
  bool m_IsColored = false;
};
