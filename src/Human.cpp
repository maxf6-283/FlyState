#include "Human.h"

#include <cmath>

Human::Human(const Foot &leftFoot, const Foot &rightFoot, float hipOffset,
             float legLength) {
  m_LeftFoot = leftFoot;
  m_RightFoot = rightFoot;
  m_HipOffset = hipOffset;
  m_LegSegmentLength = legLength;

  m_LeftHand.handTarget = m_Position;
  m_RightHand.handTarget = m_Position;
}

void Human::SetLeftFoot(const Foot &leftFoot) { m_LeftFoot = leftFoot; }

void Human::SetRightFoot(const Foot &rightFoot) { m_RightFoot = rightFoot; }

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

void Human::Parried() {
  m_LeftArmAction.callbacks.clear();
  m_RightArmAction.callbacks.clear();
}

void Human::Update() {
  if (m_MoveDuration != 0.0f) {
    m_MoveTimer += GetFrameTime();

    m_MoveTimer = Clamp(m_MoveTimer, 0.0f, m_MoveDuration);

    float t = m_MoveTimer / m_MoveDuration;
    m_Position = Vector2Lerp(m_PrevPosition, m_BodyTarget, t);

    if (m_MoveTimer >= m_MoveDuration) {
      m_MoveTimer = 0.0f;
      m_MoveDuration = 0.0f;
      m_Position = m_BodyTarget;
    }
  }

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

      hand.hand = Vector2Lerp(hand.prevHand, hand.handTarget, t);
    }

    if (hand.timer >= hand.duration) {
      hand.timer = 0.0f;
      hand.duration = 0.0f;
    }
  };

  auto executeCallbacks = [](ArmAction &armAction, Vector2 handPosition) {
    ActionCallbacks callbacks;
    callbacks.swap(armAction.callbacks);

    for (const ActionCallback &callback : callbacks) {
      if (callback) {
        callback(handPosition);
      }
    }

    callbacks.clear();
  };

  auto executeArmAction = [this, &executeCallbacks](ArmAction &armAction,
                                                    bool left,
                                                    Vector2 shoulder) {
    bool armDone = left ? LeftArmDone() : RightArmDone();
    if (armAction.state == ArmState::Slap) {
      if (armDone || armAction.windupState == ArmAction::WindupState::Begin) {
        switch (armAction.windupState) {
        case ArmAction::WindupState::Begin:
          armAction.windupState = ArmAction::WindupState::Windup;
        case ArmAction::WindupState::Windup: {
            float height = 20.0f;
            if (left) {
                LeftArmTo(armAction.target - Vector2{ 0.0f, height },
                    armAction.windupTime);
            }
            else {
                RightArmTo(armAction.target - Vector2{ 0.0f, height },
                    armAction.windupTime);
            }
            armAction.windupState = ArmAction::WindupState::Release;
            break;
        } case ArmAction::WindupState::Release:
          if (left) {
            LeftArmTo(armAction.target, armAction.releaseTime);
          } else {
            RightArmTo(armAction.target, armAction.releaseTime);
          }
          armAction.windupState = ArmAction::WindupState::Done;
          break;
        case ArmAction::WindupState::Done:
          armAction.state = ArmState::Any;
          executeCallbacks(armAction, left ? GetLeftHand() : GetRightHand());
          break;
        }
      }
    } else if (armAction.state == ArmState::Punch) {
      if (armDone || armAction.windupState == ArmAction::WindupState::Begin) {
        switch (armAction.windupState) {
        case ArmAction::WindupState::Begin:
          armAction.windupState = ArmAction::WindupState::Windup;
        case ArmAction::WindupState::Windup: {
          Vector2 windupTarget =
              shoulder +
              (GetForward() ? Vector2{10.0, 0.0} : Vector2{-10.0, 0.0});
          if (left) {
            LeftArmTo(windupTarget, armAction.windupTime);
          } else {
            RightArmTo(windupTarget, armAction.windupTime);
          }
          armAction.windupState = ArmAction::WindupState::Release;
          break;
        }
        case ArmAction::WindupState::Release:
          if (left) {
            LeftArmTo(armAction.target, armAction.releaseTime);
          } else {
            RightArmTo(armAction.target, armAction.releaseTime);
          }
          armAction.windupState = ArmAction::WindupState::Done;
          break;
        case ArmAction::WindupState::Done:
          armAction.state = ArmState::Any;
          executeCallbacks(armAction, left ? GetLeftHand() : GetRightHand());
          break;
        }
      }
    }
  };

  Vector2 shoulder = {hip.x, hip.y - m_BodySegmentLength};
  Vector2 elbow = shoulder + Vector2{0, m_ArmSegmentLength};
  Vector2 hand = elbow + Vector2{0, m_ArmSegmentLength};

  executeArmAction(m_LeftArmAction, true, shoulder);
  executeArmAction(m_RightArmAction, false, shoulder);

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

void Human::LeftArmTo(Vector2 target, float duration) {
  m_LeftHand.prevHand = m_LeftHand.hand;
  m_LeftHand.handTarget = target;
  m_LeftHand.duration = duration;
  m_LeftHand.timer = 0.0f;
}

void Human::RightArmTo(Vector2 target, float duration) {
  m_RightHand.prevHand = m_RightHand.hand;
  m_RightHand.handTarget = target;
  m_RightHand.duration = duration;
  m_RightHand.timer = 0.0f;
}

bool Human::LeftArmDone() const {
  return m_LeftHand.timer == m_LeftHand.duration;
}

bool Human::RightArmDone() const {
  return m_RightHand.timer == m_RightHand.duration;
}

