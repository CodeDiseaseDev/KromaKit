//
// Created by code on 5/9/26.
//

#ifndef TEST_IOS_APP_DUIKEYEVENT_H
#define TEST_IOS_APP_DUIKEYEVENT_H
#include <kromakit/event_types/keyboard/DUIKey.h>

struct DUIKeyEvent {
  DUIKey key = DUIKey::Unknown;
  bool pressed = false;
  bool repeat = false;

  bool shift = false;
  bool ctrl = false;
  bool alt = false;
  bool super = false;
  bool capsLock = false;
};



#endif //TEST_IOS_APP_DUIKEYEVENT_H
