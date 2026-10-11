#include "screen_manager.h"

#include <stdexcept>

#include "level.h"
#include "raylib.h"
#include "scenario.h"

void ScreenManager::Draw() const {
  m_Scenario->Draw();
  m_Fly.Draw();
}

void ScreenManager::Update() {
  m_Fly.Update();
  m_Scenario->Update(m_Fly);
}

void ScreenManager::SwitchScenario(ScenarioType sceneType) {
  m_Scenario->OnExit();
  m_Scenario = GetScenario(sceneType);
  m_Scenario->OnEnter();
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

  RenderTexture2D bg = LoadRenderTexture(500, 500);
  RenderTexture2D bg2 = LoadRenderTexture(500, 500);
  RenderTexture2D bg3 = LoadRenderTexture(500, 500);

  // clang-format off
  BeginTextureMode(bg);
    ClearBackground(ORANGE);
  EndTextureMode();
  BeginTextureMode(bg2);
    ClearBackground(LIME);
  EndTextureMode();
  BeginTextureMode(bg3);
    ClearBackground(YELLOW);
  EndTextureMode();
  // clang-format on

  m_Levels.push_back(Level("Dummy Level", ScenarioType::LevelConference,
                           thumbnail_img, bg.texture));
  m_Levels.push_back(Level("Dummy Level 2", ScenarioType::LevelCubicle,
                           thumbnail2_img, bg2.texture));
  m_Levels.push_back(Level("Dummy Level 3", ScenarioType::LevelBoss,
                           thumbnail3_img, bg3.texture));
}