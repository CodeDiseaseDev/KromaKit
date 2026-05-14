//
// Created by code on 4/27/26.
//

#include <kromakit/context_menu/IContextMenuItem.h>

#include <utility>

#include <cmath>
#include <kromakit/styling/controls/ContextMenuStyle.h>


IContextMenuItem::IContextMenuItem() = default;

void IContextMenuItem::SetLabelText(std::string str) {
  label->SetContent(std::move(str));
}

void IContextMenuItem::DoLayout(Graphics *renderTarget) {

  label->location = {
    LabelXSpacing,
    size.height / 2 - label->size.height / 2
  };
}
void IContextMenuItem::Update(float deltaTime) {
  Control::Update(deltaTime);

  tapAnimation.Update(deltaTime);

  if (tapAnimation.IsRunning() && !tapAnimation.IsFinished()) {
    background = tapColor;
    background.a = tapAnimation.GetValue();
    return;
  }

  if (isMouseDown) {
    background = tapColor;
    background.a = 1.0f;
    return;
  }

  background.a = 0.0f;
}

void IContextMenuItem::OnPointerDown(const PointerEvent& e) {
  Control::OnPointerDown(e);

  if (e.mouseButton != MouseButton::Left) return;

  background = tapColor;
  background.a = 1.0f;

  MarkVisualDirty();
}

void IContextMenuItem::OnPointerUp(const PointerEvent& e) {
  Control::OnPointerUp(e);

  if (e.mouseButton != MouseButton::Left) return;

  tapAnimation.Start();

  MarkVisualDirty();

  if (OnClick != nullptr) {
    OnClick();

    if (RequestDismiss != nullptr) {
      RequestDismiss();

      ContextMenuHapticFeedback();
    }
  }
}

void IContextMenuItem::OnPointerCancel(const PointerEvent& e) {
  Control::OnPointerCancel(e);

  background.a = 0.0f;

  MarkVisualDirty();
}