//
// Created by code on 4/15/26.
//

#ifndef TEST_IOS_APP_ANIMATION_H
#define TEST_IOS_APP_ANIMATION_H

struct Animation {
  float time = 0.0f;
  float duration = 0.5f;

  float GetProgress() const {
    return std::clamp(time / duration, 0.0f, 1.0f);
  }

  void SetProgress(float progress) {
    time = duration * progress;
  }

};

#endif //TEST_IOS_APP_ANIMATION_H
