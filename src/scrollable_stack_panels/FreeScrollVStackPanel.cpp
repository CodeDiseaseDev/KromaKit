//
// Created by code on 5/1/26.
//

#include <kromakit/scrollable_stack_panels/FreeScrollVStackPanel.h>

#include <kromakit/Utils.h>

FreeScrollVStackPanel::FreeScrollVStackPanel() {
  SetHorizontalScrollEnabled(true);
  SetVerticalScrollEnabled(true);

  SetContent(&stackPanel);

  lockedAxis = ScrollAxis::Free;
  gestureDirectionPreference = ScrollAxis::Free;
}

VStackPanel & FreeScrollVStackPanel::Stack() {
  return stackPanel;
}

const VStackPanel & FreeScrollVStackPanel::Stack() const {
  return stackPanel;
}

void FreeScrollVStackPanel::DoLayout(Graphics *renderTarget) {
  const DUISize viewport = GetSize();
  stackPanel.padding = padding;

  const DUISize measured = stackPanel.Measure({
    viewport.width,
    viewport.height
  });

  SetContentSize({
    std::max(viewport.width, measured.width),
    std::max(viewport.height, measured.height)
  });

  ScrollableView::DoLayout(renderTarget);
}

void FreeScrollVStackPanel::OnRender(Graphics *rendTarget) {
  ScrollableView::OnRender(rendTarget);
  rendTarget->FillControlBackground(this);
}

void FreeScrollVStackPanel::OnPointerMove(const PointerEvent& e) {

  ScrollableView::OnPointerMove(e);
}

void FreeScrollVStackPanel::OnScroll(const PointerEvent& e) {

  ScrollableView::OnScroll(e);
}
