//
// Created by code on 5/9/26.
//

#ifndef TEST_IOS_APP_DUIDESKTOPGLFWKEYBOARD_H
#define TEST_IOS_APP_DUIDESKTOPGLFWKEYBOARD_H

#include <functional>
#include <map>

#include <kromakit/event_types/keyboard/DUIKey.h>
#include <kromakit/event_types/keyboard/DUIKeyEvent.h>
#include <kromakit/event_types/keyboard/DUITextInputEvent.h>

struct SpecialCharacterKey {
  char normal = 0;
  char shifted = 0;
};

class DUIKeyboard {
public:
  std::function<void(const DUITextInputEvent&)> OnTextInput;
  std::function<void(const DUIKeyEvent&)> OnKeyDownEvent;
  std::function<void(const DUIKeyEvent&)> OnKeyUpEvent;

  std::function<void()> OnBackspace;
  std::function<void()> OnDelete;
  std::function<void()> OnEnter;
  std::function<void()> OnTab;
  std::function<void()> OnEscape;

  void OnKeyDown(const DUIKeyEvent& event);
  void OnKeyUp(const DUIKeyEvent& event);
  bool IsShiftHeld() const;
  bool IsCtrlHeld() const;
  bool IsAltHeld() const;
  bool IsSuperHeld() const;
  bool IsCapsLockEnabled() const;

  char32_t ToCharacter(const DUIKeyEvent& event) const;

private:
  bool isShiftHeld = false;
  bool isCtrlHeld = false;
  bool isAltHeld = false;
  bool isSuperHeld = false;
  bool isCapsLock = false;

  inline static const std::map<DUIKey, SpecialCharacterKey> specialCharacters = {
    { DUIKey::Space,        { ' ',  0   } },
    { DUIKey::Minus,        { '-',  '_' } },
    { DUIKey::Equal,        { '=',  '+' } },
    { DUIKey::Semicolon,    { ';',  ':' } },
    { DUIKey::Slash,        { '/',  '?' } },
    { DUIKey::Grave,        { '`',  '~' } },
    { DUIKey::BracketLeft,  { '[',  '{' } },
    { DUIKey::Backslash,    { '\\', '|' } },
    { DUIKey::BracketRight, { ']',  '}' } },
    { DUIKey::Apostrophe,   { '\'', '"' } },
    { DUIKey::Comma,        { ',',  '<' } },
    { DUIKey::Period,       { '.',  '>' } }
  };

  void UpdateModifierState(const DUIKeyEvent& event, bool pressed);
  void HandleControlKey(const DUIKeyEvent& event);
};



#endif //TEST_IOS_APP_DUIDESKTOPGLFWKEYBOARD_H
