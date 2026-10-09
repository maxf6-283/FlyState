#include "button.h"

void Button::Update() {
  Vector2 mousePos = GetMousePosition();
  if (CheckCollisionPointRec(mousePos, m_Area)) {
    if (m_Holding) {
      if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        // we have released
        m_Holding = false;
        m_Hovering = true;
        m_OnClick();
      }
    } else if (m_Hovering) {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        m_Holding = true;
        m_Hovering = false;
      }
    } else {
      if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        m_Hovering = true;
      }
    }
  } else {
    m_Hovering = false;
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      m_Holding = false;
    }
  }
}

void Button::Draw() {
  if (m_Holding) {
    DrawTexture(m_Held, m_Position.x, m_Position.y, Color{255, 255, 255, 255});
  } else if (m_Hovering) {
    DrawTexture(m_Hovered, m_Position.x, m_Position.y,
                Color{255, 255, 255, 255});
  } else {
    DrawTexture(m_Image, m_Position.x, m_Position.y, Color{255, 255, 255, 255});
  }
}

void Button::Reset() {
  m_Holding = false;
  m_Hovering = false;
}
