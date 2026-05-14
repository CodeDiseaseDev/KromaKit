//
// Created by code on 5/3/26.
//

#ifndef TEST_IOS_APP_HOLDINTERACTION_H
#define TEST_IOS_APP_HOLDINTERACTION_H

#pragma once

#include <chrono>

class HoldInteraction {
public:
  enum class Result {
    None,
    Tap,
    Hold,
    Cancelled
  };

  using Clock = std::chrono::steady_clock;

  void PointerDown() {
    pointerDown = true;
    holdConsumed = false;
    downTime = Clock::now();
  }

  Result PointerUp() {
    if (!pointerDown)
      return Result::None;

    pointerDown = false;

    if (holdConsumed)
      return Result::Hold;

    const auto now = Clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      now - downTime
    );

    return elapsed >= holdThreshold
      ? Result::Hold
      : Result::Tap;
  }

  void Cancel() {
    pointerDown = false;
    holdConsumed = false;
  }

  bool IsPointerDown() const {
    return pointerDown;
  }

  bool IsHolding() const {
    if (!pointerDown || holdConsumed)
      return false;

    const auto now = Clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      now - downTime
    );

    return elapsed >= holdThreshold;
  }

  bool ConsumeHoldIfReady() {
    if (!IsHolding())
      return false;

    holdConsumed = true;
    return true;
  }

  void SetHoldThresholdMs(int milliseconds) {
    holdThreshold = std::chrono::milliseconds(milliseconds);
  }

  int GetHoldThresholdMs() const {
    return static_cast<int>(holdThreshold.count());
  }

private:
  bool pointerDown = false;
  bool holdConsumed = false;

  Clock::time_point downTime = Clock::now();
  std::chrono::milliseconds holdThreshold { 450 };
};



#endif //TEST_IOS_APP_HOLDINTERACTION_H
