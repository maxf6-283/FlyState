#include "DeathScreen.h"

#include "raylib.h"
#include "screen_manager.h"

void DeathScreen::OnEnter(Fly &fly) { m_RevengeButton.Reset(); }

void DeathScreen::OnExit(Fly &fly) { fly.Reset(); }

void DeathScreen::Draw(const Fly &fly) const {
  ClearBackground(Color{190, 190, 190, 255});

  const char *message = "You have been squashed!";
  const int messageFontSize = 48;
  const int messageWidth = MeasureTextEx(ScreenManager::Get().font(), message,
                                         messageFontSize, 0).x;
  DrawTextEx(ScreenManager::Get().font(), message,
             {static_cast<float>((GetScreenWidth() - messageWidth) / 2), 220},
             messageFontSize, 0, DARKGRAY);

  m_RevengeButton.Draw();
  const char *label = "Revenge";
  constexpr int fontSize = 32;
  const int labelWidth = MeasureTextEx(ScreenManager::Get().font(), label,
                                      fontSize, 0).x;
  DrawTextEx(ScreenManager::Get().font(), label,
             {static_cast<float>((GetScreenWidth() - labelWidth) / 2), 350},
             fontSize, 0, DARKGRAY);
  fly.Draw();
}

void DeathScreen::Update(Fly &fly) {
  fly.Update();
  m_RevengeButton.Update(fly);
}

Button DeathScreen::GetRevengeButton() {
  constexpr int buttonWidth = 240;
  constexpr int buttonHeight = 90;
  const int buttonX = (GetScreenWidth() - buttonWidth) / 2;
  const int buttonY = 320;

  return Button{
      Rectangle{static_cast<float>(buttonX), static_cast<float>(buttonY),
                static_cast<float>(buttonWidth),
                static_cast<float>(buttonHeight)},
      Vector2{static_cast<float>(buttonX), static_cast<float>(buttonY)},
      WHITE,
      DeathScreen::RevengeButtonOnClick,
      RED, BLUE};
}

void DeathScreen::RevengeButtonOnClick() {
  ScreenManager::Get().SwitchScenario(ScenarioType::MainMenu);
}
