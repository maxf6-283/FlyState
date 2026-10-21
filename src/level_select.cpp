#include "level_select.h"

#include <raylib.h>

#include "button.h"
#include "fly.h"
#include "screen_manager.h"

static const int level_horizontal_sep = 50;
static const int level_row_height = 100;
static const int level_font_size = 24;
static const int level_text_sep = 20;
static const int start_x = 50;
static const int start_y = 150;

void LevelSelect::OnEnter(Fly& fly) {
  TraceLog(LOG_INFO, "Entered level select");

  m_LevelButtons.clear();
  const int width = GetScreenWidth();
  float x = start_x;
  float y = start_y;
  for (const Level& level : ScreenManager::Get().levels()) {
    Texture2D img = level.thumbnail_texture();

    if (x + img.width + level_horizontal_sep / 2.0 > width) {
      x = start_x;
      y += level_row_height;
    }

    m_LevelButtons.push_back(LevelButton{
        Button(
            Rectangle{
                .x = x,
                .y = y,
                .width = static_cast<float>(img.width),
                .height = static_cast<float>(img.height),
            },
            Vector2{.x = x, .y = y}, img, img, img,
            [scenarioType = level.scenario_type()]() {
              ScreenManager::Get().SwitchScenario(scenarioType);
            },
            WHITE, LIGHTGRAY, GRAY),
        level.name()});

    x += level_horizontal_sep + img.width;
  }
}

void LevelSelect::OnExit(Fly& fly) {}

void LevelSelect::Draw(const Fly& fly) const {
  ClearBackground(Color{190, 190, 190, 255});
  DrawTextEx(ScreenManager::Get().font(), "Level Select", {32, 32}, 48, 0,
             DARKGRAY);

  for (const LevelButton& level_button : m_LevelButtons) {
    level_button.button.Draw();
    int text_width = MeasureTextEx(ScreenManager::Get().font(),
                                   level_button.name.c_str(), level_font_size, 0).x;
    int pos_x = level_button.button.area().x +
                level_button.button.area().width / 2.0 - text_width / 2.0;
    int pos_y = level_button.button.area().y +
                level_button.button.area().height + level_text_sep;
    DrawTextEx(ScreenManager::Get().font(), level_button.name.c_str(),
               {static_cast<float>(pos_x), static_cast<float>(pos_y)},
               level_font_size, 0, DARKGRAY);
  }

  fly.Draw();
}

void LevelSelect::Update(Fly& fly) {
  fly.Update();

  for (LevelButton& level_button : m_LevelButtons) {
    level_button.button.Update(fly);
  }
}
