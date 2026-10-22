#include "HumanController.h"

#include <algorithm>
#include <cmath>

HumanController::HumanController() : m_Model({}, {}) {}

void HumanController::Buzzed() {
  m_BuzzCount += 1;
  if (m_BuzzCount == m_AnnoyedThreshold) {
    m_State = State::AnnoyedWork;
  }
}

void HumanController::SetClosest(bool closest) {
  // All controllers share the same keyboard Sound asset. Only stop it when
  // this controller actually loses closest status; stopping it on every
  // non-closest update would also stop the closest controller's sound.
  if (m_IsClosest && !closest) {
    StopSound(m_Keyboard);
  }
  m_IsClosest = closest;
}

HumanController::HumanController(Vector2 position) : m_Model({}, {}) {
  // Assumption is leg length is 200 units
  // which means that the tweakable leg length parameter
  // is useless LOL
  float initialFootDistance = 50.0f;
  float legLength = 200.0f;
  Vector2 leftFootPos = position - Vector2{initialFootDistance, -legLength};
  Vector2 rightFootPos = position - Vector2{0.0f, -legLength};
  Human::Foot leftFoot = {leftFootPos, leftFootPos, leftFootPos};
  Human::Foot rightFoot = {rightFootPos, rightFootPos, rightFootPos};
  m_Model = Human(leftFoot, rightFoot, 0.0f);
  m_Model.SetPosition(position);
  m_BodyTarget = position;
}

std::function<void(Vector2)>
HumanController::FlyHitCallback(Fly &fly, std::vector<Obstacle> &obstacles) {
  Fly *flyPtr = &fly;
  std::vector<Obstacle> *obstaclePtr = &obstacles;
  auto slapEnd = [this, flyPtr, obstaclePtr](Vector2 handPos) {
    if (!CheckCollisionCircles(handPos, 20.0f, flyPtr->position(), 15)) {
      if (m_State == State::AnnoyedWork) {
        m_PunchCount += 1;
        if (m_PunchCount >= m_ChaseThreshold) {
          m_State = State::Chase;
          StopSound(m_Keyboard);
        }
      }
      // TODO: Destroy objects here?
    } else {
      if (flyPtr->IsParrying()) {
        if (m_State == State::AnnoyedWork) {
          m_State = State::Chase;
          StopSound(m_Keyboard);
        }
        m_Staggered = true;
        m_Model.Parried();
        PlaySound(m_Parry);
        if (m_ParriedCallback) {
          m_ParriedCallback();
        }
      }
      if (!flyPtr->IsInvincible() && !flyPtr->IsParrying()) {
        SlappedFly(flyPtr);
      }
    }

    for (Obstacle &obstacle : *obstaclePtr) {
      Rectangle obstacleBounds = obstacle.area();
      if (CheckCollisionCircleRec(handPos, 40.0f, obstacleBounds)) {
        obstacle.IncrementHitCounter();
        if (obstacle.IsDestroyed()) {
          flyPtr->AddMoney(obstacle.value());
          if (m_ObjectDestroyedCallback) {
            m_ObjectDestroyedCallback(obstacle.value(), obstacle.position());
          }
          m_State = State::Death;
        }
      }
    }
  };

  return slapEnd;
}

void HumanController::Chase(Fly &fly, std::vector<Obstacle> &obstacle) {
  Vector2 flyPosition = fly.position();
  float flyHumanDist = Vector2DistanceSqr(flyPosition, m_Model.GetPosition());
  Vector2 flyToHuman = Vector2Normalize(m_Model.GetPosition() - flyPosition);

  // Yes 30,000 is pretty big but trust :)
  if (flyHumanDist > 30000.0f) {
    float lineDist = 100.0f;
    m_BodyTarget =
        Vector2Lerp(m_BodyTarget, flyPosition + flyToHuman * lineDist, 0.01f);
    m_Model.MoveTo(m_BodyTarget, 0.4f);

    Vector2 rightFoot = m_Model.GetRightFoot();
    Vector2 leftFoot = m_Model.GetLeftFoot();

    Vector2 humanPos = m_Model.GetPosition();
    bool forward = m_Model.GetForward();
    float offset = forward ? 60.0f : -60.0f;

    if (m_Model.RightArmState() == Human::ArmState::Any) {
      m_Model.RightArmTo(Vector2{leftFoot.x + offset, humanPos.y + 30.0f},
                         0.1f);
    }

    if (m_Model.LeftArmState() == Human::ArmState::Any) {
      m_Model.LeftArmTo(Vector2{rightFoot.x + offset, humanPos.y + 30.0f},
                        0.1f);
    }
  } else {
    auto punchEnd = FlyHitCallback(fly, obstacle);
    m_Model.LeftPunch(flyPosition, 0.2, 0.15, {punchEnd});
    m_Model.RightPunch(flyPosition, 0.25, 0.15, {punchEnd});
  }
}

void HumanController::Work(Fly &fly, std::vector<Obstacle> &obstacle) {
  if (m_IsClosest) {
    const float distance = Vector2Distance(GetPosition(), fly.position());
    const float volume =
        std::min(1.0f, std::exp(-distance / m_KeyboardVolumeFalloff));
    SetSoundVolume(m_Keyboard, volume);
  }
  if (m_IsClosest && !IsSoundPlaying(m_Keyboard)) {
    PlaySound(m_Keyboard);
  }
  bool forward = m_Model.GetForward();
  Vector2 position = m_Model.GetPosition() - Vector2(0, 50);
  position += (forward ? Vector2{70.0f, 50.0f} : Vector2{-70.0f, 50.0f});

  m_Model.LeftSlap(position, 0.1, 0.3);
  m_Model.RightSlap(position, 0.2, 0.3);
}

