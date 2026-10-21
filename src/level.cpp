#include "level.h"

#include <raylib.h>

#include <algorithm>
#include <limits>

#include "AssetMap.h"
#include "obstacle.h"
#include "raymath.h"
#include "screen_manager.h"

void Level::OnEnter(Fly &fly) {
  TraceLog(LOG_INFO, "Entered level %s", m_Name.c_str());
  for (Obstacle &obstacle : m_Obstacles) {
    obstacle.Reset();
  }
  m_Humans.clear();
  m_ValueAnimator.Reset();
  m_ScreenShakeTimer = 0.0f;
  m_ScreenShakeOffset = Vector2Zero();
  for (const HumanSpawn &spawn : m_HumanSpawns) {
    m_Humans.emplace_back(spawn.position);
    m_Humans.back().SetForward(spawn.forward);
  }
  for (HumanController &human : m_Humans) {
    human.SetParriedCallback([this]() { StartScreenShake(); });
    human.SetObjectDestroyedCallback(
        [this](int value, Vector2 position) {
          m_ValueAnimator.AddValue(value, position);
        });
  }

  const auto background = m_AssetMap->GetAsset(m_BackgroundKey);
  if (background && std::holds_alternative<Texture2D>(*background)) {
    m_Background = std::get<Texture2D>(*background);
  }
  const auto buzz = m_AssetMap->GetAsset("buzzsfx");
  if (buzz && std::holds_alternative<Sound>(*buzz)) {
    fly.SetBuzzAsset(std::get<Sound>(*buzz));
  }
  const auto keyboard = m_AssetMap->GetAsset("type");
  if (keyboard && std::holds_alternative<Sound>(*keyboard)) {
    for (HumanController &human : m_Humans) {
      human.SetKeyboardAsset(std::get<Sound>(*keyboard));
    }
  }
  const auto parry = m_AssetMap->GetAsset("parry");
  if (parry && std::holds_alternative<Sound>(*parry)) {
    for (HumanController &human : m_Humans) {
      human.SetParryAsset(std::get<Sound>(*parry));
    }
  }
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

void Level::OnExit(Fly &fly) {
  for (HumanController &human : m_Humans) {
    human.SetClosest(false);
  }
  m_Humans.clear();
  m_ValueAnimator.Reset();
  for (Obstacle &obstacle : m_Obstacles) {
    obstacle.Reset();
  }
}

void Level::Draw(const Fly &fly) const {
  Camera2D drawCamera = m_Camera;
  drawCamera.target += m_ScreenShakeOffset;
  BeginMode2D(drawCamera);

  DrawTexture(m_Background, 0, 0, WHITE);

  for (const Obstacle &obstacle : m_Obstacles) {
    obstacle.Draw();
  }
  m_ValueAnimator.Draw();

  fly.Draw();
  for (const HumanController &controller : m_Humans) {
    controller.Draw();
  }

  EndMode2D();

  const Font font = ScreenManager::Get().font();
  DrawTextEx(font, TextFormat("Destruction Caused: $%d", fly.Money()),
             {20.0f, 20.0f}, 24.0f, 0.0f, DARKGRAY);
  DrawTextEx(font,
             TextFormat("Employees Lost: %d", fly.HumansDestroyedCounter()),
             {20.0f, 50.0f}, 24.0f, 0.0f, DARKGRAY);

  float width = GetScreenWidth();
  float height = GetScreenHeight();

  if (!fly.IsBuzzing()) {
    DrawCircle(0.9 * width, 0.1 * height, 20.0f, DARKGREEN);
  } else {
    DrawCircle(0.9 * width, 0.1 * height, 20.0f, ColorBrightness(RED, 0.1f));
  }

  if (fly.CanDash()) {
    DrawCircle(0.9 * width, 0.25 * height, 20.0f, PURPLE);
  } else {
    DrawCircle(0.9 * width, 0.25 * height, 20.0f, ColorBrightness(RED, 0.1f));
  }

  if (fly.CanParry()) {
    DrawCircle(0.9 * width, 0.4 * height, 20.0f, BLUE);
  } else {
    DrawCircle(0.9 * width, 0.4 * height, 20.0f, ColorBrightness(RED, 0.1f));
  }
}

void Level::Update(Fly &fly) {
  const float frameTime = GetFrameTime();
  m_ValueAnimator.Update();
  m_ScreenShakeTimer = std::max(0.0f, m_ScreenShakeTimer - frameTime);
  if (m_ScreenShakeTimer == 0.0f) {
    m_ScreenShakeOffset = Vector2Zero();
  } else {
    m_ScreenShakeOffset = {static_cast<float>(GetRandomValue(-100, 100)) /
                               100.0f * m_ScreenShakeMagnitude,
                           static_cast<float>(GetRandomValue(-100, 100)) /
                               100.0f * m_ScreenShakeMagnitude};
  }

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

  if (fly.IsDead()) {
    ScreenManager::Get().SwitchScenario(ScenarioType::DeathScreen);
  }

  fly.Update(m_Obstacles);

  for (const Obstacle &obstacle : m_Obstacles) {
    fly.Offset(obstacle.CollisionOffset(fly.position(), fly.radius()));
  }

  if (fly.IsBuzzing()) {
    int min = -1;
    float currentMinDist = std::numeric_limits<float>::max();
    for (int i = 0; i < m_Humans.size(); ++i) {
      float dist2 =
          Vector2DistanceSqr(m_Humans.at(i).GetPosition(), fly.position());
      if (dist2 < 20000 && dist2 < currentMinDist) {
        min = i;
        currentMinDist = dist2;
      }
    }
    if (min > -1) {
      m_Humans.at(min).Buzzed();
    }
  }

  int closestHuman = -1;
  float closestDistance = std::numeric_limits<float>::max();
  for (int i = 0; i < m_Humans.size(); ++i) {
    const float distance =
        Vector2DistanceSqr(m_Humans.at(i).GetPosition(), fly.position());
    if (distance < closestDistance) {
      closestDistance = distance;
      closestHuman = i;
    }
  }

  for (int i = 0; i < m_Humans.size(); ++i) {
    m_Humans.at(i).SetClosest(i == closestHuman);
  }

  for (HumanController &controller : m_Humans) {
    controller.Update(fly, m_Obstacles);
  }

  for (auto human = m_Humans.begin(); human != m_Humans.end();) {
    if (human->DeathDone()) {
      fly.AddHumanDestroyed();
      human = m_Humans.erase(human);
    } else {
      ++human;
    }
  }

  if (m_Humans.empty() && !fly.IsDead()) {
    ScreenManager::Get().SwitchScenario(ScenarioType::EndScreen);
    return;
  }
}

void Level::StartScreenShake() {
  m_ScreenShakeTimer = std::max(m_ScreenShakeTimer, m_ScreenShakeDuration);
}
