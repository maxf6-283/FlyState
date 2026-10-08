#pragma once

class Scenario {
 public:
  virtual void OnEnter() = 0;
  virtual void OnExit() = 0;

  virtual void Draw() = 0;
  virtual void Update() = 0;
};