void HumanController::SlappedFly(Fly *fly) {
  if (fly->IsInvincible())
    return;
  fly->Slapped();
}

void HumanController::AnnoyedWork(Fly &fly, std::vector<Obstacle> &obstacle) {
  Vector2 flyPosition = fly.position();
  float flyHumanDist = Vector2DistanceSqr(flyPosition, m_Model.GetPosition());

  if (flyHumanDist > 30000) {
    Work(fly, obstacle);
  } else {
    auto slapEnd = FlyHitCallback(fly, obstacle);
    StopSound(m_Keyboard);
    m_Model.LeftSlap(flyPosition, 0.4, 0.1, {slapEnd});
    m_Model.RightSlap(flyPosition, 0.5, 0.1, {slapEnd});
  }
}

Vector2 HumanController::GetPosition() const { return m_Model.GetPosition(); }

void HumanController::AggressiveStand(Fly &fly,
                                      std::vector<Obstacle> &obstacle) {
  Vector2 flyPosition = fly.position();
  float flyHumanDist = Vector2DistanceSqr(flyPosition, m_Model.GetPosition());

  float humanX = m_Model.GetPosition().x;

  m_Model.SetForward(flyPosition.x >= humanX);

  if (flyHumanDist < 60000) {
    auto punchEnd = FlyHitCallback(fly, obstacle);
    m_Model.LeftPunch(flyPosition, 0.3, 0.15, {punchEnd});
    m_Model.RightPunch(flyPosition, 0.4, 0.15, {punchEnd});
  } else {
    m_Model.StabilizeBody();
  }
}

void HumanController::Death(Fly &fly, std::vector<Obstacle> &obstacle) {
  (void)fly;
  (void)obstacle;
  if (m_DeathDone) {
    return;
  }
  bool forward = true;
  if (GetRandomValue(0, 1) < 1) {
    forward = false;
  }
  m_DeathDone = true;

  Vector2 humanPos = m_Model.GetPosition();
  Vector2 literallyHeaven = {humanPos.x + (forward ? 10000.0f : -10000.0f),
                             0.0f};
  m_Model.MoveTo(literallyHeaven, 5.0f);
}

void HumanController::Update(Fly &fly, std::vector<Obstacle> &obstacles) {
  // TODO: Use obstacles :)
  (void)obstacles;
  if (m_Staggered) {
    m_StaggerTimer += GetFrameTime();

    if (m_StaggerTimer >= m_StaggerDuration) {
      m_StaggerTimer = 0.0f;
      m_Staggered = false;
    }
    return;
  }
  switch (m_State) {
  case State::Work: {
    Work(fly, obstacles);
    break;
  }
  case State::AnnoyedWork: {
    AnnoyedWork(fly, obstacles);
    break;
  }
  case State::Chase: {
    Chase(fly, obstacles);
    break;
  }
  case State::AggressiveStand: {
    AggressiveStand(fly, obstacles);
    break;
  }
  case State::Talk: {
    break;
  }
  case State::Death: {
    Death(fly, obstacles);
    break;
  }
  }
  m_Model.Update();
}

void HumanController::Draw() const {
  float initialX = m_Model.GetPosition().x;
  bool forward = m_Model.GetForward();

  Human::LimbOutput leftLeg = m_Model.GetLeftLeg();
  Human::LimbOutput rightLeg = m_Model.GetRightLeg();

  Human::LimbOutput leftArm = m_Model.GetLeftArm();
  Human::LimbOutput rightArm = m_Model.GetRightArm();

  Vector2 lowerBody = {initialX, leftLeg.root.y};
  Vector2 upperBody = {initialX, leftLeg.root.y};

  Vector2 shoulder = {upperBody.x, upperBody.y - 100.0f};
  Vector2 elbow = {initialX, shoulder.y + 90.0f};
  Vector2 hand = {initialX, elbow.y + 90.0f};

  auto drawLimb = [](Vector2 root, IKSolver::Output limb, Color pointColor,
                     bool hand = false) {
    DrawLineEx(root, limb.mid, 10.0f, BLUE);
    DrawLineEx(limb.mid, limb.effect, 10.0f, BLUE);

    DrawCircleV(root, 10.0f, pointColor);
    DrawCircleV(limb.mid, 10.0f, pointColor);
    if (!hand) {
      DrawCircleV(limb.effect, 10.0f, pointColor);
    } else {
      DrawCircleV(limb.effect, 20.0f, pointColor);
    }
  };

  auto drawBack = [&]() {
    if (forward) {
      drawLimb(leftLeg.root, {leftLeg.mid, leftLeg.effect}, DARKGREEN);
      drawLimb(leftArm.root, {leftArm.mid, leftArm.effect}, DARKGREEN, true);
    } else {
      drawLimb(rightLeg.root, {rightLeg.mid, rightLeg.effect}, GREEN);
      drawLimb(rightArm.root, {rightArm.mid, rightArm.effect}, GREEN, true);
    }
  };

  auto drawFront = [&]() {
    if (forward) {
      drawLimb(rightLeg.root, {rightLeg.mid, rightLeg.effect}, GREEN);
      drawLimb(rightArm.root, {rightArm.mid, rightArm.effect}, GREEN, true);
    } else {
      drawLimb(leftLeg.root, {leftLeg.mid, leftLeg.effect}, DARKGREEN);
      drawLimb(leftArm.root, {leftArm.mid, leftArm.effect}, DARKGREEN, true);
    }
  };

  drawBack();

  Color body = ColorBrightness(BLUE, 0.1);

  // lower and upper body
  DrawCircleV(lowerBody, 45.0f, body);
  DrawCircleV(upperBody - Vector2{0.0f, 90.0f}, 45.0f, body);

  drawFront();

  m_Model.DrawTargetIndicators();
}
