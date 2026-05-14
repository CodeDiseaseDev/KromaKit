//
// Created by code on 5/1/26.
//

#include <kromakit/scrollable_stack_panels/ScrollableVStackPanel.h>

#include <kromakit/Utils.h>

ScrollableVStackPanel::ScrollableVStackPanel() {
  SetHorizontalScrollEnabled(false);
  SetVerticalScrollEnabled(true);

  SetContent(&stackPanel);

  lockedAxis = ScrollAxis::Vertical;
  gestureDirectionPreference = ScrollAxis::Vertical;
}

VStackPanel & ScrollableVStackPanel::Stack() {
  return stackPanel;
}

const VStackPanel & ScrollableVStackPanel::Stack() const {
  return stackPanel;
}

void ScrollableVStackPanel::DoLayout(Graphics *renderTarget) {
  const DUISize viewport = GetSize();
  stackPanel.padding = padding;

  const DUISize measured = stackPanel.Measure({
    viewport.width,
    viewport.height
  });

  SetContentSize({
    viewport.width,
    std::max(viewport.height, measured.height)
  });

  ScrollableView::DoLayout(renderTarget);
}

void ScrollableVStackPanel::OnRender(Graphics *rendTarget) {
  ScrollableView::OnRender(rendTarget);
  rendTarget->FillControlBackground(this);
}

void ScrollableVStackPanel::OnPointerMove(const PointerEvent& e) {

  ScrollableView::OnPointerMove(e);
}

void ScrollableVStackPanel::OnScroll(const PointerEvent& e) {

  ScrollableView::OnScroll(e);
}
