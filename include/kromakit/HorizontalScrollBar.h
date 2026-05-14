#pragma once
#include <kromakit/Control.h>
#include <kromakit/StretchMode.h>

#include <kromakit/Utils.h>

class HorizontalScrollBar :
    public Control
{
public:
  float ScrollBarValue = 0.0f;
  Control* scrollTarget = nullptr;

  float get_track_w();

  void OnScroll(float deltaX, float deltaY);

  float track_spacing = 7;

  void OnRender(Graphics* rendTarget) override;
  void DoLayout(Graphics* renderTarget) override;

  virtual void OnMouseMove(int x, int y) override;
  virtual void OnMouseDown() override;
  virtual void OnMouseUp() override;

  std::optional<std::string> GetControlName() const override;
  const std::vector<std::string> GetDebugOverlayItems() override;

  virtual bool WantsScrollCapture() const { return true; }

  HorizontalScrollBar();
};

