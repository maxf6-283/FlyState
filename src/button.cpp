#include "button.h"

void Button::Update(const Fly& fly) {
  const Vector2 flyPos = fly.position();
  if (CheckCollisionPointRec(flyPos, m_Area)) {
    if (m_Holding) {
      if (!IsKeyDown(KEY_SPACE)) {
        // we have released
        m_Holding = false;
        m_Hovering = true;
        m_OnClick();
      }
    } else if (m_Hovering) {
      if (IsKeyDown(KEY_SPACE)) {
        m_Holding = true;
        m_Hovering = false;
      }
    } else {
      if (!IsKeyDown(KEY_SPACE)) {
        m_Hovering = true;
      }
    }
  } else {
    m_Hovering = false;
    if (!IsKeyDown(KEY_SPACE)) {
      m_Holding = false;
    }
  }
}

void Button::Draw() const {
  if (m_Holding) {
    DrawTexture(m_Held, m_Position.x, m_Position.y, m_HeldTint);
  } else if (m_Hovering) {
    DrawTexture(m_Hovered, m_Position.x, m_Position.y, m_HoveredTint);
  } else {
    DrawTexture(m_Image, m_Position.x, m_Position.y, m_ImageTint);
  }
}

void Button::Reset() {
  m_Holding = false;
  m_Hovering = false;
}
