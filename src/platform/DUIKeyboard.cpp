//
// Created by code on 5/9/26.
//

#include <kromakit/platform/DUIKeyboard.h>

void DUIKeyboard::OnKeyDown(const DUIKeyEvent &event) {
  DUIKeyEvent updated = event;
  updated.pressed = true;

  UpdateModifierState(updated, true);

  if (OnKeyDownEvent != nullptr)
    OnKeyDownEvent(updated);

  HandleControlKey(updated);

  // char32_t c = ToCharacter(updated);
  // if (c != 0 && OnTextInput != nullptr) {
  //   DUITextInputEvent textEvent;
  //   textEvent.codepoint = c;
  //   OnTextInput(textEvent);
  // }
}

void DUIKeyboard::OnKeyUp(const DUIKeyEvent &event) {
  DUIKeyEvent updated = event;
  updated.pressed = false;

  UpdateModifierState(updated, false);

  if (OnKeyUpEvent != nullptr)
    OnKeyUpEvent(updated);
}

bool DUIKeyboard::IsShiftHeld() const {
  return isShiftHeld;
}

bool DUIKeyboard::IsCtrlHeld() const {
  return isCtrlHeld;
}

bool DUIKeyboard::IsAltHeld() const {
  return isAltHeld;
}

bool DUIKeyboard::IsSuperHeld() const {
  return isSuperHeld;
}

bool DUIKeyboard::IsCapsLockEnabled() const {
  return isCapsLock;
}

char32_t DUIKeyboard::ToCharacter(const DUIKeyEvent &event) const {
  const bool shift = event.shift || isShiftHeld;
  const bool caps = event.capsLock || isCapsLock;
  const bool capitalLetter = shift ^ caps;

  if (event.ctrl || event.alt || event.super || isCtrlHeld || isAltHeld || isSuperHeld)
    return 0;

  if (event.key >= DUIKey::Num0 && event.key <= DUIKey::Num9) {
    if (shift) {
      static constexpr char shiftedNumbers[10] = {
        ')', '!', '@', '#', '$',
        '%', '^', '&', '*', '('
      };

      return shiftedNumbers[static_cast<int>(event.key) - static_cast<int>(DUIKey::Num0)];
    }

    return U'0' + (static_cast<int>(event.key) - static_cast<int>(DUIKey::Num0));
  }

  if (event.key >= DUIKey::A && event.key <= DUIKey::Z) {
    return capitalLetter
             ? U'A' + (static_cast<int>(event.key) - static_cast<int>(DUIKey::A))
             : U'a' + (static_cast<int>(event.key) - static_cast<int>(DUIKey::A));
  }

  auto it = specialCharacters.find(event.key);
  if (it != specialCharacters.end()) {
    if (shift && it->second.shifted != 0)
      return it->second.shifted;

    return it->second.normal;
  }

  return 0;
}

void DUIKeyboard::UpdateModifierState(const DUIKeyEvent &event, bool pressed) {
  switch (event.key) {
    case DUIKey::Shift:
      isShiftHeld = pressed;
      break;

    case DUIKey::Control:
      isCtrlHeld = pressed;
      break;

    case DUIKey::Alt:
      isAltHeld = pressed;
      break;

    case DUIKey::Super:
      isSuperHeld = pressed;
      break;

    case DUIKey::CapsLock:
      if (pressed)
        isCapsLock = !isCapsLock;
      break;

    default:
      break;
  }
}

void DUIKeyboard::HandleControlKey(const DUIKeyEvent &event) {
  switch (event.key) {
    case DUIKey::Backspace:
      if (OnBackspace != nullptr)
        OnBackspace();
      break;

    case DUIKey::Delete:
      if (OnDelete != nullptr)
        OnDelete();
      break;

    case DUIKey::Enter:
      if (OnEnter != nullptr)
        OnEnter();
      break;

    case DUIKey::Tab:
      if (OnTab != nullptr)
        OnTab();
      break;

    case DUIKey::Escape:
      if (OnEscape != nullptr)
        OnEscape();
      break;

    default:
      break;
  }
}
