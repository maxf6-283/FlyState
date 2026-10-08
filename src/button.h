#pragma once

#include <raylib.h>

class Button {
 public:
  Button(Rectangle area, Vector2 loc, Texture2D img, Texture2D hovered,
         Texture2D held, void (*on_click)())
      : area_(area),
        location_(loc),
        img_(img),
        hovered_(hovered),
        held_(held),
        on_click_(on_click) {}

  void Update();

  void Draw();

 private:
  bool hovering_;
  bool holding_;

  const Rectangle area_;
  const Vector2 location_;

  const Texture2D img_;
  const Texture2D hovered_;
  const Texture2D held_;

  void (*const on_click_)();
};
