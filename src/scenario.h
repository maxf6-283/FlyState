#pragma once

#include "fly.h"

enum class ScenarioType {
  MainMenu,
  LevelSelect,
  LevelCubicle,
  LevelConference,
  LevelBoss,
};

class Scenario {
 public:
  virtual void OnEnter(Fly& fly) = 0;
  virtual void OnExit(Fly& fly) = 0;

  virtual void Draw(const Fly& fly) const = 0;
  virtual void Update(Fly& fly) = 0;
};