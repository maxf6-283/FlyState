#include "level_select.h"

void LevelSelect::OnEnter() { TraceLog(LOG_INFO, "Entered level select"); }

void LevelSelect::OnExit() {}

void LevelSelect::Draw() {
  ClearBackground(Color{190, 190, 190, 255});
  DrawText("Level Select", 32, 32, 40, DARKGRAY);
}

void LevelSelect::Update() {}
