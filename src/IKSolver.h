#pragma once

#include <raymath.h>

class IKSolver {
public:
  struct Input {
    Vector2 root = Vector2Zero();
    Vector2 mid = Vector2Zero();
    Vector2 effect = Vector2Zero();

    Vector2 pole = Vector2Zero();

    float epsilon = EPSILON;
  };

  struct Output {
    Vector2 mid = Vector2Zero();
    Vector2 effect = Vector2Zero();
  };

public:
  Output solve(const Input &input, Vector2 target);
};
