#pragma once

#include "Human.h"
#include "fly.h"

class HumanController {
public:
  HumanController();
  HumanController(Vector2 position);

  enum class State { Work, AnnoyedWork, Chase, AggressiveStand, Talk, Death };
  enum class Type { Intern, Programmer, Manager };

  Vector2 GetPosition() const;

  void Update(Fly &fly);

  void Buzzed();

  void AnnoyedWork(Fly &fly);
  void Work(Fly &fly);
  void Chase(Fly &fly);
  void AggressiveStand(Fly &fly);
  void Death(Fly &fly);

  void Draw() const;

private:
  Human m_Model;

  Vector2 m_BodyTarget = Vector2Zero();

  State m_State = State::Work;
  Type m_Type;

  float m_Aggressiveness;

  int m_BuzzCount = 0;
  int m_PunchCount = 0;

  // How many buzzes to get to AnnoyedWork
  int m_AnnoyedThreshold = 1;

  // How many punches to actively chase
  int m_ChaseThreshold = 2;

  bool m_DeathDone = false;

  Sound m_Keyboard;
};
