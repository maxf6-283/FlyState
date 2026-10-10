#pragma once

#include <vector>

#include "level.h"
#include "level_select.h"
#include "main_menu.h"
#include "scenario.h"

class ScreenManager {
public:
  ScreenManager(const ScreenManager &) = delete;
  ScreenManager &operator=(const ScreenManager &) = delete;
  ScreenManager(ScreenManager &&) = delete;
  ScreenManager &operator=(ScreenManager &&) = delete;

  void Update();
  void Draw() const;

  void SwitchScenario(ScenarioType sceneType);

  const std::vector<Level> &levels() const { return m_Levels; }

  static ScreenManager &Get() {
    static ScreenManager instance{};
    return instance;
  }

private:
 ScreenManager();

 Scenario *GetScenario(ScenarioType sceneType);

 Scenario *m_Scenario = &m_MainMenu;

 MainMenu m_MainMenu{};
 LevelSelect m_LevelSelect{};

 std::vector<Level> m_Levels;
};
