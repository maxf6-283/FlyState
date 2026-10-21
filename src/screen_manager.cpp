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
  case ScenarioType::DeathScreen:
    return &m_DeathScreen;
  case ScenarioType::EndScreen:
    return &m_EndScreen;
  case ScenarioType::LevelSelect:
    return &m_LevelSelect;
  default:
    for (Level &level : m_Levels) {
      if (level.scenario_type() == sceneType) {
        return &level;
      }
    }
    throw std::invalid_argument("Invalid scenario type!");
  }
}

ScreenManager::ScreenManager() { LoadAllAssets(); }

void ScreenManager::LoadAllAssets() {
  m_Assets.LoadAsset("assets/buzzsfx.wav");
  m_Assets.LoadAsset("assets/type.wav");
  m_Assets.LoadAsset("assets/verity.png");
  m_Assets.LoadAsset("assets/parry.wav");

  // Rasterize the font above the largest UI size we currently use. Loading
  // the default 32px atlas and scaling it up makes the text visibly soft.
  m_Font = LoadFontEx(
      "assets/Libre_Caslon_Condensed/static/LibreCaslonCondensed-Regular.ttf",
      96, nullptr, 0);
  SetTextureFilter(m_Font.texture, TEXTURE_FILTER_BILINEAR);

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
  Texture2D thumbnail_texture = LoadTextureFromImage(thumbnail_img);
  Texture2D thumbnail2_texture = LoadTextureFromImage(thumbnail2_img);
  Texture2D thumbnail3_texture = LoadTextureFromImage(thumbnail3_img);

  RenderTexture2D bg = LoadRenderTexture(2000, 1000);
  RenderTexture2D bg2 = LoadRenderTexture(3000, 1000);
  RenderTexture2D bg3 = LoadRenderTexture(1000, 1000);

  // clang-format off
  BeginTextureMode(bg);
    ClearBackground(GRAY);
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

  m_Assets.AddAsset("bg1", bg.texture);
  m_Assets.AddAsset("bg2", bg2.texture);
  m_Assets.AddAsset("bg3", bg3.texture);
  m_Assets.AddAsset("obstacle", obs_img.texture);

  Obstacle test_obs = Obstacle({300, 400, 300, 100}, RED);
  Obstacle test_obs2 = Obstacle(Rectangle{450, 300, 100, 100}, bg3.texture);
  Obstacle destroyable_obs = Obstacle(Rectangle{1200, 300, 50, 50}, WHITE, 6,
                                      100, ObstacleType::Destructible);

  m_Levels.push_back(Level("Dummy Level", ScenarioType::LevelConference,
                           thumbnail_img, thumbnail_texture, "bg1",
                           {test_obs, test_obs2, destroyable_obs}, &m_Assets));
  m_Levels.push_back(Level("Dummy Level 2", ScenarioType::LevelCubicle,
                           thumbnail2_img, thumbnail2_texture, "bg2", {},
                           &m_Assets));
  m_Levels.push_back(Level("Dummy Level 3", ScenarioType::LevelBoss,
                           thumbnail3_img, thumbnail3_texture, "bg3", {},
                           &m_Assets));
}
