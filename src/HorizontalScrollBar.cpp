//
// Created by code on 3/24/26.
//

#include <kromakit/HorizontalScrollBar.h>
#include <kromakit/styling/controls/HorizontalScrollBarStyle.h>

#include <kromakit/stack_panels/VStackPanel.h>


float HorizontalScrollBar::get_track_w()
{
  float available_track_w = size.width - track_spacing;
  return DirectUIUtils::clamp<float>(
    (scrollTarget->size.width / scrollTarget->contentLayoutSize.width) * available_track_w,
    0, available_track_w
  );
}

void HorizontalScrollBar::OnScroll(float deltaX, float deltaY) {
  scrollOffset.x += deltaX;
}

void HorizontalScrollBar::OnRender(Graphics* rendTarget)
{
  if (scrollTarget == nullptr)
  {
    rendTarget->DrawErrorX(this, "HorizontalScrollBar instance is not attached any control.");
    return;
  }

  if (scrollTarget->size.width >= scrollTarget->contentLayoutSize.width)
  {
    ScrollBarValue = 0;
  }

  float corner_radius = std::min(size.width, size.height) / 2;

  rendTarget->FillRoundedRectangle(
    0, 0, size.width, size.height,
    background, corner_radius
  );

  rendTarget->DrawRoundedRectangle(
    0, 0, size.width, size.height,
    foreground, corner_radius
  );

  float available_track_w = size.width - track_spacing;
  float track_bar_w = get_track_w();

  float height = size.height - track_spacing;
  float track_cr = height / 2;

  float x_offset = (available_track_w - track_bar_w) * ScrollBarValue;

  rendTarget->FillRoundedRectangle(
    track_spacing / 2 + x_offset,
    size.height / 2 - height / 2,
    track_bar_w,
    height,
    foreground,
    track_cr
  );
}

void HorizontalScrollBar::DoLayout(Graphics* renderTarget)
{
}

void HorizontalScrollBar::OnMouseMove(int x, int y)
{
  static int last_x = x,
             last_y = y;

  if (scrollTarget == nullptr)
    return;

  if (scrollTarget->size.width >= scrollTarget->contentLayoutSize.width)
  {
    return;
  }

  float available_track_w = size.width - track_spacing;

  if (isMouseDown)
  {
    float deltaX = x - last_x;

    float track_bar_w = get_track_w();
    float normalizedDelta = deltaX / (available_track_w - track_bar_w);

    ScrollBarValue += normalizedDelta;

    ScrollBarValue = DirectUIUtils::clamp<float>(
      ScrollBarValue, 0, 1);
  }

  scrollTarget->scrollOffset.x = -ScrollBarValue *
    (scrollTarget->contentLayoutSize.width - size.width);

  last_x = x;
  last_y = y;
}

void HorizontalScrollBar::OnMouseDown()
{
}

void HorizontalScrollBar::OnMouseUp()
{
}

std::optional<std::string> HorizontalScrollBar::GetControlName() const {
  return "HorizontalScrollBar";
}

const std::vector<std::string> HorizontalScrollBar::GetDebugOverlayItems() {
  return {};
}

HorizontalScrollBar::HorizontalScrollBar()
{
  UseStyle(DUIDefaultStyles::HorizontalScrollBarStyles::Default);
}
