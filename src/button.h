#pragma once

#include <raylib.h>

#include <functional>

#include "fly.h"

class Button {
public:
 Button(Rectangle area, Vector2 pos, Texture2D img, Texture2D hovered,
        Texture2D held, const std::function<void()> &onClick,
        Color image_tint = WHITE, Color hovered_tint = WHITE,
        Color held_tint = WHITE)
     : m_Area(area),
       m_Position(pos),
       m_Image(img),
       m_ImageTint(image_tint),
       m_Hovered(hovered),
       m_HoveredTint(hovered_tint),
       m_Held(held),
       m_HeldTint(held_tint),
       m_OnClick(onClick) {}

 void Update(const Fly& fly);

 void Draw() const;

 void Reset();

 const Vector2 position() const { return m_Position; }

 const Rectangle area() const { return m_Area; }

private:
 bool m_Hovering;
 bool m_Holding;

 Rectangle m_Area;
 Vector2 m_Position;

 Texture2D m_Image;
 Color m_ImageTint;
 Texture2D m_Hovered;
 Color m_HoveredTint;
 Texture2D m_Held;
 Color m_HeldTint;

 std::function<void()> m_OnClick;
};
