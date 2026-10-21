#pragma once

#include <raylib.h>

#include <string>
#include <vector>

#include "HumanController.h"
#include "ValueAnimator.h"
#include "obstacle.h"
#include "scenario.h"

class AssetMap;

class Level : public Scenario {
public:
  Level(std::string background_key, Texture2D thumbnail_texture,
        std::string name, ScenarioType scenario_type, AssetMap *asset_map)
      : m_Name(name), m_ScenarioType(scenario_type),
        m_ThumbnailTexture(thumbnail_texture), m_BackgroundKey(background_key),
        m_AssetMap(asset_map) {}

  void addHuman(Vector2 position) { m_HumanPositions.push_back(position); }
  void addObstacle(Obstacle obstacle) { m_Obstacles.push_back(obstacle); }

  void OnEnter(Fly &fly) override;
  void OnExit(Fly &fly) override;

  void Draw(const Fly &fly) const override;
  void Update(Fly &fly) override;

  void SetScreenShakeDuration(float duration) {
    m_ScreenShakeDuration = duration;
  }

  Texture2D thumbnail_texture() const { return m_ThumbnailTexture; }
  const std::string &name() const { return m_Name; };
  ScenarioType scenario_type() const { return m_ScenarioType; };

private:
  const std::string m_Name;
  const ScenarioType m_ScenarioType;
  const Texture2D m_ThumbnailTexture;

  const std::string m_BackgroundKey;
  Texture2D m_Background{};

  std::vector<Obstacle> m_Obstacles;
  std::vector<Vector2> m_HumanPositions;

  Camera2D m_Camera;

  std::vector<HumanController> m_Humans;
  ValueAnimator m_ValueAnimator;
  AssetMap *m_AssetMap;

  // Tunable screen-shake settings. The offset is kept separate from the
  // camera target so shaking never changes the camera's actual position.
  float m_ScreenShakeDuration = 0.2f;
  float m_ScreenShakeMagnitude = 8.0f;
  float m_ScreenShakeTimer = 0.0f;
  Vector2 m_ScreenShakeOffset = Vector2Zero();

  void StartScreenShake();

  // TODO: fly
  // TODO: list of obstacles, slappables, etc.
};
