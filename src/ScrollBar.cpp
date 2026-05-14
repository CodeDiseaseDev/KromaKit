#include <kromakit/ScrollBar.h>
#include <kromakit/styling/controls/ScrollBarStyle.h>

float ScrollBar::get_track_h()
{
  float available_track_h = size.height - track_spacing;
  return DirectUIUtils::clamp<float>(
    (scrollTarget->size.height / scrollTarget->contentLayoutSize.height) * available_track_h,
    0, available_track_h
  );
}

void ScrollBar::OnRender(
  Graphics* rendTarget)
{
  if (scrollTarget == nullptr)
  {
    rendTarget->DrawErrorX(this, "ScrollBar instance is not attached any control.");
    return;
  }

  if (scrollTarget->size.height >= scrollTarget->contentLayoutSize.height)
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
    
  float available_track_h = size.height - track_spacing;
  float track_bar_h = get_track_h();
  
  float width = size.width - track_spacing;
  float track_cr = width / 2;
  
  float y_offset = (available_track_h - track_bar_h) * ScrollBarValue;


  rendTarget->FillRoundedRectangle(
    size.width / 2 - width / 2, track_spacing / 2 + y_offset,
    width, track_bar_h,
    foreground, track_cr
  );
}

bool ScrollBar::WantsClickCapture() const {
  return true;
}

void ScrollBar::DoLayout(Graphics* renderTarget)
{

}

void ScrollBar::OnMouseMove(int x, int y)
{
  static int last_x = x,
             last_y = y;

  if (scrollTarget == nullptr)
    return;

  if (scrollTarget->size.height >= scrollTarget->contentLayoutSize.height)
  {
    return;
  }

  float available_track_h = size.height - track_spacing;

  if (isMouseDown)
  {
    float deltaX = x - last_x;
    float deltaY = y - last_y;

    float track_bar_h = get_track_h();
    float normalizedDelta = deltaY / (available_track_h - track_bar_h);

    ScrollBarValue += normalizedDelta;

    ScrollBarValue = DirectUIUtils::clamp<float>(
      ScrollBarValue, 0, 1);
  }
  
  scrollTarget->scrollOffset.y = -ScrollBarValue *
    (scrollTarget->contentLayoutSize.height - size.height);

  // printf("%.2f\n", ScrollBarValue);

  last_x = x;
  last_y = y;
}

void ScrollBar::OnMouseDown()
{
  
}

void ScrollBar::OnMouseUp()
{
}

ScrollBar::ScrollBar()
{
  UseStyle(DUIDefaultStyles::ScrollBarStyles::Default);
}

std::optional<std::string> ScrollBar::GetControlName() const {
  return "ScrollBar";
}

const std::vector<std::string> ScrollBar::GetDebugOverlayItems() {
  return {};
}
