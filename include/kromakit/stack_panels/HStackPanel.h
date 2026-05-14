#pragma once
#include <kromakit/Control.h>
#include <kromakit/StretchMode.h>
#include <kromakit/gesture/scrolling/SwipeMomentum.h>
#include <kromakit/layout/layout_types/IHStackLayout.h>

class HStackPanel :
    public Control,
    public IHStackLayout
{
public:
  HStackPanel();

  StackPanelStretchMode ContentStretchMode = StretchModeAlignLeft;

  void SetSpacing(float value);



  DUISize Measure(const DUISize& available) override;


  std::vector<LayoutItem> layoutItems;

  void BuildLayoutItems();

  void DoLayout(Graphics* renderTarget) override;
  LayoutResult DoLayout2(Graphics* renderTarget) override;

  bool NeedsLayoutWhenOwnSizeChanges() const override {
    return true;
  }

private:
  LayoutAlignment GetVerticalAlignmentForStretchMode() const;

  void OnRender(Graphics* rendTarget) override;

  bool WantsClickCapture() const override;

  std::optional<std::string> GetControlName() const override;
  const std::vector<std::string> GetDebugOverlayItems() override;
};