void Human::MoveTo(Vector2 target, float duration) {
  if (target.x == m_Position.x)
    return;

  if (target.x > m_Position.x) {
    SetForward(true);
  } else if (target.x < m_Position.x) {
    SetForward(false);
  }

  m_MoveDuration = duration;
  m_MoveTimer = 0.0f;
  m_BodyTarget = target;
  m_PrevPosition = m_Position;
}

void Human::LeftSlap(Vector2 slapTarget, float windupTime, float releaseTime,
                     const ActionCallbacks &callbacks) {
  if (m_LeftArmAction.state != ArmState::Any)
    return;

  m_LeftArmAction.windupTime = windupTime;
  m_LeftArmAction.releaseTime = releaseTime;
  m_LeftArmAction.windupState = ArmAction::WindupState::Begin;

  m_LeftArmAction.state = ArmState::Slap;
  m_LeftArmAction.target = slapTarget;
  m_LeftArmAction.callbacks = callbacks;
}

void Human::RightSlap(Vector2 slapTarget, float windupTime, float releaseTime,
                      const ActionCallbacks &callbacks) {
  if (m_RightArmAction.state != ArmState::Any)
    return;

  m_RightArmAction.windupTime = windupTime;
  m_RightArmAction.releaseTime = releaseTime;
  m_RightArmAction.windupState = ArmAction::WindupState::Begin;

  m_RightArmAction.state = ArmState::Slap;
  m_RightArmAction.target = slapTarget;
  m_RightArmAction.callbacks = callbacks;
}

void Human::LeftPunch(Vector2 punchTarget, float windupTime, float releaseTime,
                      const ActionCallbacks &callbacks) {
  if (m_LeftArmAction.state != ArmState::Any)
    return;

  m_LeftArmAction.windupTime = windupTime;
  m_LeftArmAction.releaseTime = releaseTime;
  m_LeftArmAction.windupState = ArmAction::WindupState::Begin;

  m_LeftArmAction.state = ArmState::Punch;
  m_LeftArmAction.target = punchTarget;
  m_LeftArmAction.callbacks = callbacks;
}

void Human::RightPunch(Vector2 punchTarget, float windupTime, float releaseTime,
                       const ActionCallbacks &callbacks) {
  if (m_RightArmAction.state != ArmState::Any)
    return;

  m_RightArmAction.windupTime = windupTime;
  m_RightArmAction.releaseTime = releaseTime;
  m_RightArmAction.windupState = ArmAction::WindupState::Begin;

  m_RightArmAction.state = ArmState::Punch;
  m_RightArmAction.target = punchTarget;
  m_RightArmAction.callbacks = callbacks;
}

Human::LimbOutput Human::GetRightArm() const { return m_RightArm; }
Human::LimbOutput Human::GetLeftArm() const { return m_LeftArm; }

Vector2 Human::GetRightHand() const { return m_RightHand.hand; }
Vector2 Human::GetLeftHand() const { return m_LeftHand.hand; }

void Human::StabilizeBody() {
  float offset = m_Forward ? 20.0f : -20.0f;
  float hipDistance = -30.0f;
  float hipY = m_Position.y + m_HipOffset;
  RightArmTo(Vector2{m_Position.x + offset, hipY - hipDistance}, 0.1f);
  LeftArmTo(Vector2{m_Position.x - offset, hipY - hipDistance + 10.0f}, 0.1f);

  // Don't stabilize hips if movement is still in progress
  // because you can get launched back depending on where your
  // feet are
  if (!BodyMoveDone())
    return;

  Vector2 rightFoot = GetRightFoot();
  Vector2 leftFoot = GetLeftFoot();

  float balance = (rightFoot.x + leftFoot.x) * 0.5f;
  float t = 1.0f - exp(-8.0f * GetFrameTime());
  m_Position.x = Lerp(m_Position.x, balance, t);
}

bool Human::BodyMoveDone() const { return m_MoveTimer == m_MoveDuration; }

Human::ArmState Human::LeftArmState() const { return m_LeftArmAction.state; }
Human::ArmState Human::RightArmState() const { return m_RightArmAction.state; }

void Human::DrawTargetIndicators() const {
  auto drawIndicator = [](const ArmAction &armAction, const Hand &hand) {
    // No callbacks means the action can't hurt anything (idle typing slaps,
    // or an attack that has already been parried)
    if (armAction.state == ArmState::Any || armAction.callbacks.empty())
      return;

    float t = (hand.duration > 0.0f) ? hand.timer / hand.duration : 1.0f;

    Color color;
    float radius;
    switch (armAction.windupState) {
    case ArmAction::WindupState::Begin:
    case ArmAction::WindupState::Release:
      // Winding up: fade in and warm up as the hit gets closer
      color = ColorLerp(YELLOW, ORANGE, t);
      color = Fade(color, 0.3f + 0.4f * t);
      radius = Lerp(45.0f, 25.0f, t);
      break;
    case ArmAction::WindupState::Windup:
    case ArmAction::WindupState::Done:
      // Released: the hit is about to land
      color = RED;
      radius = 20.0f;
      break;
    }

    Vector2 target = armAction.target;
    DrawCircleV(target, radius, Fade(color, color.a / 255.0f * 0.35f));
    DrawRing(target, radius - 3.0f, radius, 0.0f, 360.0f, 32, color);
    DrawLineEx(target - Vector2{8.0f, 0.0f}, target + Vector2{8.0f, 0.0f},
               2.0f, color);
    DrawLineEx(target - Vector2{0.0f, 8.0f}, target + Vector2{0.0f, 8.0f},
               2.0f, color);
  };

  drawIndicator(m_LeftArmAction, m_LeftHand);
  drawIndicator(m_RightArmAction, m_RightHand);
}
