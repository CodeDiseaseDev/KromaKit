//
// Created by code on 5/1/26.
//

#include <kromakit/scrollable_stack_panels/ScrollableHStackPanel.h>

#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

ScrollableHStackPanel::ScrollableHStackPanel() {
  SetHorizontalScrollEnabled(true);
  SetVerticalScrollEnabled(false);

  SetContent(&stackPanel);

  stackPanel.UseStyle(DUIDefaultStyles::SurfaceBaseStyles::NeutralSurfaces::Clear);

  lockedAxis = ScrollAxis::Horizontal;
  gestureDirectionPreference = ScrollAxis::Horizontal;
}

HStackPanel & ScrollableHStackPanel::Stack() {
  return stackPanel;
}

const HStackPanel & ScrollableHStackPanel::Stack() const {
  return stackPanel;
}

void ScrollableHStackPanel::DoLayout(Graphics *renderTarget) {
  const DUISize viewport = GetSize();

  const DUISize measured = stackPanel.Measure({
    viewport.width,
    viewport.height
  });

  SetContentSize({
    std::max(viewport.width, measured.width),
    viewport.height
  });

  ScrollableView::DoLayout(renderTarget);
}

void ScrollableHStackPanel::OnRender(Graphics *rendTarget) {
  ScrollableView::OnRender(rendTarget);
  rendTarget->FillControlBackground(this);
}
