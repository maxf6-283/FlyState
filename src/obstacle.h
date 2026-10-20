#pragma once

#include <raylib.h>

class Obstacle {
public:
  Obstacle(Rectangle area, Texture2D img)
      : m_Area(area), m_Position({area.x, area.y}), m_Image(img) {}

  Vector2 CollisionOffset(Vector2 pos, float rad) const;

  void Draw() const;

  void Reset();

  Vector2 position() const { return m_Position; }

  Rectangle area() const { return m_Area; }

private:
  bool m_Hovering;
  bool m_Holding;

  Rectangle m_Area;
  Vector2 m_Position;

  Texture2D m_Image;
};
