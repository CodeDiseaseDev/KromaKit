#pragma once

#include <kromakit/event_types/keyboard/DUIKey.h>
#include <kromakit/event_types/keyboard/DUIKeyEvent.h>
#include <kromakit/event_types/keyboard/DUITextInputEvent.h>

#if defined(_WIN32) && !defined(GLFW_INCLUDE_NONE)
#define GLFW_INCLUDE_NONE 1
#endif
#include <GLFW/glfw3.h>
#include <functional>

class GLFWKeyboard {
public:
  std::function<void(const DUIKeyEvent&)>       OnKeyDown;
  std::function<void(const DUIKeyEvent&)>       OnKeyUp;
  std::function<void(const DUIKeyEvent&)>       OnKeyRepeat;
  std::function<void(const DUITextInputEvent&)> OnTextInput;

  void HandleKey(
    GLFWwindow* window,
    int         key,
    int         scancode,
    int         action,
    int         mods
  ) {
    DUIKeyEvent event;

    event.key      = TranslateKey(key);
    event.pressed  = action != GLFW_RELEASE;
    event.repeat   = action == GLFW_REPEAT;

    event.shift    = (mods & GLFW_MOD_SHIFT) != 0;
    event.ctrl     = (mods & GLFW_MOD_CONTROL) != 0;
    event.alt      = (mods & GLFW_MOD_ALT) != 0;
    event.super    = (mods & GLFW_MOD_SUPER) != 0;
    event.capsLock = (mods & GLFW_MOD_CAPS_LOCK) != 0;

    if (action == GLFW_PRESS) {
      if (OnKeyDown)
        OnKeyDown(event);
    } else if (action == GLFW_REPEAT) {
      if (OnKeyRepeat)
        OnKeyRepeat(event);
    } else if (action == GLFW_RELEASE) {
      if (OnKeyUp)
        OnKeyUp(event);
    }
  }

  void HandleChar(
    GLFWwindow*  window,
    unsigned int codepoint
  ) {
    DUITextInputEvent event;
    event.codepoint = static_cast<char32_t>(codepoint);

    if (OnTextInput)
      OnTextInput(event);
  }

