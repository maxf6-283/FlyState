#include "level.h"

#include <raylib.h>

void Level::OnEnter() {
  TraceLog(LOG_INFO, "Entered level %s", m_Name.c_str());
}

void Level::OnExit() {}

void Level::Draw() const {
  // TODO: make this fly POV
  DrawTexture(m_Background, 0, 0, WHITE);
}

void Level::Update(const Fly& fly) {}
