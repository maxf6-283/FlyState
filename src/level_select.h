#pragma once

#include <raylib.h>

#include "scenario.h"

class LevelSelect : public Scenario {
 public:
  LevelSelect() {}

  virtual void OnEnter();
  virtual void OnExit();

  virtual void Draw();
  virtual void Update();
};