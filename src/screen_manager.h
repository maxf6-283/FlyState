#pragma once

#include <cstdlib>
#include <vector>

#include "AssetMap.h"
#include "DeathScreen.h"
#include "EndScreen.h"
#include "fly.h"
#include "level.h"
#include "level_select.h"
#include "main_menu.h"
#include "raylib.h"
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
  const AssetMap &assets() const { return m_Assets; }
  Font font() const { return m_Font; }

  static ScreenManager &Get() {
    static ScreenManager instance{};
    return instance;
  }

private:
  ScreenManager();
  void LoadAllAssets();

  Scenario *GetScenario(ScenarioType sceneType);

  Fly m_Fly{Vector2{static_cast<float>(GetRandomValue(0, GetScreenWidth())),
                    static_cast<float>(GetRandomValue(0, GetScreenHeight()))}};

  Scenario *m_Scenario = &m_MainMenu;

  MainMenu m_MainMenu{};
  DeathScreen m_DeathScreen{};
  EndScreen m_EndScreen{};
  LevelSelect m_LevelSelect{};

  std::vector<Level> m_Levels;
  AssetMap m_Assets;
  Font m_Font{};
};
