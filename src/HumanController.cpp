#include "HumanController.h"

HumanController::HumanController() : m_Model({}, {}) {}

void HumanController::Buzzed() {
  m_BuzzCount += 1;
  if (m_BuzzCount == m_AnnoyedThreshold) {
    m_State = State::AnnoyedWork;
  }
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

  m_Keyboard = LoadSound("assets/type.wav");
}

void HumanController::Chase(Fly &fly) {
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
    m_Model.LeftPunch(flyPosition, 0.3, 0.3);
    m_Model.RightPunch(flyPosition, 0.4, 0.3);
  }
}

void HumanController::Work(Fly &fly) {
  if (!IsSoundPlaying(m_Keyboard)) {
    PlaySound(m_Keyboard);
  }
  bool forward = m_Model.GetForward();
  Vector2 position = m_Model.GetPosition();
  position += (forward ? Vector2{70.0f, 50.0f} : Vector2{-70.0f, 50.0f});
  m_Model.LeftSlap(position, 0.1, 0.3);
  m_Model.RightSlap(position, 0.2, 0.3);
}

void HumanController::AnnoyedWork(Fly &fly) {
  Vector2 flyPosition = fly.position();
  float flyHumanDist = Vector2DistanceSqr(flyPosition, m_Model.GetPosition());

  if (flyHumanDist > 30000) {
    Work(fly);
  } else {
    StopSound(m_Keyboard);
    m_Model.LeftSlap(flyPosition, 0.4, 0.1);
    m_Model.RightSlap(flyPosition, 0.5, 0.1);
  }
}

Vector2 HumanController::GetPosition() const { return m_Model.GetPosition(); }

void HumanController::AggressiveStand(Fly &fly) {
  Vector2 flyPosition = fly.position();
  float flyHumanDist = Vector2DistanceSqr(flyPosition, m_Model.GetPosition());

  float humanX = m_Model.GetPosition().x;

  m_Model.SetForward(flyPosition.x >= humanX);

  if (flyHumanDist < 60000) {
    m_Model.LeftPunch(flyPosition, 0.3, 0.15);
    m_Model.RightPunch(flyPosition, 0.4, 0.15);
  } else {
    m_Model.StabilizeBody();
  }
}

void HumanController::Death(Fly &fly) {
  if (m_DeathDone) {
    // TODO: Check when body move is done then delete controller
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

void HumanController::Update(Fly &fly) {
  if (IsKeyPressed(KEY_Q)) {
    m_State = State::Death;
  }
  switch (m_State) {
  case State::Work: {
    Work(fly);
    break;
  }
  case State::AnnoyedWork: {
    AnnoyedWork(fly);
    break;
  }
  case State::Chase: {
    Chase(fly);
    break;
  }
  case State::AggressiveStand: {
    AggressiveStand(fly);
    break;
  }
  case State::Talk: {
    break;
  }
  case State::Death: {
    Death(fly);
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

  // lower and upper body
  DrawCircleV(lowerBody, 45.0f, YELLOW);
  DrawCircleV(upperBody - Vector2{0.0f, 90.0f}, 45.0f, YELLOW);

  drawFront();
}
