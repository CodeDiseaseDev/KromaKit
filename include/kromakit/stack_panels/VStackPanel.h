
#pragma once

#include <kromakit/Control.h>
#include <kromakit/layout/LayoutItem.h>
#include <kromakit/layout/LayoutResult.h>
#include <kromakit/StretchMode.h>
#include <kromakit/layout/layout_types/IVStackLayout.h>

class VStackPanel :
    public Control,
    public IVStackLayout
{
public:
  VStackPanel();

  StackPanelStretchMode ContentStretchMode = StretchModeAlignLeft;

  void SetSpacing(float value);

  DUISize Measure(const DUISize& available) override;

private:
  std::vector<LayoutItem> layoutItems;

  void BuildLayoutItems();

  void DoLayout(Graphics* renderTarget) override;
  LayoutResult DoLayout2(Graphics* renderTarget) override;

  bool NeedsLayoutWhenOwnSizeChanges() const override {
    return true;
  }

  LayoutAlignment GetHorizontalAlignmentForStretchMode() const;

  void OnRender(Graphics* rendTarget) override;

  bool WantsClickCapture() const override;

  std::optional<std::string> GetControlName() const override;
  const std::vector<std::string> GetDebugOverlayItems() override;
};









// #include "scrolling/SwipeMomentum.h"

// class VStackPanel :
//     public IDraggableControl
// {
// public:
//   VStackPanel();
//
//   // TODO: deprecate `spacing` property and use Control::marginSpacing.
//   // TODO: implement new `DUIProperty<T>` "reactive property" class.
//
//   DUISize stack_layout_size;
//   StackPanelStretchMode ContentStretchMode = StretchModeAlignLeft;
//   float spacing = 25;
//
//   SwipeMomentum scroll_momentum;
//
//   float GetUsableWidth() const { return size.width - spacing * 2; }
//
// private:
//   float _deltaTimeCache = 0.0f;
//   DUISize UpdateLayout(LayoutResult& result);
//
//   void Update(float deltaTime) override;
//
//   virtual void OnRender(
//     Graphics* rendTarget) override;
//
//   void DoLayout(Graphics* renderTarget) override;
//   LayoutResult DoLayout2(Graphics* renderTarget) override;
//   bool NeedsLayoutWhenOwnSizeChanges() const override { return true; }
//
//   bool UpdateStretchModeLayout();
//   bool AlignCenter();
//
//   bool WantsClickCapture() const;
//   virtual bool WantsScrollCapture() const {
//     return contentLayoutSize.height > size.height;
//   }
//
//   bool AlignLeft();
//   bool AlignRight();
//   bool AlignStretch();
//
//   void OnScroll(float deltaX, float deltaY) override;
//   void OnMouseUp() override;
//   void OnMouseDown() override;
//
//   void ClampScroll();
//
//   std::string GetControlName() const override;
//   const std::vector<std::string> GetDebugOverlayItems() override;
// };


