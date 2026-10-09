#pragma once

#include <functional>
#include <raylib.h>

class Button {
public:
  Button(Rectangle area, Vector2 pos, Texture2D img, Texture2D hovered,
         Texture2D held, const std::function<void()> onClick)
      : m_Area(area), m_Position(pos), m_Image(img), m_Hovered(hovered),
        m_Held(held), m_OnClick(onClick) {}

  void Update();

  void Draw();

  void Reset();

private:
  bool m_Hovering;
  bool m_Holding;

  Rectangle m_Area;
  Vector2 m_Position;

  Texture2D m_Image;
  Texture2D m_Hovered;
  Texture2D m_Held;

  std::function<void()> m_OnClick;
};
