#include <raylib.h>

#include "IKSolver.h"

IKSolver::Output IKSolver::solve(const Input &input, Vector2 target) {
  float eps2 = EPSILON * EPSILON;
  if (Vector2LengthSqr(input.root - input.pole) <= eps2) {
    TraceLog(LOG_FATAL, "Root cannot be equal to pole!");
  }

  bool zeroLengthBoneEncountered =
      Vector2LengthSqr(input.mid - input.root) <= eps2 ||
      Vector2LengthSqr(input.effect - input.mid) <= eps2;

  if (zeroLengthBoneEncountered) {
    TraceLog(LOG_FATAL, "Encountered a zero length bone!");
  }

  float s = Vector2CrossProduct(input.effect - input.root,
                                input.pole - input.root) > 0.0f
                ? 1
                : -1.0f;

  float rootToMid = Vector2Length(input.mid - input.root);
  float midToEffect = Vector2Length(input.effect - input.mid);

  float rootToTarget = Clamp(Vector2Length(target - input.root),
                             abs(rootToMid - midToEffect) + input.epsilon,
                             rootToMid + midToEffect - input.epsilon);

  float cosAlpha = (rootToMid * rootToMid + rootToTarget * rootToTarget -
                    midToEffect * midToEffect) /
                   (2 * rootToMid * rootToTarget);

  float cosBeta = (rootToMid * rootToMid + midToEffect * midToEffect -
                   rootToTarget * rootToTarget) /
                  (2 * rootToMid * midToEffect);

  float alpha = acos(Clamp(cosAlpha, -1.0f, 1.0f));
  float beta = acos(Clamp(cosBeta, -1.0f, 1.0f));

  Vector2 rootTargetLine = target - input.root;

  float rootAngle = atan2(rootTargetLine.y, rootTargetLine.x);
  float rootDelta = rootAngle + s * alpha;
  float midDelta = rootDelta + s * (beta - PI);

  Output output;
  output.mid = input.root + rootToMid * Vector2(cos(rootDelta), sin(rootDelta));
  output.effect =
      output.mid + midToEffect * Vector2(cos(midDelta), sin(midDelta));

  return output;
}
