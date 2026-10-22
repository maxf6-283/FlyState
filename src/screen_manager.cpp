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
  m_Assets.LoadAsset("assets/Computer.png");
  m_Assets.LoadAsset("assets/BasicBackground2.png");

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

  // Sprite Asset Textures
  Texture2D computer = std::get<Texture2D>(m_Assets.GetAsset("Computer").value());
  //Texture2D cubicle2 = std::get<Texture2D>(m_Assets.GetAsset("ForegroundCubicle2").value());

  // clang-format off
  BeginTextureMode(bg);
    ClearBackground(ColorBrightness(RAYWHITE, 0.7));
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

  Level conference_level("bg1", thumbnail_texture, "Level 1",
                         ScenarioType::LevelConference, &m_Assets);

  Vector2 screenCenter = Vector2(bg.texture.width / 2, bg.texture.height / 2);

  conference_level.SetFlySpawn(screenCenter);

  conference_level.AddHuman({680.0f, 400.0f}, false);
  conference_level.AddObstacle({{0, 600, 20000, 800}, GRAY});
  conference_level.AddObstacle({{300, 400, 300, 100}, GRAY});
  conference_level.AddObstacle({{300, 400, 50, 300}, GRAY});
  conference_level.AddObstacle({{500, 400, 50, 300}, GRAY});
  conference_level.AddObstacle({{450, 210, 96, 192}, computer, 3, 50,
                                ObstacleType::Destructible});
  conference_level.AddObstacle({Rectangle{1200, 300, 50, 100}, GOLD, 6, 100,
                                ObstacleType::Destructible});
  conference_level.AddObstacle({Rectangle{1200, 400, 50, 500}, GRAY, 6, 100});

  m_Levels.push_back(conference_level);

  Level cubicle_level("bg1", thumbnail2_texture, "Level 2",
                      ScenarioType::LevelCubicle, &m_Assets);

  cubicle_level.AddHuman({195.0f, 400.0f}, true); //left npc
  cubicle_level.AddHuman({660.0f, 400.0f}, false); //right npc

  cubicle_level.SetFlySpawn(Vector2(1000, 400));

  cubicle_level.AddObstacle({{0, 600, 20000, 800}, GRAY}); //Floor
  cubicle_level.AddObstacle({{250, 425, 350, 100}, BROWN}); //desk top
  cubicle_level.AddObstacle({{300, 425, 50, 175}, BROWN});  //desk right leg
  cubicle_level.AddObstacle({{500, 425, 50, 175}, BROWN});  //desk left leg

  cubicle_level.AddObstacle({{450, 225, 50, 200}, BLACK, 4, 100,
                            ObstacleType::Destructible}); //right computer
  cubicle_level.AddObstacle({ {360, 225, 50, 200}, BLACK, 4, 100,
                            ObstacleType::Destructible }); //left computer
  cubicle_level.AddObstacle({ {260, 415, 50, 10}, BLACK, 2, 50,
                            ObstacleType::Destructible }); //right keyboard
  cubicle_level.AddObstacle({ {540, 415, 50, 10}, BLACK, 2, 50,
                            ObstacleType::Destructible }); //left keyboard

  cubicle_level.AddObstacle({ {1500, 400, 125, 200}, BLUE, 6, 200,
                            ObstacleType::Destructible}); //Water dispenser



  m_Levels.push_back(cubicle_level);

  Level boss_level("bg3", thumbnail3_texture, "Dummy Level 3",
                   ScenarioType::LevelBoss, &m_Assets);
  boss_level.AddHuman({680.0f, 400.0f}, false);
  m_Levels.push_back(boss_level);
}
