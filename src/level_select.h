#pragma once

#include <raylib.h>

#include "scenario.h"

class LevelSelect : public Scenario {
public:
  LevelSelect() = default;

  void OnEnter() override;
  void OnExit() override;

  void Draw() override;
  void Update() override;
};
