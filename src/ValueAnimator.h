#pragma once

#include <raylib.h>

#include <vector>

class ValueAnimator {
public:
  void AddValue(int value, Vector2 position);
  void Update();
  void Draw() const;
  void Reset();

private:
  struct ValueText {
    int m_Value;
    Vector2 m_Position;
    float m_Timer = 0.0f;
  };

  static constexpr float m_TimerDuration = 1.0f;
  static constexpr float m_RiseHeight = 80.0f;

  std::vector<ValueText> m_Values;
};
