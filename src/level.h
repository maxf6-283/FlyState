#pragma once

#include <raylib.h>

#include <string>
#include <vector>

#include "HumanController.h"
#include "obstacle.h"
#include "scenario.h"

class Level : public Scenario {
public:
  Level(std::string name, ScenarioType scenario_type, Image thumbnail,
        Texture2D background, std::vector<Obstacle> obstacles)
      : m_Name(name), m_Thumbnail(thumbnail), m_ScenarioType(scenario_type),
        m_Background(background), m_Obstacles(obstacles) {}

  void OnEnter(Fly &fly) override;
  void OnExit(Fly &fly) override;

  void Draw(const Fly &fly) const override;
  void Update(Fly &fly) override;

  const Image &thumbnail() const { return m_Thumbnail; };
  const std::string &name() const { return m_Name; };
  ScenarioType scenario_type() const { return m_ScenarioType; };

private:
  const std::string m_Name;
  const Image m_Thumbnail;
  const ScenarioType m_ScenarioType;

  const Texture2D m_Background;

  std::vector<Obstacle> m_Obstacles;

  Camera2D m_Camera;

  std::vector<HumanController> m_Humans;

  // TODO: fly
  // TODO: list of obstacles, slappables, etc.
};
