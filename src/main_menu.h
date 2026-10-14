#pragma once

#include "button.h"
#include "scenario.h"

class MainMenu : public Scenario {
public:
  MainMenu() : m_StartButton(GetStartButton()) {}

  virtual void OnEnter(Fly& fly) override;
  virtual void OnExit(Fly& fly) override;

  virtual void Draw(const Fly& fly) const override;
  virtual void Update(Fly& fly) override;

 private:
  Button m_StartButton;

  static Button GetStartButton();
  static void StartButtonOnClick();
};