  static DUIKey TranslateKey(int key) {
    switch (key) {
      case GLFW_KEY_A: return DUIKey::A;
      case GLFW_KEY_B: return DUIKey::B;
      case GLFW_KEY_C: return DUIKey::C;
      case GLFW_KEY_D: return DUIKey::D;
      case GLFW_KEY_E: return DUIKey::E;
      case GLFW_KEY_F: return DUIKey::F;
      case GLFW_KEY_G: return DUIKey::G;
      case GLFW_KEY_H: return DUIKey::H;
      case GLFW_KEY_I: return DUIKey::I;
      case GLFW_KEY_J: return DUIKey::J;
      case GLFW_KEY_K: return DUIKey::K;
      case GLFW_KEY_L: return DUIKey::L;
      case GLFW_KEY_M: return DUIKey::M;
      case GLFW_KEY_N: return DUIKey::N;
      case GLFW_KEY_O: return DUIKey::O;
      case GLFW_KEY_P: return DUIKey::P;
      case GLFW_KEY_Q: return DUIKey::Q;
      case GLFW_KEY_R: return DUIKey::R;
      case GLFW_KEY_S: return DUIKey::S;
      case GLFW_KEY_T: return DUIKey::T;
      case GLFW_KEY_U: return DUIKey::U;
      case GLFW_KEY_V: return DUIKey::V;
      case GLFW_KEY_W: return DUIKey::W;
      case GLFW_KEY_X: return DUIKey::X;
      case GLFW_KEY_Y: return DUIKey::Y;
      case GLFW_KEY_Z: return DUIKey::Z;

      case GLFW_KEY_0: return DUIKey::Num0;
      case GLFW_KEY_1: return DUIKey::Num1;
      case GLFW_KEY_2: return DUIKey::Num2;
      case GLFW_KEY_3: return DUIKey::Num3;
      case GLFW_KEY_4: return DUIKey::Num4;
      case GLFW_KEY_5: return DUIKey::Num5;
      case GLFW_KEY_6: return DUIKey::Num6;
      case GLFW_KEY_7: return DUIKey::Num7;
      case GLFW_KEY_8: return DUIKey::Num8;
      case GLFW_KEY_9: return DUIKey::Num9;

      case GLFW_KEY_SPACE:      return DUIKey::Space;
      case GLFW_KEY_ENTER:      return DUIKey::Enter;
      case GLFW_KEY_BACKSPACE:  return DUIKey::Backspace;
      case GLFW_KEY_DELETE:     return DUIKey::Delete;
      case GLFW_KEY_ESCAPE:     return DUIKey::Escape;
      case GLFW_KEY_TAB:        return DUIKey::Tab;

      case GLFW_KEY_MINUS:         return DUIKey::Minus;
      case GLFW_KEY_EQUAL:         return DUIKey::Equal;
      case GLFW_KEY_SEMICOLON:     return DUIKey::Semicolon;
      case GLFW_KEY_APOSTROPHE:    return DUIKey::Apostrophe;
      case GLFW_KEY_COMMA:         return DUIKey::Comma;
      case GLFW_KEY_PERIOD:        return DUIKey::Period;
      case GLFW_KEY_SLASH:         return DUIKey::Slash;
      case GLFW_KEY_BACKSLASH:     return DUIKey::Backslash;
      case GLFW_KEY_GRAVE_ACCENT:  return DUIKey::Grave;
      case GLFW_KEY_LEFT_BRACKET:  return DUIKey::BracketLeft;
      case GLFW_KEY_RIGHT_BRACKET: return DUIKey::BracketRight;

      case GLFW_KEY_LEFT:      return DUIKey::Left;
      case GLFW_KEY_RIGHT:     return DUIKey::Right;
      case GLFW_KEY_UP:        return DUIKey::Up;
      case GLFW_KEY_DOWN:      return DUIKey::Down;
      case GLFW_KEY_HOME:      return DUIKey::Home;
      case GLFW_KEY_END:       return DUIKey::End;
      case GLFW_KEY_PAGE_UP:   return DUIKey::PageUp;
      case GLFW_KEY_PAGE_DOWN: return DUIKey::PageDown;
      case GLFW_KEY_INSERT:    return DUIKey::Insert;

      case GLFW_KEY_LEFT_SHIFT:    return DUIKey::ShiftLeft;
      case GLFW_KEY_RIGHT_SHIFT:   return DUIKey::ShiftRight;
      case GLFW_KEY_LEFT_CONTROL:  return DUIKey::ControlLeft;
      case GLFW_KEY_RIGHT_CONTROL: return DUIKey::ControlRight;
      case GLFW_KEY_LEFT_ALT:      return DUIKey::AltLeft;
      case GLFW_KEY_RIGHT_ALT:     return DUIKey::AltRight;
      case GLFW_KEY_LEFT_SUPER:    return DUIKey::SuperLeft;
      case GLFW_KEY_RIGHT_SUPER:   return DUIKey::SuperRight;

      case GLFW_KEY_CAPS_LOCK:   return DUIKey::CapsLock;
      case GLFW_KEY_NUM_LOCK:    return DUIKey::NumLock;
      case GLFW_KEY_SCROLL_LOCK: return DUIKey::ScrollLock;

      case GLFW_KEY_F1:  return DUIKey::F1;
      case GLFW_KEY_F2:  return DUIKey::F2;
      case GLFW_KEY_F3:  return DUIKey::F3;
      case GLFW_KEY_F4:  return DUIKey::F4;
      case GLFW_KEY_F5:  return DUIKey::F5;
      case GLFW_KEY_F6:  return DUIKey::F6;
      case GLFW_KEY_F7:  return DUIKey::F7;
      case GLFW_KEY_F8:  return DUIKey::F8;
      case GLFW_KEY_F9:  return DUIKey::F9;
      case GLFW_KEY_F10: return DUIKey::F10;
      case GLFW_KEY_F11: return DUIKey::F11;
      case GLFW_KEY_F12: return DUIKey::F12;

      case GLFW_KEY_KP_0: return DUIKey::Keypad0;
      case GLFW_KEY_KP_1: return DUIKey::Keypad1;
      case GLFW_KEY_KP_2: return DUIKey::Keypad2;
      case GLFW_KEY_KP_3: return DUIKey::Keypad3;
      case GLFW_KEY_KP_4: return DUIKey::Keypad4;
      case GLFW_KEY_KP_5: return DUIKey::Keypad5;
      case GLFW_KEY_KP_6: return DUIKey::Keypad6;
      case GLFW_KEY_KP_7: return DUIKey::Keypad7;
      case GLFW_KEY_KP_8: return DUIKey::Keypad8;
      case GLFW_KEY_KP_9: return DUIKey::Keypad9;

      case GLFW_KEY_KP_DECIMAL:  return DUIKey::KeypadDecimal;
      case GLFW_KEY_KP_DIVIDE:   return DUIKey::KeypadDivide;
      case GLFW_KEY_KP_MULTIPLY: return DUIKey::KeypadMultiply;
      case GLFW_KEY_KP_SUBTRACT: return DUIKey::KeypadSubtract;
      case GLFW_KEY_KP_ADD:      return DUIKey::KeypadAdd;
      case GLFW_KEY_KP_ENTER:    return DUIKey::KeypadEnter;
      case GLFW_KEY_KP_EQUAL:    return DUIKey::KeypadEqual;

      case GLFW_KEY_MENU:         return DUIKey::Menu;
      case GLFW_KEY_PRINT_SCREEN: return DUIKey::PrintScreen;
      case GLFW_KEY_PAUSE:        return DUIKey::Pause;

      default:
        return DUIKey::Unknown;
    }
  }
};
