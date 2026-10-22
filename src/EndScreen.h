#pragma once

#include "button.h"
#include "scenario.h"

class EndScreen : public Scenario {
public:
  EndScreen()
      : m_ReplayButton(GetReplayButton()),
        m_LevelSelectButton(GetLevelSelectButton()) {}

  void OnEnter(Fly &fly) override;
  void OnExit(Fly &fly) override;

  void Draw(const Fly &fly) const override;
  void Update(Fly &fly) override;

private:
  Button m_ReplayButton;
  Button m_LevelSelectButton;

  static Button MakeButton(int buttonX, void (*onClick)());
  static Button GetReplayButton();
  static Button GetLevelSelectButton();
  static void ReplayButtonOnClick();
  static void LevelSelectButtonOnClick();
};
