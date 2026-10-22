#include "main_menu.h"

#include <raylib.h>

#include "button.h"
#include "fly.h"
#include "screen_manager.h"

void MainMenu::OnEnter(Fly &fly) { 
    m_StartButton.Reset();

}

void MainMenu::OnExit(Fly& fly) {}

void MainMenu::Draw(const Fly &fly) const {
  ClearBackground(Color{190, 190, 190, 255});

  DrawTextEx(ScreenManager::Get().font(), "FlyState", {32, 32}, 64, 0,
             DARKGRAY);

  float start = 114;
  DrawTextEx(ScreenManager::Get().font(), "Q - Parry", {32, start}, 32, 0,
             GRAY);

  DrawTextEx(ScreenManager::Get().font(), "Left Shift - Dash", {32, start + 40},
             32, 0, GRAY);

  DrawTextEx(ScreenManager::Get().font(), "Space Bar - Select/Buzz",
             {32, start + 80}, 32, 0, GRAY);

  DrawTextEx(ScreenManager::Get().font(), "WASD - Movement", {32, start + 120},
             32, 0, GRAY);

  m_StartButton.Draw();
  fly.Draw();
}

void MainMenu::Update(Fly &fly) {
  fly.Update();

  m_StartButton.Update(fly);
}

Button MainMenu::GetStartButton() {
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
                WHITE,
                MainMenu::StartButtonOnClick,
                RED,
                BLUE};
}

void MainMenu::StartButtonOnClick() {
  ScreenManager::Get().SwitchScenario(ScenarioType::LevelSelect);
}
