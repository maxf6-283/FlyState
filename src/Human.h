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

  struct LimbOutput {
    Vector2 root;
    Vector2 mid;
    Vector2 effect;
  };

  Human(const Foot &leftFoot, const Foot &rightFoot, float hipOffset = 30.0f,
        float legLength = 100.0f);

  void Update();

  LimbOutput GetRightLeg() const;
  LimbOutput GetLeftLeg() const;

  bool GetForward() const;
  void SetForward(bool forward);

  Vector2 GetPosition() const;
  void SetPosition(Vector2 position);

private:
  Vector2 m_Position = Vector2Zero();

  float m_HipOffset = 30.0f;

  Foot m_RightFoot{};
  Foot m_LeftFoot{};

  // Is character facing right?
  bool m_Forward = false;

  float m_LegSegmentLength = 100.0f;

  float m_MinimumFootDistance = 150.0f;

private:
  IKSolver m_LeftLegSolver;
  IKSolver m_RightLegSolver;

  LimbOutput m_RightLeg{};
  LimbOutput m_LeftLeg{};
};
