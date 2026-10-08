#pragma once

#include <raylib.h>

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
  ScreenManager(const ScreenManager&) = delete;
  ScreenManager& operator=(const ScreenManager&) = delete;
  ScreenManager(ScreenManager&&) = delete;
  ScreenManager& operator=(ScreenManager&&) = delete;

  void Update();
  void Draw();

  void SwitchScenario(ScenarioType scen_type);

  static ScreenManager& Get() {
    static ScreenManager instance{};
    return instance;
  }

 private:
  ScreenManager() {}

  Scenario& GetScenario(ScenarioType scen_type);

  Scenario& scenario_ = main_menu_;

  MainMenu main_menu_{};
};