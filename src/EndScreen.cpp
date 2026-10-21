#include "EndScreen.h"

#include "raylib.h"
#include "screen_manager.h"

namespace {
void DrawCenteredText(const char *text, float y, float fontSize, Color color) {
  const Font font = ScreenManager::Get().font();
  const float width = MeasureTextEx(font, text, fontSize, 0.0f).x;
  DrawTextEx(font, text,
             {static_cast<float>((GetScreenWidth() - width) / 2.0f), y},
             fontSize, 0.0f, color);
}
} // namespace

void EndScreen::OnEnter(Fly &fly) {
  m_ReplayButton.Reset();
  fly.SetPos({GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f});
  fly.SetVel(Vector2Zero());
}

void EndScreen::OnExit(Fly &fly) { fly.Reset(); }

void EndScreen::Draw(const Fly &fly) const {
  ClearBackground(Color{190, 190, 190, 255});

  const Color textColor = DARKGRAY;
  DrawCenteredText("Thanks for playing", 100.0f, 48.0f, textColor);
  DrawCenteredText("Results", 190.0f, 36.0f, textColor);
  DrawCenteredText(TextFormat("Destruction Caused: $%d", fly.Money()), 250.0f,
                   28.0f, textColor);
  DrawCenteredText(
      TextFormat("Employees Destroyed: %d", fly.HumansDestroyedCounter()),
      290.0f, 28.0f, textColor);

  m_ReplayButton.Draw();
  DrawCenteredText("Replay", GetScreenHeight() - 100.0f, 32.0f, textColor);
  fly.Draw();
}

void EndScreen::Update(Fly &fly) {
  fly.Update();
  m_ReplayButton.Update(fly);
}

Button EndScreen::GetReplayButton() {
  constexpr int buttonWidth = 240;
  constexpr int buttonHeight = 90;
  const int buttonX = (GetScreenWidth() - buttonWidth) / 2;
  const int buttonY = GetScreenHeight() - buttonHeight - 30;

  return Button{
      Rectangle{static_cast<float>(buttonX), static_cast<float>(buttonY),
                static_cast<float>(buttonWidth),
                static_cast<float>(buttonHeight)},
      Vector2{static_cast<float>(buttonX), static_cast<float>(buttonY)},
      WHITE,
      EndScreen::ReplayButtonOnClick,
      RED,
      BLUE};
}

void EndScreen::ReplayButtonOnClick() {
  ScreenManager::Get().SwitchScenario(ScenarioType::MainMenu);
}
