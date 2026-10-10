#pragma once

#include <string>
#include <vector>

#include "button.h"
#include "scenario.h"

class LevelSelect : public Scenario {
public:
  LevelSelect() = default;

  void OnEnter() override;
  void OnExit() override;

  void Draw() const override;
  void Update() override;

 private:
  struct LevelButton {
    Button button;
    std::string name;
  };

  std::vector<LevelButton> m_LevelButtons{};
};
