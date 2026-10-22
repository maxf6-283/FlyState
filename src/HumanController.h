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
  void SetClosest(bool closest);
  void SetForward(bool forward) { m_Model.SetForward(forward); }
  // Bosses use their own colors, attack faster, start out chasing and throw
  // projectiles at the fly when it is out of punching range
  void MakeBoss();
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
  struct Projectile {
    Vector2 start;
    Vector2 target;
    float timer = 0.0f;
    float duration = 0.0f;
  };

  // Scales an attack's windup/release time by the attack speed
  float AttackTime(float time) const { return time / m_AttackSpeed; }

  void ThrowProjectile(Vector2 target);
  void UpdateProjectiles(Fly &fly, std::vector<Obstacle> &obstacles);
  void DrawProjectiles() const;

  Human m_Model;

  bool m_IsBoss = false;
  // Bosses stay idle until the fly is within this horizontal/vertical
  // distance, which roughly keeps them on screen when they start attacking
  bool m_Activated = false;
  Vector2 m_ActivationDistance = {400.0f, 400.0f};
  float m_AttackSpeed = 1.0f;

  Color m_LimbColor = BLUE;
  Color m_FrontColor = GREEN;
  Color m_BackColor = DARKGREEN;
  Color m_BodyColor = ColorBrightness(BLUE, 0.1);

  std::vector<Projectile> m_Projectiles;
  float m_ThrowCooldown = 1.6f;
  float m_ThrowTimer = 0.0f;
  float m_ProjectileFlightTime = 1.0f;
  float m_ProjectileArcHeight = 150.0f;
  float m_ProjectileRadius = 20.0f;

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
  bool m_IsClosest = false;
  static constexpr float m_KeyboardVolumeFalloff = 500.0f;

  Sound m_Keyboard;
  Sound m_Parry;
  std::function<void()> m_ParriedCallback;
  std::function<void(int, Vector2)> m_ObjectDestroyedCallback;
};
