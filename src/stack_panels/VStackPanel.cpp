#include <kromakit/stack_panels/VStackPanel.h>

#include <kromakit/gesture/scrolling/ScrollAxis.h>
#include <kromakit/styling/controls/VStackPanelStyle.h>

// VStackPanel::VStackPanel() {
//   UseStyle(DUIDefaultStyles::VStackPanelStyles::Default);
// }
//
// DUISize VStackPanel::UpdateLayout(LayoutResult& result)
// {
//   float layout_y = spacing;
//   float layout_x = spacing;
//
//   for (int i = 0; i < Children.size(); i++)
//   {
//     result.geometryChanged |= Children[i]->SetLayoutLocation({ layout_x, layout_y });
//
//     layout_y += Children[i]->size.height + spacing;
//   }
//
//   layout_x += spacing;
//   // layout_y += spacing;
//
//   result.geometryChanged |= UpdateStretchModeLayout();
//
//   return DUISize { layout_x, layout_y };
// }
//
// void VStackPanel::Update(float deltaTime) {
//   IDraggableControl::Update(deltaTime);
//
//   const float oldScrollY = scrollOffset.y;
//   float extra = scroll_momentum.Tick(deltaTime);
//   scrollOffset.y += extra;
//
//   ClampScroll();
//
//   if (scrollOffset.y != oldScrollY)
//     MarkRenderTreeDirty();
// }
//
// void VStackPanel::OnRender(
//   Graphics* rendTarget)
// {
//   rendTarget->FillControlBackground(this);
//
//   // rendTarget->DrawRoundedRectangle(
//   //   0, 0, size.width, size.height,
//   //   foreground, 6
//   // );
//
//   _deltaTimeCache = rendTarget->DeltaTime;
// }
//
// void VStackPanel::OnScroll(float deltaX, float deltaY) {
//   const float oldScrollY = scrollOffset.y;
//   scrollOffset.y += deltaY;
//   scroll_momentum.Update(deltaY, _deltaTimeCache);
//
//   ClampScroll();
//
//   if (scrollOffset.y != oldScrollY)
//     MarkRenderTreeDirty();
// }
//
// void VStackPanel::OnMouseUp() {
//   IDraggableControl::OnMouseUp();
//
//   scroll_momentum.End();
// }
//
// void VStackPanel::OnMouseDown() {
//   IDraggableControl::OnMouseDown();
//
//   scroll_momentum.Begin();
// }
//
// void VStackPanel::ClampScroll() {
//   float minY = std::min(0.0f, -(contentLayoutSize.height - size.height));
//
//   if (scrollOffset.y > 0)
//     scrollOffset.y = 0;
//
//   if (scrollOffset.y < minY)
//     scrollOffset.y = minY;
// }
//
// void VStackPanel::DoLayout(Graphics* renderTarget)
// {
//   (void)DoLayout2(renderTarget);
// }
//
// LayoutResult VStackPanel::DoLayout2(Graphics* renderTarget)
// {
//   LayoutResult result;
//   result.geometryChanged |= SetLayoutContentSize(UpdateLayout(result));
//   gestureDirectionPreference = ScrollAxis::Vertical;
//   return result;
// }
//
// bool VStackPanel::UpdateStretchModeLayout()
// {
//   switch (ContentStretchMode)
//   {
//   default:
//   case StretchModeAlignLeft:
//     return AlignLeft();
//
//   case StretchModeAlignRight:
//     return AlignRight();
//
//   case StretchModeFill:
//     return AlignStretch();
//
//   case StretchModeCenter:
//     return AlignCenter();
//   }
// }
//
// bool VStackPanel::AlignCenter()
// {
//   bool changed = false;
//   for (int i = 0; i < Children.size(); i++)
//   {
//     DUIPoint loc = Children[i].get()->location;
//     DUISize cSize = Children[i].get()->size;
//
//     loc.x = size.width / 2 - cSize.width / 2;
//     changed |= Children[i]->SetLayoutLocation(loc);
//   }
//
//   return changed;
// }
//
// bool VStackPanel::WantsClickCapture() const {
//   return true;
// }
//
// bool VStackPanel::AlignLeft()
// {
//   bool changed = false;
//   for (int i = 0; i < Children.size(); i++)
//     changed |= Children[i]->SetLayoutLocation({ spacing, Children[i]->location.y });
//
//   return changed;
// }
//
// bool VStackPanel::AlignRight()
// {
//   bool changed = false;
//   for (int i = 0; i < Children.size(); i++)
//     changed |= Children[i]->SetLayoutLocation({
//       size.width - Children[i]->size.width - spacing,
//       Children[i]->location.y
//     });
//
//   return changed;
// }
//
// bool VStackPanel::AlignStretch()
// {
//   bool changed = false;
//   for (int i = 0; i < Children.size(); i++)
//   {
//     changed |= Children[i]->SetLayoutLocation({ spacing, Children[i]->location.y });
//     changed |= Children[i]->SetLayoutSize({
//       size.width - spacing * 2,
//       Children[i]->size.height
//     });
//   }
//
//   return changed;
// }
//
// std::string VStackPanel::GetControlName() const {
//   return "VStackPanel";
// }
//
// const std::vector<std::string> VStackPanel::GetDebugOverlayItems() {
//   return {};
// }
VStackPanel::VStackPanel() {
  UseStyle(DUIDefaultStyles::VStackPanelStyles::Default);
}

void VStackPanel::SetSpacing(float value) {
  IVStackLayout::SetSpacing(value);
  MarkLayoutDirty();
}

LayoutAlignment VStackPanel::GetHorizontalAlignmentForStretchMode() const {
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

void VStackPanel::BuildLayoutItems() {
  IVStackLayout::SetPadding(padding);

  layoutItems.clear();
  layoutItems.reserve(Children.size());

  const LayoutAlignment horizontalAlignment =
      GetHorizontalAlignmentForStretchMode();

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

DUISize VStackPanel::Measure(const DUISize &available) {
  BuildLayoutItems();
  return IStackLayout::Measure(layoutItems, available);
}

void VStackPanel::DoLayout(Graphics* renderTarget) {
  (void)DoLayout2(renderTarget);
}

LayoutResult VStackPanel::DoLayout2(Graphics* renderTarget) {
  LayoutResult result;

  BuildLayoutItems();

  const DUIRect layoutRect {
    0.0f,
    0.0f,
    size.width,
    size.height
  };

  IVStackLayout::Arrange(layoutItems, layoutRect);

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

void VStackPanel::OnRender(Graphics* rendTarget) {
  rendTarget->FillControlBackground(this);
}

bool VStackPanel::WantsClickCapture() const {
  return true;
}

std::optional<std::string> VStackPanel::GetControlName() const {
  return "VStackPanel";
}

const std::vector<std::string> VStackPanel::GetDebugOverlayItems() {
  return {};
}
