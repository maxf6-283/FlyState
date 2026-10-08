#pragma once

#include <raylib.h>

#include "button.h"
#include "scenario.h"

class MainMenu : public Scenario {
 public:
  MainMenu() : start_button_(GetStartButton()) {}

  virtual void OnEnter();
  virtual void OnExit();

  virtual void Draw();
  virtual void Update();

 private:
  Button start_button_;

  static Button GetStartButton();
  static void StartButtonOnClick();
};