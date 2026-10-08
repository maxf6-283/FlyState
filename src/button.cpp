#include "button.h"

#include "raylib.h"

void Button::Update() {
  Vector2 mouse_pos = GetMousePosition();
  if (CheckCollisionPointRec(mouse_pos, area_)) {
    if (holding_) {
      if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        // we have released
        holding_ = false;
        hovering_ = true;
        on_click_();
      }
    } else if (hovering_) {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        holding_ = true;
        hovering_ = false;
      }
    } else {
      if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        hovering_ = true;
      }
    }
  } else {
    hovering_ = false;
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      holding_ = false;
    }
  }
}

void Button::Draw() {
  if (holding_) {
    DrawTexture(held_, location_.x, location_.y, Color{255, 255, 255, 255});
  } else if (hovering_) {
    DrawTexture(hovered_, location_.x, location_.y, Color{255, 255, 255, 255});
  } else {
    DrawTexture(img_, location_.x, location_.y, Color{255, 255, 255, 255});
  }
}