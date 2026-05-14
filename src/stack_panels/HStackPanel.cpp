#include <kromakit/stack_panels/HStackPanel.h>

#include <kromakit/stack_panels/VStackPanel.h>
// #include "../IDraggableControl.h"
#include <kromakit/styling/controls/HStackPanelStyle.h>
#include <kromakit/layout/layout_types/IHStackLayout.h>


HStackPanel::HStackPanel() {
  UseStyle(DUIDefaultStyles::HStackPanelStyles::Default);

}

void HStackPanel::SetSpacing(float value) {
  IHStackLayout::SetSpacing(value);
  MarkLayoutDirty();
}

LayoutAlignment HStackPanel::GetVerticalAlignmentForStretchMode() const {
  switch (ContentStretchMode) {
    default:
    case StretchModeAlignLeft:
      return LayoutAlignment::Start;

    case StretchModeAlignRight:
      return LayoutAlignment::End;

    case StretchModeCenter:
      return LayoutAlignment::Center;

    case StretchModeFill:
      return LayoutAlignment::Stretch;
  }
}

void HStackPanel::BuildLayoutItems() {
  IHStackLayout::SetPadding(padding);

  layoutItems.clear();
  layoutItems.reserve(Children.size());

  const LayoutAlignment horizontalAlignment =
      GetVerticalAlignmentForStretchMode();

  for (auto& child : Children) {
    if (!child)
      continue;

    LayoutItem item;
    item.control = child;
    item.margin = child->margin;
    item.padding = child->padding;

    item.options.horizontalAlignment = horizontalAlignment;
    item.options.verticalAlignment = LayoutAlignment::Start;

    // TODO: migrate old `spacing` into Control::marginSpacing or LayoutItem margin.
    // For now spacing is handled by StackLayout::SetSpacing.

    layoutItems.push_back(item);
  }
}

DUISize HStackPanel::Measure(const DUISize &available) {
  BuildLayoutItems();
  return IStackLayout::Measure(layoutItems, available);
}

void HStackPanel::DoLayout(Graphics* renderTarget) {
  (void)DoLayout2(renderTarget);
}

LayoutResult HStackPanel::DoLayout2(Graphics* renderTarget) {
  LayoutResult result;

  BuildLayoutItems();

  const DUIRect layoutRect {
    0.0f,
    0.0f,
    size.width,
    size.height
  };

  IHStackLayout::Arrange(layoutItems, layoutRect);

  const DUISize measuredSize = IStackLayout::Measure(
    layoutItems,
    {
      size.width,
      size.height
    }
  );

  result.geometryChanged |= SetLayoutContentSize(measuredSize);

  return result;
}

void HStackPanel::OnRender(Graphics* rendTarget) {
  rendTarget->FillControlBackground(this);
}

bool HStackPanel::WantsClickCapture() const {
  return true;
}

std::optional<std::string> HStackPanel::GetControlName() const {
  return "HStackPanel";
}

const std::vector<std::string> HStackPanel::GetDebugOverlayItems() {
  return {};
}
