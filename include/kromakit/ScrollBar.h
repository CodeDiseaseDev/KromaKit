#pragma once
#include <kromakit/Control.h>
#include <kromakit/IDraggableControl.h>
#include <kromakit/StretchMode.h>

#include <kromakit/Utils.h>

class ScrollBar :
    public IDraggableControl
{
public:
  float ScrollBarValue = 0.0f;
  Control* scrollTarget = nullptr;

  float get_track_h();

  float track_spacing = 7;

  void OnRender(Graphics* rendTarget) override;

  bool WantsClickCapture() const;

  void DoLayout(Graphics* renderTarget) override;

  virtual void OnMouseMove(int x, int y) override;
  virtual void OnMouseDown() override;
  virtual void OnMouseUp() override;



  ScrollBar();

  std::optional<std::string> GetControlName() const override;
  const std::vector<std::string> GetDebugOverlayItems() override;
};

