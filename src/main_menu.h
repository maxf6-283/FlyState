#pragma once

#include "button.h"
#include "scenario.h"

class MainMenu : public Scenario {
public:
  MainMenu() : m_StartButton(GetStartButton()) {}

  virtual void OnEnter() override;
  virtual void OnExit() override;

  virtual void Draw() const override;
  virtual void Update() override;

 private:
  Button m_StartButton;

  static Button GetStartButton();
  static void StartButtonOnClick();
};
