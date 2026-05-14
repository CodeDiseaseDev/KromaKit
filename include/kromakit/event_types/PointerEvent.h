//
// Created by code on 4/30/26.
//

#ifndef TEST_IOS_APP_POINTEREVENT_H
#define TEST_IOS_APP_POINTEREVENT_H
#include <kromakit/event_types/IEventType.h>
#include <kromakit/graphics/types/DUIGeometry.h>

enum class MouseButton {
  None,
  Left,
  Right,
  Middle
};

struct PointerEvent : public IEventType {
  DUIPoint position;
  DUIPoint previousPosition;
  DUIPoint startPosition;

  DUIPoint globalPosition;
  DUIPoint previousGlobalPosition;
  DUIPoint startGlobalPosition;

  DUIPoint delta;        // position - previousPosition
  DUIPoint totalDelta;   // position - startPosition

  int pointerId = 0;
  bool isPrimary = true;
  MouseButton mouseButton = MouseButton::None;
};

#endif //TEST_IOS_APP_POINTEREVENT_H
