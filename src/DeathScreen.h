#pragma once

#include "button.h"
#include "scenario.h"

class DeathScreen : public Scenario {
public:
  DeathScreen() : m_RevengeButton(GetRevengeButton()) {}

  void OnEnter(Fly &fly) override;
  void OnExit(Fly &fly) override;

  void Draw(const Fly &fly) const override;
  void Update(Fly &fly) override;

private:
  Button m_RevengeButton;

  static Button GetRevengeButton();
  static void RevengeButtonOnClick();
};
