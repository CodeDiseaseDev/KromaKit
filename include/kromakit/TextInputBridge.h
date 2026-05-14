#pragma once

#include <functional>

namespace DirectUITextInput {
  using KeyboardRequestFn = std::function<void()>;

  void Register(
    KeyboardRequestFn showKeyboard,
    KeyboardRequestFn hideKeyboard
  );

  void ShowKeyboard();
  void HideKeyboard();
}

