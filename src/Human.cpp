#include "Human.h"

#include <cmath>

Human::Human(const Foot &leftFoot, const Foot &rightFoot, float hipOffset,
             float legLength) {
  m_LeftFoot = leftFoot;
  m_RightFoot = rightFoot;
  m_HipOffset = hipOffset;
  m_LegSegmentLength = legLength;
}

void UpdateFoot(Vector2 &hip, Human::Foot &foot, bool forward) {
  if (fabs(foot.foot.x - hip.x) >= foot.strideLength && !foot.isStepping) {
    if (forward) {
      foot.footTarget.x = hip.x + foot.strideLength;
    } else {
      foot.footTarget.x = hip.x - foot.strideLength;
    }

    foot.prevFoot = foot.foot;
    foot.isStepping = true;
  }

  if (foot.isStepping) {
    foot.stepTimer += GetFrameTime();

    float t = foot.stepTimer / foot.stepDuration;

    foot.foot.x = (1.0f - t) * foot.prevFoot.x + t * foot.footTarget.x;
    foot.foot.y = foot.footTarget.y - foot.stepHeight * sinf(PI * t);

    if (foot.stepTimer >= foot.stepDuration) {
      foot.isStepping = false;
    }
  }

  if (!foot.isStepping) {
    foot.foot = foot.footTarget;
    foot.stepTimer = 0.0f;
  }
}

void Human::Update() {

  Vector2 hip = {m_Position.x, m_Position.y};

  if (!m_LeftFoot.isStepping) {
    UpdateFoot(hip, m_RightFoot, m_Forward);
  }
  if (!m_RightFoot.isStepping) {
    UpdateFoot(hip, m_LeftFoot, m_Forward);
  }

  float horizontalOff = 0.0f;

  bool isRightLongest = false;
  isRightLongest = Vector2LengthSqr(hip - m_LeftFoot.foot) <
                   Vector2LengthSqr(hip - m_RightFoot.foot);

  if (isRightLongest) {
    horizontalOff = hip.x - m_RightFoot.foot.x;

    float maxHorizontalOffset =
        sqrtf(fmax(0.0f, m_RightFoot.legLength * m_RightFoot.legLength -
                             m_MinimumFootDistance * m_MinimumFootDistance));

    horizontalOff =
        Clamp(horizontalOff, -maxHorizontalOffset, maxHorizontalOffset);

    hip.y = m_RightFoot.foot.y -
            sqrtf(m_RightFoot.legLength * m_RightFoot.legLength -
                  horizontalOff * horizontalOff);
  } else {
    horizontalOff = hip.x - m_LeftFoot.foot.x;

    float maxHorizontalOffset =
        sqrtf(fmax(0.0f, m_LeftFoot.legLength * m_LeftFoot.legLength -
                             m_MinimumFootDistance * m_MinimumFootDistance));

    horizontalOff =
        Clamp(horizontalOff, -maxHorizontalOffset, maxHorizontalOffset);

    hip.y =
        m_LeftFoot.foot.y - sqrtf(m_LeftFoot.legLength * m_LeftFoot.legLength -
                                  horizontalOff * horizontalOff);
  }

  hip.y += m_HipOffset;

  Vector2 legPole = {m_Position.x + ((m_Forward) ? 100.0f : -100.0f),
                     m_Position.y};

  float radius = 100.0f;

  Vector2 hipToKnee = hip + Vector2{0.0f, m_LegSegmentLength};
  Vector2 kneeToFoot = hipToKnee + Vector2{0.0f, m_LegSegmentLength};

  IKSolver::Input input{hip, hipToKnee, kneeToFoot, legPole};

  // Vector2 body = {initialX, hip.y - 20.0f};
  //
  // Vector2 shoulder = {initialX, body.y - 100.0f};
  // Vector2 elbow = {initialX, shoulder.y + 90.0f};
  // Vector2 hand = {initialX, elbow.y + 90.0f};

  // float armRadius = Lerp(20.0f, 50.0f, Clamp(cos(GetTime()), 0.0f, 1.0f));
  // Vector2 armTarget =
  //     shoulder + Vector2(cos(GetTime() * 2.5f) * armRadius * 5.5f,
  //                        sin(GetTime()) * armRadius * 2.2f);
  // armTarget.y += 115.0f;
  //
  // IKSolver::Input armInput{shoulder, elbow, hand, armPole};

  IKSolver::Output leftLegOutput =
      m_LeftLegSolver.solve(input, m_LeftFoot.foot);

  IKSolver::Output rightLegOutput =
      m_RightLegSolver.solve(input, m_RightFoot.foot);

  m_LeftLeg.root = hip;
  m_LeftLeg.mid = leftLegOutput.mid;
  m_LeftLeg.effect = leftLegOutput.effect;

  m_RightLeg.root = hip;
  m_RightLeg.mid = rightLegOutput.mid;
  m_RightLeg.effect = rightLegOutput.effect;
}

bool Human::GetForward() const { return m_Forward; }
void Human::SetForward(bool forward) { m_Forward = forward; }

Human::LimbOutput Human::GetRightLeg() const { return m_RightLeg; }
Human::LimbOutput Human::GetLeftLeg() const { return m_LeftLeg; }

Vector2 Human::GetPosition() const { return m_Position; }
void Human::SetPosition(Vector2 position) { m_Position = position; }
