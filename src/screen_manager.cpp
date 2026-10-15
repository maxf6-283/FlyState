#include "screen_manager.h"

#include <stdexcept>

#include "level.h"
#include "obstacle.h"
#include "raylib.h"
#include "scenario.h"

void ScreenManager::Draw() const { m_Scenario->Draw(m_Fly); }

void ScreenManager::Update() { m_Scenario->Update(m_Fly); }

void ScreenManager::SwitchScenario(ScenarioType sceneType) {
  m_Scenario->OnExit(m_Fly);
  m_Scenario = GetScenario(sceneType);
  m_Scenario->OnEnter(m_Fly);
}

Scenario *ScreenManager::GetScenario(ScenarioType sceneType) {
  switch (sceneType) {
  case ScenarioType::MainMenu:
    return &m_MainMenu;
  case ScenarioType::LevelSelect:
    return &m_LevelSelect;
  default:
    for (Level& level : m_Levels) {
      if (level.scenario_type() == sceneType) {
        return &level;
      }
    }
    throw std::invalid_argument("Invalid scenario type!");
  }
}

ScreenManager::ScreenManager() {
  // dummy level
  RenderTexture2D thumbnail = LoadRenderTexture(100, 100);
  RenderTexture2D thumbnail2 = LoadRenderTexture(100, 100);

  // clang-format off
  BeginTextureMode(thumbnail);
    ClearBackground(ORANGE);
  EndTextureMode();
  BeginTextureMode(thumbnail2);
    ClearBackground(LIME);
  EndTextureMode();
  // clang-format on

  Image thumbnail_img = LoadImageFromTexture(thumbnail.texture);
  Image thumbnail2_img = LoadImageFromTexture(thumbnail2.texture);
  Image thumbnail3_img = LoadImage("assets/verity.png");

  RenderTexture2D bg = LoadRenderTexture(2000, 1000);
  RenderTexture2D bg2 = LoadRenderTexture(3000, 1000);
  RenderTexture2D bg3 = LoadRenderTexture(1000, 1000);

  // clang-format off
  BeginTextureMode(bg);
    ClearBackground(ORANGE);
    DrawRectangle(0, 0, 1000, 500, YELLOW);
    DrawRectangle(0, 500, 1000, 500, PINK);
  EndTextureMode();
  BeginTextureMode(bg2);
    ClearBackground(LIME);
  EndTextureMode();
  BeginTextureMode(bg3);
    ClearBackground(YELLOW);
  EndTextureMode();
  // clang-format on

  RenderTexture2D obs_img = LoadRenderTexture(300, 100);
  BeginTextureMode(obs_img);
  ClearBackground(RED);
  EndTextureMode();

  Obstacle test_obs =
      Obstacle({300, 300, 300, 100}, {300, 300}, obs_img.texture);

  m_Levels.push_back(Level("Dummy Level", ScenarioType::LevelConference,
                           thumbnail_img, bg.texture, {test_obs}));
  m_Levels.push_back(Level("Dummy Level 2", ScenarioType::LevelCubicle,
                           thumbnail2_img, bg2.texture, {}));
  m_Levels.push_back(Level("Dummy Level 3", ScenarioType::LevelBoss,
                           thumbnail3_img, bg3.texture, {}));
}