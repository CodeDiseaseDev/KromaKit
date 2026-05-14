//
// Created by code on 5/10/26.
//

#ifndef TEST_IOS_APP_MACOSKEYBOARD_H
#define TEST_IOS_APP_MACOSKEYBOARD_H

#import <AppKit/NSEvent.h>
#include <functional>
#include <kromakit/event_types/keyboard/DUITextInputEvent.h>
#include <kromakit/event_types/keyboard/DUIKeyEvent.h>

class MacOSKeyboard {
public:
  std::function<void(const DUIKeyEvent&)> OnKeyDown;
  std::function<void(const DUIKeyEvent&)> OnKeyUp;
  std::function<void(const DUITextInputEvent&)> OnTextInput;

  void HandleKeyDown(NSEvent* event);
  void HandleKeyUp(NSEvent* event);

private:
  DUIKey TranslateKey(unsigned short keyCode) const;
  DUIKeyEvent BuildKeyEvent(NSEvent* event, bool pressed) const;
  void SendTextInput(NSEvent* event);
};



#endif //TEST_IOS_APP_MACOSKEYBOARD_H
