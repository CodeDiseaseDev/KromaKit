//
// Created by code on 5/8/26.
//

#ifndef TEST_IOS_APP_WHEELSCROLLEVENT_H
#define TEST_IOS_APP_WHEELSCROLLEVENT_H

#include <kromakit/event_types/IEventType.h>
#include <kromakit/graphics/types/DUIGeometry.h>

struct WheelScrollEvent : public IEventType {
  DUIPoint position;
  DUIPoint globalPosition;
  DUIPoint delta;
  bool isPrecise = false;
};

#endif //TEST_IOS_APP_WHEELSCROLLEVENT_H
