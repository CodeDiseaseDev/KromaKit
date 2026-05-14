#pragma once
#include <kromakit/Control.h>

class Panel :
    public Control
{
public:
  Panel();

  void OnRender(Graphics* rendTarget) override;
  void DoLayout(Graphics* renderTarget) override;
  bool NeedsLayoutWhenOwnSizeChanges() const override { return true; }

  void OnMouseDown() override;

  std::optional<std::string> GetControlName() const override;
  const std::vector<std::string> GetDebugOverlayItems() override;

    virtual bool IsDraggable() const { return false; }
    virtual bool WantsDragCapture() const { return false; }
    virtual bool WantsScrollCapture() const { return false; }
    virtual bool WantsClickCapture() const { return false; }
};

