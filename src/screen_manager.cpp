#include "screen_manager.h"

#include <stdexcept>

#include "scenario.h"

void ScreenManager::Draw() { scenario_.Draw(); }

void ScreenManager::Update() { scenario_.Update(); }

void ScreenManager::SwitchScenario(ScenarioType scen_type) {
  scenario_.OnExit();
  scenario_ = GetScenario(scen_type);
  scenario_.OnEnter();
}

Scenario& ScreenManager::GetScenario(ScenarioType scen_type) {
  switch (scen_type) {
    case ScenarioType::MainMenu:
      return main_menu_;
    default:
      throw std::invalid_argument("Invalid scenario type!");
  }
}