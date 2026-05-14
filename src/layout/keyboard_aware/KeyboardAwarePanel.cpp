//
// Created by code on 5/9/26.
//

#include <kromakit/layout/keyboard_aware/KeyboardAwarePanel.h>

KeyboardAwarePanel::KeyboardAwarePanel() {
  // panel_ = CreateControl<Panel>();
}

float KeyboardAwarePanel::CalculateKeyboardOverlap() const {
  if (!keyboardVisible)
    return 0.0f;

  return std::max(
    0.0f,
    size.height - keyboardFrame.y);
}

void KeyboardAwarePanel::SetKeyboardFrame(DUIRect keyboardFrameInWindow) {
  keyboardFrame = keyboardFrameInWindow;
  MarkLayoutDirty();
}

void KeyboardAwarePanel::SetKeyboardVisible(bool visible) {
  if (keyboardVisible == visible)
    return;

  keyboardVisible = visible;
  MarkLayoutDirty();
}

void KeyboardAwarePanel::OnRender(Graphics *rendTarget) {
}

void KeyboardAwarePanel::DoLayout(Graphics* renderTarget) {
  if (borrowedChild_ == nullptr)
    return;

  const float keyboardOverlap = CalculateKeyboardOverlap();

  const float availableHeight = std::max(
    0.0f,
    size.height - keyboardOverlap);

  borrowedChild_->SetLayoutLocation({0.0f, 0.0f});
  borrowedChild_->SetLayoutSize({
    size.width,
    availableHeight
  });

  contentLayoutSize = {
    size.width,
    availableHeight
  };
}

void KeyboardAwarePanel::Update(float deltaTime) {
  Control::Update(deltaTime);
}

