//
// Created by code on 4/28/26.
//

#ifndef TEST_IOS_APP_IANIMATION_H
#define TEST_IOS_APP_IANIMATION_H

class IAnimation {
  bool running = false;

  float duration = 1.0f;
  float time = 0.0f;

  float const timeScale = 1.0f;

public:
  virtual ~IAnimation() = default;

  virtual void Update(float deltaTime) = 0;

  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void Reset() = 0;

  [[nodiscard]] bool IsRunning() const {
    return running;
  }
  [[nodiscard]] bool IsFinished() const {
    return !running && time >= duration;
  }
};

#endif //TEST_IOS_APP_IANIMATION_H
