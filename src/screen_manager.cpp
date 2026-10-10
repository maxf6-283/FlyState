#include "screen_manager.h"

#include <stdexcept>

#include "scenario.h"

void ScreenManager::Draw() { m_Scenario->Draw(); }

void ScreenManager::Update() { m_Scenario->Update(); }

void ScreenManager::SwitchScenario(ScenarioType sceneType) {
  m_Scenario->OnExit();
  m_Scenario = GetScenario(sceneType);
  m_Scenario->OnEnter();
}

Scenario *ScreenManager::GetScenario(ScenarioType sceneType) {
  switch (sceneType) {
  case ScenarioType::MainMenu:
    return &m_MainMenu;
  case ScenarioType::LevelSelect:
    return &m_LevelSelect;
  default:
    throw std::invalid_argument("Invalid scenario type!");
  }
}
