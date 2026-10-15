#include "level.h"

#include <raylib.h>

#include "obstacle.h"
#include "raymath.h"

void Level::OnEnter(Fly& fly) {
  TraceLog(LOG_INFO, "Entered level %s", m_Name.c_str());
  // TODO: define level-specific starting positions for the fly
  const Vector2 fly_start_pos =
      Vector2{m_Background.width / 2.0f, m_Background.height / 2.0f};
  const Vector2 fly_screen_pos = fly.position();

  fly.SetPos(fly_start_pos);

  // calculate camera pos so that the fly starts at the same point on screen.
  m_Camera.target.x = fly_start_pos.x - fly_screen_pos.x;
  m_Camera.target.y = fly_start_pos.y - fly_screen_pos.y;

  m_Camera.zoom = 1.0;
  m_Camera.rotation = 0.0;
  m_Camera.offset = {0.0, 0.0};
}

void Level::OnExit(Fly& fly) {}

void Level::Draw(const Fly& fly) const {
  BeginMode2D(m_Camera);

  DrawTexture(m_Background, 0, 0, WHITE);

  for (const Obstacle& obstacle : m_Obstacles) {
    obstacle.Draw();
  }

  fly.Draw();

  EndMode2D();
}

void Level::Update(Fly& fly) {
  // lerp camera target to fly pos
  m_Camera.target = Vector2Lerp(
      m_Camera.target,
      fly.position() -
          Vector2{GetScreenWidth() * 1.0f, GetScreenHeight() * 1.0f} / 2,
      0.1f);

  // clamp camera position to only show bg
  if (m_Camera.target.x < 0) {
    m_Camera.target.x = 0;
  }
  if (m_Camera.target.y < 0) {
    m_Camera.target.y = 0;
  }

  if (m_Camera.target.x + GetScreenWidth() > m_Background.width) {
    m_Camera.target.x = m_Background.width - GetScreenWidth();
  }
  if (m_Camera.target.y + GetScreenHeight() > m_Background.height) {
    m_Camera.target.y = m_Background.height - GetScreenHeight();
  }

  fly.Update();

  for (const Obstacle& obstacle : m_Obstacles) {
    fly.Offset(obstacle.CollisionOffset(fly.position(), fly.radius()));
  }
}
