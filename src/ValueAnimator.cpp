#include "ValueAnimator.h"

#include <algorithm>
#include <string>

void ValueAnimator::AddValue(int value, Vector2 position) {
  m_Values.push_back({value, position});
}

void ValueAnimator::Update() {
  const float frameTime = GetFrameTime();
  for (ValueText &value : m_Values) {
    value.m_Timer += frameTime;
    value.m_Position.y -= m_RiseHeight * frameTime / m_TimerDuration;
  }

  m_Values.erase(
      std::remove_if(m_Values.begin(), m_Values.end(), [](const ValueText &value) {
        return value.m_Timer >= m_TimerDuration;
      }),
      m_Values.end());
}

void ValueAnimator::Draw() const {
  for (const ValueText &value : m_Values) {
    const float progress = std::clamp(value.m_Timer / m_TimerDuration, 0.0f, 1.0f);
    const unsigned char opacity =
        static_cast<unsigned char>((1.0f - progress) * 255.0f);
    const Color textColor{RED.r, RED.g, RED.b, opacity};
    const std::string text = "-" + std::to_string(value.m_Value);
    DrawText(text.c_str(), static_cast<int>(value.m_Position.x),
             static_cast<int>(value.m_Position.y), 20, textColor);
  }
}

void ValueAnimator::Reset() { m_Values.clear(); }
