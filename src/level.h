#pragma once

#include <raylib.h>

#include <string>

#include "scenario.h"

class Level : public Scenario {
 public:
  Level(std::string name, ScenarioType scenario_type, Image thumbnail,
        Texture2D background)
      : m_Name(name),
        m_Thumbnail(thumbnail),
        m_ScenarioType(scenario_type),
        m_Background(background) {}

  void OnEnter() override;
  void OnExit() override;

  void Draw() const override;
  void Update(const Fly& fly) override;

  const Image& thumbnail() const { return m_Thumbnail; };
  const std::string& name() const { return m_Name; };
  ScenarioType scenario_type() const { return m_ScenarioType; };

 private:
  std::string m_Name;
  Image m_Thumbnail;
  ScenarioType m_ScenarioType;

  Texture2D m_Background;

  // TODO: fly
  // TODO: list of obstacles, slappables, etc.
};
