//
// Created by code on 5/9/26.
//

#include <kromakit/IconButton.h>

#include <kromakit/styling/controls/ButtonStyle.h>
#include <kromakit/styling/controls/IconButtonStyle.h>

IconButton::IconButton(std::string_view svg_path) {
  icon_ = CreateControl<SvgIcon>(svg_path);
  icon_->isHitTestVisible = false;

  SetContent(std::string());
  UseStyle(DUIDefaultStyles::IconButtonStyles::Default);
}

void IconButton::SetRadius(float value) {
  radius = value;
  MarkLayoutDirty();
  MarkVisualDirty();
}

void IconButton::OnRender(Graphics *rendTarget) {
  rendTarget->FillControlBackground(this);

}

void IconButton::DoLayout(Graphics *renderTarget) {
  if (icon_ == nullptr) return;

  DUISize s = { radius * 2, radius * 2 };
  DUIPoint l = {
    size.width / 2 - s.width / 2,
    size.height / 2 - s.height / 2
  };

  icon_->SetLayoutFrame(l, s);
}

void IconButton::SetTint(Color color) {
  if (icon_ == nullptr) return;
  icon_->SetTint(color);
}

void IconButton::ClearTint() {
  if (icon_ == nullptr) return;
  icon_->ClearTint();
}
