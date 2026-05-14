//
// Created by code on 5/9/26.
//

#ifndef TEST_IOS_APP_DUIKEY_H
#define TEST_IOS_APP_DUIKEY_H


enum class DUIKey {
  Unknown,

  // Letters
  A, B, C, D, E, F, G, H, I, J, K, L, M,
  N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

  // Number row
  Num0, Num1, Num2, Num3, Num4,
  Num5, Num6, Num7, Num8, Num9,

  // Text / editing
  Space,
  Enter,
  Backspace,
  Delete,
  Escape,
  Tab,

  // Punctuation / symbols
  Minus,          // -
  Equal,          // =
  Semicolon,      // ;
  Apostrophe,     // '
  Comma,          // ,
  Period,         // .
  Slash,          // /
  Backslash,      // backslash
  Grave,          // `
  BracketLeft,    // [
  BracketRight,   // ]

  // Navigation
  Left,
  Right,
  Up,
  Down,
  Home,
  End,
  PageUp,
  PageDown,
  Insert,

  // Modifiers
  Shift,
  ShiftLeft,
  ShiftRight,

  Control,
  ControlLeft,
  ControlRight,

  Alt,
  AltLeft,
  AltRight,

  Super,
  SuperLeft,
  SuperRight,

  CapsLock,
  NumLock,
  ScrollLock,

  // Function keys
  F1, F2, F3, F4, F5, F6,
  F7, F8, F9, F10, F11, F12,

  // Keypad
  Keypad0,
  Keypad1,
  Keypad2,
  Keypad3,
  Keypad4,
  Keypad5,
  Keypad6,
  Keypad7,
  Keypad8,
  Keypad9,

  KeypadDecimal,
  KeypadDivide,
  KeypadMultiply,
  KeypadSubtract,
  KeypadAdd,
  KeypadEnter,
  KeypadEqual,

  // Menu / system-ish
  Menu,
  PrintScreen,
  Pause,

  // Media keys
  VolumeUp,
  VolumeDown,
  VolumeMute,
  MediaPlayPause,
  MediaStop,
  MediaPrevious,
  MediaNext
};



#endif //TEST_IOS_APP_DUIKEY_H
