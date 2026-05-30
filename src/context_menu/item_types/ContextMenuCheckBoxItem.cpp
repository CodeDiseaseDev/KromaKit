//
// Created by code on 5/28/26.
//

#include <kromakit/context_menu/item_types/ContextMenuCheckBoxItem.h>

#include <kromakit/styling/DUIDefaultStyles.h>

#include <utility>

ContextMenuCheckBoxItem::ContextMenuCheckBoxItem() :
  IContextMenuItem() {

  checkbox = CreateControl<Checkbox>();
  label = CreateControl<Label>();

  UseStyle(DUIDefaultStyles::ContextMenuStyle::Child::Item);


  // OnPointerUp = [this](const PointerEvent& e) {
  //
  // };
}

ContextMenuCheckBoxItem::ContextMenuCheckBoxItem(std::string text) :
  ContextMenuCheckBoxItem() {
  SetLabelText(text);
}

ContextMenuCheckBoxItem::ContextMenuCheckBoxItem(std::string text, bool checked) :
  ContextMenuCheckBoxItem(std::move(text)) {
  SetChecked(checked, false);
}

void ContextMenuCheckBoxItem::SetLabelText(const std::string& text) {
  if (label != nullptr) {
    label->SetContent(text);
  }
}

void ContextMenuCheckBoxItem::SetChecked(bool checked, bool notify) {
  if (isChecked == checked) {
    return;
  }

  isChecked = checked;

  if (checkbox != nullptr) {
    checkbox->isChecked = isChecked;
  }

  if (notify && OnCheckedChanged) {
    OnCheckedChanged(isChecked);
  }
}

bool ContextMenuCheckBoxItem::IsChecked() const {
  return isChecked;
}

void ContextMenuCheckBoxItem::Toggle() {
  SetChecked(!isChecked, true);
}

void ContextMenuCheckBoxItem::DoLayout(Graphics* renderTarget) {
  IContextMenuItem::DoLayout(renderTarget);

  if (checkbox == nullptr || label == nullptr) {
    return;
  }

  const float checkboxSize = 18.0f;
  const float gap = 10.0f;
  const float leftPadding = 10.0f;

  const float contentX = padding.left + leftPadding;
  const float contentY = padding.top;
  const float contentHeight = size.height - padding.top - padding.bottom;

  checkbox->location = {
    contentX,
    contentY + (contentHeight - checkboxSize) * 0.5f
  };

  checkbox->size = {
    checkboxSize,
    checkboxSize
  };

  label->location.x = contentX + checkboxSize + gap;

  label->size = {
    size.width - padding.left - padding.right - checkboxSize - gap,
    contentHeight
  };
}

void ContextMenuCheckBoxItem::OnRender(Graphics* rendTarget) {
  rendTarget->FillControlBackground(this);
}

void ContextMenuCheckBoxItem::OnPointerUp(const PointerEvent &e) {
  IContextMenuItem::OnPointerUp(e);

  Toggle();
}
