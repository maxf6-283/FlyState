#pragma once

#include <string>
#include <vector>

#include "button.h"
#include "scenario.h"

class LevelSelect : public Scenario {
public:
  LevelSelect() = default;

  void OnEnter(Fly& fly) override;
  void OnExit(Fly& fly) override;

  void Draw(const Fly& fly) const override;
  void Update(Fly& fly) override;

 private:
  struct LevelButton {
    Button button;
    std::string name;
  };

  std::vector<LevelButton> m_LevelButtons{};
};
