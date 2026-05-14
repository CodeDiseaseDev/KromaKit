//
// Created by code on 4/28/26.
//

#include <kromakit/animation/FloatAnimation.h>

#include <algorithm>

FloatAnimation::FloatAnimation() = default;

FloatAnimation::FloatAnimation(
    float from,
    float to,
    float duration,
    AnimationCurve curve)
  : from(from),
    to(to),
    duration(std::max(duration, 0.0001f)),
    value(from),
    curve(curve) {}

void FloatAnimation::SetRange(float from, float to) {
  this->from = from;
  this->to = to;

  if (!running && time <= 0.0f) {
    value = from;
  } else if (!running && time >= duration) {
    value = to;
  } else {
    value = Lerp(from, to, EvaluateCurve(curve, GetProgress()));
  }
}

void FloatAnimation::SetDuration(float duration) {
  this->duration = std::max(duration, 0.0001f);
  time = std::min(time, this->duration);
}

void FloatAnimation::SetCurve(AnimationCurve curve) {
  this->curve = curve;
  value = Lerp(from, to, EvaluateCurve(curve, GetProgress()));
}

void FloatAnimation::Start() {
  if (time >= duration) {
    time = 0.0f;
    value = from;
  }

  running = true;
}

void FloatAnimation::Stop() {
  running = false;
}

void FloatAnimation::Reset() {
  running = false;
  time = 0.0f;
  value = from;
}

void FloatAnimation::Update(float deltaTime) {
  if (!running) {
    return;
  }

  if (deltaTime <= 0.0f) {
    return;
  }

  time += deltaTime * timeScale;
  if (time >= duration) {
    time = duration;
    running = false;
  }

  value = Lerp(from, to, EvaluateCurve(curve, GetProgress()));
}

float FloatAnimation::GetValue() const {
  return value;
}

float FloatAnimation::GetProgress() const {
  if (duration <= 0.0f) {
    return 1.0f;
  }

  return Clamp01(time / duration);
}

bool FloatAnimation::IsRunning() const {
  return running;
}

bool FloatAnimation::IsFinished() const {
  return !running && time >= duration;
}

float FloatAnimation::EvaluateCurve(AnimationCurve curve, float progress) {
  float t = Clamp01(progress);

  switch (curve) {
    case AnimationCurve::Linear:
      return t;
    case AnimationCurve::EaseIn:
      return t * t;
    case AnimationCurve::EaseOut:
      return 1.0f - (1.0f - t) * (1.0f - t);
    case AnimationCurve::EaseInOut:
      if (t < 0.5f) {
        return 2.0f * t * t;
      }
      return 1.0f - ((-2.0f * t + 2.0f) * (-2.0f * t + 2.0f)) * 0.5f;
    case AnimationCurve::BounceOut:
      return BounceOut(t);
    case AnimationCurve::SmoothStep:
      return t * t * (3.0f - 2.0f * t);
    case AnimationCurve::SmootherStep:
      return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);

  }

  return t;
}

float FloatAnimation::Lerp(float from, float to, float t) {
  return from + (to - from) * t;
}

float FloatAnimation::Clamp01(float value) {
  return std::clamp(value, 0.0f, 1.0f);
}

float FloatAnimation::BounceOut(float t) {
  constexpr float n1 = 7.5625f;
  constexpr float d1 = 2.75f;

  if (t < 1.0f / d1) {
    return n1 * t * t;
  }
  if (t < 2.0f / d1) {
    t -= 1.5f / d1;
    return n1 * t * t + 0.75f;
  }
  if (t < 2.5f / d1) {
    t -= 2.25f / d1;
    return n1 * t * t + 0.9375f;
  }

  t -= 2.625f / d1;
  return n1 * t * t + 0.984375f;
}

LinearFloatAnimation::LinearFloatAnimation()
  : FloatAnimation(0.0f, 1.0f, 1.0f, AnimationCurve::Linear) {}

LinearFloatAnimation::LinearFloatAnimation(
    float from,
    float to,
    float duration)
  : FloatAnimation(from, to, duration, AnimationCurve::Linear) {}

EaseFloatAnimation::EaseFloatAnimation()
  : FloatAnimation(0.0f, 1.0f, 1.0f, AnimationCurve::EaseInOut) {}

EaseFloatAnimation::EaseFloatAnimation(
    float from,
    float to,
    float duration)
  : FloatAnimation(from, to, duration, AnimationCurve::EaseInOut) {}

BounceFloatAnimation::BounceFloatAnimation()
  : FloatAnimation(0.0f, 1.0f, 1.0f, AnimationCurve::BounceOut) {}

BounceFloatAnimation::BounceFloatAnimation(
    float from,
    float to,
    float duration)
  : FloatAnimation(from, to, duration, AnimationCurve::BounceOut) {}
