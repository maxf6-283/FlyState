#include "main_menu.h"

#include <raylib.h>

#include "button.h"
#include "screen_manager.h"

void MainMenu::OnEnter() { m_StartButton.Reset(); }

void MainMenu::OnExit() {}

void MainMenu::Draw() {
  ClearBackground(Color{190, 190, 190, 255});
  DrawText("FlyState Game Jam", 32, 32, 28, DARKGRAY);
  DrawText("Start working on the game!", 32, 72, 20, GRAY);

  m_StartButton.Draw();
}

void MainMenu::Update() { m_StartButton.Update(); }

Button MainMenu::GetStartButton() {
  RenderTexture2D img = LoadRenderTexture(200, 100);
  RenderTexture2D hover = LoadRenderTexture(200, 100);
  RenderTexture2D hold = LoadRenderTexture(200, 100);

  // clang-format off
  BeginTextureMode(img);
    ClearBackground(RED);
  EndTextureMode();
  BeginTextureMode(hover);
    ClearBackground(GREEN);
  EndTextureMode();
  BeginTextureMode(hold);
    ClearBackground(BLUE);
  EndTextureMode();
  // clang-format on

  return Button{Rectangle{
                    .x = 380,
                    .y = 400,
                    .width = 200,
                    .height = 100,
                },
                Vector2{
                    .x = 380,
                    .y = 400,
                },
                img.texture,
                hover.texture,
                hold.texture,
                MainMenu::StartButtonOnClick};
}

void MainMenu::StartButtonOnClick() {
  TraceLog(LOG_INFO, "Start button pressed!");
  ScreenManager::Get().SwitchScenario(ScenarioType::LevelSelect);
}
