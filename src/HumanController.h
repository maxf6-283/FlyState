#pragma once

#include <functional>

#include "Human.h"
#include "fly.h"

class AssetMap;

class HumanController {
public:
  HumanController();
  HumanController(Vector2 position);

  enum class State { Work, AnnoyedWork, Chase, AggressiveStand, Talk, Death };
  enum class Type { Intern, Programmer, Manager };

  Vector2 GetPosition() const;

  void Update(Fly &fly, std::vector<Obstacle> &obstacles);

  void Buzzed();

  void AnnoyedWork(Fly &fly, std::vector<Obstacle> &obstacle);
  void Work(Fly &fly, std::vector<Obstacle> &obstacle);
  void Chase(Fly &fly, std::vector<Obstacle> &obstacle);
  void AggressiveStand(Fly &fly, std::vector<Obstacle> &obstacle);
  void Death(Fly &fly, std::vector<Obstacle> &obstacle);

  bool DeathDone() const { return m_DeathDone && m_Model.BodyMoveDone(); }

  void Draw() const;

  void SlappedFly(Fly *fly);
  void SetKeyboardAsset(Sound keyboard) { m_Keyboard = keyboard; }
  void SetParryAsset(Sound parry) { m_Parry = parry; }
  void SetParriedCallback(const std::function<void()> &callback) {
    m_ParriedCallback = callback;
  }
  void SetObjectDestroyedCallback(
      const std::function<void(int, Vector2)> &callback) {
    m_ObjectDestroyedCallback = callback;
  }

  std::function<void(Vector2)> FlyHitCallback(Fly &fly,
                                              std::vector<Obstacle> &obstacles);

private:
  Human m_Model;

  bool m_Staggered = false;
  float m_StaggerTimer = 0.0f;
  float m_StaggerDuration = 0.2f;

  Vector2 m_BodyTarget = Vector2Zero();

  State m_State = State::Work;
  Type m_Type;

  float m_Aggressiveness;

  int m_BuzzCount = 0;
  int m_PunchCount = 0;

  // How many buzzes to get to AnnoyedWork
  int m_AnnoyedThreshold = 1;

  // How many punches to actively chase
  int m_ChaseThreshold = 4;

  bool m_DeathDone = false;

  Sound m_Keyboard;
  Sound m_Parry;
  std::function<void()> m_ParriedCallback;
  std::function<void(int, Vector2)> m_ObjectDestroyedCallback;
};
