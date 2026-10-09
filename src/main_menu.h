#pragma once

#include "button.h"
#include "scenario.h"

class MainMenu : public Scenario {
public:
  MainMenu() : m_StartButton(GetStartButton()) {}

  virtual void OnEnter();
  virtual void OnExit();

  virtual void Draw();
  virtual void Update();

private:
  Button m_StartButton;

  static Button GetStartButton();
  static void StartButtonOnClick();
};
