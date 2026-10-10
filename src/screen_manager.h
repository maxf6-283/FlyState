#pragma once

#include "level_select.h"
#include "main_menu.h"
#include "scenario.h"

enum class ScenarioType {
  MainMenu,
  LevelSelect,
  LevelCubicle,
  LevelConference,
  LevelBoss,
};

class ScreenManager {
public:
  ScreenManager(const ScreenManager &) = delete;
  ScreenManager &operator=(const ScreenManager &) = delete;
  ScreenManager(ScreenManager &&) = delete;
  ScreenManager &operator=(ScreenManager &&) = delete;

  void Update();
  void Draw();

  void SwitchScenario(ScenarioType sceneType);

  static ScreenManager &Get() {
    static ScreenManager instance{};
    return instance;
  }

private:
  ScreenManager() {}

  Scenario *GetScenario(ScenarioType sceneType);

  Scenario *m_Scenario = &m_MainMenu;

  MainMenu m_MainMenu{};
  LevelSelect m_LevelSelect{};
};
