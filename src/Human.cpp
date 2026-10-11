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
  Vector2 armPole = {m_Position.x - ((m_Forward) ? 100.0f : -100.0f),
                     m_Position.y};

  Vector2 hipToKnee = hip + Vector2{0.0f, m_LegSegmentLength};
  Vector2 kneeToFoot = hipToKnee + Vector2{0.0f, m_LegSegmentLength};

  IKSolver::Input legInput{hip, hipToKnee, kneeToFoot, legPole};

  IKSolver::Output leftLegOutput =
      m_LeftLegSolver.solve(legInput, m_LeftFoot.foot);

  IKSolver::Output rightLegOutput =
      m_RightLegSolver.solve(legInput, m_RightFoot.foot);

  m_LeftLeg.root = hip;
  m_LeftLeg.mid = leftLegOutput.mid;
  m_LeftLeg.effect = leftLegOutput.effect;

  m_RightLeg.root = hip;
  m_RightLeg.mid = rightLegOutput.mid;
  m_RightLeg.effect = rightLegOutput.effect;

  auto animateHand = [](Hand &hand) {
    if (hand.duration > 0.0f) {
      hand.timer += GetFrameTime();
      hand.timer = Clamp(hand.timer, 0.0f, hand.duration);

      float t = hand.timer / hand.duration;

      hand.hand = Vector2Lerp(hand.hand, hand.handTarget, t);
    }

    if (hand.timer >= hand.duration) {
      hand.timer = 0.0f;
      hand.duration = 0.0f;
    }
  };

  Vector2 shoulder = {hip.x, hip.y - m_BodySegmentLength};
  Vector2 elbow = shoulder + Vector2{0, m_ArmSegmentLength};
  Vector2 hand = elbow + Vector2{0, m_ArmSegmentLength};

  animateHand(m_RightHand);
  animateHand(m_LeftHand);

  IKSolver::Input armInput{shoulder, elbow, hand, armPole};

  IKSolver::Output leftArmOutput =
      m_LeftArmSolver.solve(armInput, m_LeftHand.hand);

  IKSolver::Output rightArmOutput =
      m_RightArmSolver.solve(armInput, m_RightHand.hand);

  m_RightArm.root = shoulder;
  m_RightArm.mid = rightArmOutput.mid;
  m_RightArm.effect = rightArmOutput.effect;

  m_LeftArm.root = shoulder;
  m_LeftArm.mid = leftArmOutput.mid;
  m_LeftArm.effect = leftArmOutput.effect;
}

bool Human::GetForward() const { return m_Forward; }
void Human::SetForward(bool forward) { m_Forward = forward; }

Human::LimbOutput Human::GetRightLeg() const { return m_RightLeg; }
Human::LimbOutput Human::GetLeftLeg() const { return m_LeftLeg; }

Vector2 Human::GetPosition() const { return m_Position; }
void Human::SetPosition(Vector2 position) { m_Position = position; }

Vector2 Human::GetRightFoot() const { return m_RightFoot.foot; }
Vector2 Human::GetLeftFoot() const { return m_LeftFoot.foot; }

void Human::leftArmTo(Vector2 target, float duration) {
  m_LeftHand.handTarget = target;
  m_LeftHand.duration = duration;
  m_LeftHand.timer = 0.0f;
}

void Human::rightArmTo(Vector2 target, float duration) {
  m_RightHand.handTarget = target;
  m_RightHand.duration = duration;
  m_RightHand.timer = 0.0f;
}

bool Human::leftArmDone() const {
  return m_LeftHand.timer == m_LeftHand.duration;
}

bool Human::rightArmDone() const {
  return m_RightHand.timer == m_RightHand.duration;
}

Human::LimbOutput Human::GetRightArm() const { return m_RightArm; }
Human::LimbOutput Human::GetLeftArm() const { return m_LeftArm; }

Vector2 Human::GetRightHand() const { return m_RightHand.hand; }
Vector2 Human::GetLeftHand() const { return m_LeftHand.hand; }

void Human::StabilizeBody() {
  Vector2 rightFoot = GetRightFoot();
  Vector2 leftFoot = GetLeftFoot();

  float balance = (rightFoot.x + leftFoot.x) * 0.5f;
  float t = 1.0f - exp(-8.0f * GetFrameTime());
  m_Position.x = Lerp(m_Position.x, balance, t);

  float offset = m_Forward ? 20.0f : -20.0f;
  float hipDistance = -30.0f;
  float hipY = m_Position.y + m_HipOffset;

  rightArmTo(Vector2{m_Position.x + offset, hipY - hipDistance}, 0.1f);
  leftArmTo(Vector2{m_Position.x + offset, hipY - hipDistance + 10.0f}, 0.1f);
}
