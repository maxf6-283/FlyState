#pragma once

#include <raylib.h>
#include <raymath.h>

#include <functional>
#include <vector>

#include "IKSolver.h"

class Human {
public:
  using ActionCallback = std::function<void(Vector2)>;
  using ActionCallbacks = std::vector<ActionCallback>;

  enum class ArmState {
    Slap,
    Punch,

    // Not punching or slapping so allow any arm motion
    Any
  };

  struct ArmAction {
    enum class WindupState { Begin, Windup, Release, Done };

    ArmState state = ArmState::Any;
    Vector2 target = Vector2Zero();

    float windupTime = 0.0f;
    float releaseTime = 0.0f;

    WindupState windupState = WindupState::Done;
    ActionCallbacks callbacks;
  };

  struct Foot {
    Vector2 foot{};
    Vector2 footTarget{};

    Vector2 prevFoot{};
    bool isStepping = false;
    float strideLength = 80.0f;
    float legLength = 200.0f;
    float stepHeight = 20.0f;

    float stepDuration = 0.5f;
    float stepTimer = 0.0f;
  };

  struct Hand {
    Vector2 hand{};
    Vector2 prevHand{};
    Vector2 handTarget{};

    float duration;
    float timer;
  };

  struct LimbOutput {
    Vector2 root;
    Vector2 mid;
    Vector2 effect;
  };

  Human(const Foot &leftFoot, const Foot &rightFoot, float hipOffset = 30.0f,
        float legLength = 100.0f);

  void SetLeftFoot(const Foot &leftFoot);
  void SetRightFoot(const Foot &rightFoot);

  void Update();

  void MoveTo(Vector2 target, float duration);

  void Parried();

  void LeftArmTo(Vector2 target, float duration);
  void RightArmTo(Vector2 target, float duration);

  void LeftSlap(Vector2 slapTarget, float windupTime, float releaseTime,
                const ActionCallbacks &callbacks = {});
  void RightSlap(Vector2 slapTarget, float windupTime, float releaseTime,
                 const ActionCallbacks &callbacks = {});

  void LeftPunch(Vector2 punchTarget, float windupTime, float releaseTime,
                 const ActionCallbacks &callbacks = {});
  void RightPunch(Vector2 punchTarget, float windupTime, float releaseTime,
                  const ActionCallbacks &callbacks = {});

  bool LeftArmDone() const;
  bool RightArmDone() const;
  bool BodyMoveDone() const;

  ArmState LeftArmState() const;
  ArmState RightArmState() const;

  LimbOutput GetRightLeg() const;
  LimbOutput GetLeftLeg() const;

  LimbOutput GetRightArm() const;
  LimbOutput GetLeftArm() const;

  Vector2 GetRightFoot() const;
  Vector2 GetLeftFoot() const;

  Vector2 GetRightHand() const;
  Vector2 GetLeftHand() const;

  bool GetForward() const;
  void SetForward(bool forward);

  Vector2 GetPosition() const;
  void SetPosition(Vector2 position);

  void StabilizeBody();

  // Draws a marker at each active slap/punch target that shifts from
  // yellow to orange during windup and turns red once the hit is released
  void DrawTargetIndicators() const;

private:
  Vector2 m_PrevPosition = Vector2Zero();
  Vector2 m_Position = Vector2Zero();
  Vector2 m_BodyTarget = Vector2Zero();

  ArmAction m_LeftArmAction;
  ArmAction m_RightArmAction;

  float m_MoveTimer = 0.0f;
  float m_MoveDuration = 0.0f;

  float m_HipOffset = 30.0f;

  Foot m_RightFoot{};
  Foot m_LeftFoot{};

  Hand m_LeftHand{};
  Hand m_RightHand{};

  // Is character facing right?
  bool m_Forward = false;

  float m_LegSegmentLength = 100.0f;

  float m_MinimumFootDistance = 150.0f;

  float m_BodySegmentLength = 100.0f;

  float m_ArmSegmentLength = 90.0f;

private:
  // Only really need one solver because
  // it's stateless so far. But just in case I have to make
  // it track unique state to a limb then it will be duplicated here
  IKSolver m_LeftLegSolver;
  IKSolver m_RightLegSolver;

  IKSolver m_LeftArmSolver;
  IKSolver m_RightArmSolver;

  LimbOutput m_RightLeg{};
  LimbOutput m_LeftLeg{};

  LimbOutput m_RightArm{};
  LimbOutput m_LeftArm{};
};
