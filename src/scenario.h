#pragma once

enum class ScenarioType {
  MainMenu,
  LevelSelect,
  LevelCubicle,
  LevelConference,
  LevelBoss,
};

class Scenario {
 public:
  virtual void OnEnter() = 0;
  virtual void OnExit() = 0;

  virtual void Draw() const = 0;
  virtual void Update() = 0;
};