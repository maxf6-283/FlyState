#pragma once

#include "button.h"
#include "scenario.h"

class EndScreen : public Scenario {
public:
  EndScreen() : m_ReplayButton(GetReplayButton()) {}

  void OnEnter(Fly &fly) override;
  void OnExit(Fly &fly) override;

  void Draw(const Fly &fly) const override;
  void Update(Fly &fly) override;

private:
  Button m_ReplayButton;

  static Button GetReplayButton();
  static void ReplayButtonOnClick();
};
