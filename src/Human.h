#pragma once

#include <raylib.h>
#include <raymath.h>

#include "IKSolver.h"

class Human {
public:
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

  void Update();

  void leftArmTo(Vector2 target, float duration);
  void rightArmTo(Vector2 target, float duration);
  bool leftArmDone() const;
  bool rightArmDone() const;

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

private:
  Vector2 m_Position = Vector2Zero();

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
