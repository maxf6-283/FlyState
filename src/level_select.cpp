#include "level_select.h"

#include <raylib.h>

#include "button.h"
#include "screen_manager.h"

static const int level_horizontal_sep = 50;
static const int level_row_height = 100;
static const int level_font_size = 20;
static const int level_text_sep = 20;
static const int start_x = 50;
static const int start_y = 150;

void LevelSelect::OnEnter() {
  TraceLog(LOG_INFO, "Entered level select");

  m_LevelButtons.clear();
  const int width = GetScreenWidth();
  float x = start_x;
  float y = start_y;
  for (const Level& level : ScreenManager::Get().levels()) {
    const Image& thumbnail = level.thumbnail();
    Texture2D img = LoadTextureFromImage(thumbnail);

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
            [&level]() {
              ScreenManager::Get().SwitchScenario(level.scenario_type());
            },
            WHITE, LIGHTGRAY, GRAY),
        level.name()});

    x += level_horizontal_sep + img.width;
  }
}

void LevelSelect::OnExit() {}

void LevelSelect::Draw() const {
  ClearBackground(Color{190, 190, 190, 255});
  DrawText("Level Select", 32, 32, 40, DARKGRAY);

  for (const LevelButton& level_button : m_LevelButtons) {
    level_button.button.Draw();
    int text_width = MeasureText(level_button.name.c_str(), level_font_size);
    int pos_x = level_button.button.area().x +
                level_button.button.area().width / 2.0 - text_width / 2.0;
    int pos_y = level_button.button.area().y +
                level_button.button.area().height + level_text_sep;
    DrawText(level_button.name.c_str(), pos_x, pos_y, level_font_size,
             DARKGRAY);
  }
}

void LevelSelect::Update(const Fly& fly) {
  for (LevelButton& level_button : m_LevelButtons) {
    level_button.button.Update(fly);
  }
}
