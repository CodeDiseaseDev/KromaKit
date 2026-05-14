//
// Created by code on 4/28/26.
//

#ifndef TEST_IOS_APP_FLOATANIMATION_H
#define TEST_IOS_APP_FLOATANIMATION_H

#include <kromakit/animation/IAnimation.h>

enum class AnimationCurve {
  Linear,
  EaseIn,
  EaseOut,
  EaseInOut,
  BounceOut,
  SmoothStep,
  SmootherStep
};

class FloatAnimation : public IAnimation {
public:
  FloatAnimation();
  FloatAnimation(float from, float to, float duration, AnimationCurve curve);

  void SetRange(float from, float to);
  void SetDuration(float duration);
  void SetCurve(AnimationCurve curve);

  void Start() override;
  void Stop() override;
  void Reset() override;
  void Update(float deltaTime) override;

  [[nodiscard]] float GetValue() const;
  [[nodiscard]] float GetProgress() const;

  // Mirrors the expected IAnimation status semantics using this class state.
  [[nodiscard]] bool IsRunning() const;
  [[nodiscard]] bool IsFinished() const;

protected:
  [[nodiscard]] static float EvaluateCurve(AnimationCurve curve, float progress);

private:
  [[nodiscard]] static float Lerp(float from, float to, float t);
  [[nodiscard]] static float Clamp01(float value);
  [[nodiscard]] static float BounceOut(float t);

  float from = 0.0f;
  float to = 0.0f;

  float duration = 1.0f;
  float time = 0.0f;
  float value = 0.0f;

  float const timeScale = 1.0f;
  bool running = false;
  AnimationCurve curve = AnimationCurve::EaseInOut;
};

class LinearFloatAnimation : public FloatAnimation {
public:
  LinearFloatAnimation();
  LinearFloatAnimation(float from, float to, float duration);
};

class EaseFloatAnimation : public FloatAnimation {
public:
  EaseFloatAnimation();
  EaseFloatAnimation(float from, float to, float duration);
};

class BounceFloatAnimation : public FloatAnimation {
public:
  BounceFloatAnimation();
  BounceFloatAnimation(float from, float to, float duration);
};

#endif // TEST_IOS_APP_FLOATANIMATION_H
