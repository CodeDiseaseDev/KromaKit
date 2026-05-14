//
// Created by code on 5/9/26.
//

#include <kromakit/TextInputBridge.h>

namespace {
  DirectUITextInput::KeyboardRequestFn gShowKeyboard;
  DirectUITextInput::KeyboardRequestFn gHideKeyboard;
}

namespace DirectUITextInput {

  inline bool isKeyboardVisible = false;

  void Register(
    KeyboardRequestFn showKeyboard,
    KeyboardRequestFn hideKeyboard
  ) {
    gShowKeyboard = std::move(showKeyboard);
    gHideKeyboard = std::move(hideKeyboard);
  }

  void ShowKeyboard() {
    if (gShowKeyboard != nullptr) {
      gShowKeyboard();
      isKeyboardVisible = true;
    }
  }

  void HideKeyboard() {
    if (gHideKeyboard != nullptr) {
      gHideKeyboard();
      isKeyboardVisible = false;
    }
  }

}
