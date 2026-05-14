//
// Created by code on 5/6/26.
//

#ifndef TEST_IOS_APP_FLOATINGWINDOWCONTROL_H
#define TEST_IOS_APP_FLOATINGWINDOWCONTROL_H

#include <kromakit/Button.h>
#include <kromakit/Label.h>
#include <kromakit/Panel.h>
#include <kromakit/grid_layout_panel/GridLayoutPanel.h>
#include <kromakit/overlay_background_layer/IOverlayControl.h>

#include <functional>
#include <string>
#include <vector>

#include <kromakit/IconButton.h>

class FloatingWindowControl final : public IOverlayControl {
  class ResizeDragControl: public Control {
  public:
    std::function<void(DUIPoint)> OnResizeDrag = nullptr;

    ResizeDragControl() { size = {20,20}; }

    std::optional<std::string> GetControlName() const override { return "ResizeDragControl"; }
    const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

    void OnMouseDrag(float deltaX, float deltaY) override {
      if (OnResizeDrag) {
        OnResizeDrag({deltaX, deltaY});
      }
    }

    bool WantsClickCapture() const override { return true; }
    bool WantsDragCapture() const override { return true; }
    bool IsDraggable() const override { return true; }

    void DoLayout(Graphics *renderTarget) override {

    }
    void OnRender(Graphics *renderTarget) override {
      // renderTarget->FillControlBackground(this);
    }
  };

public:
  GridLayoutPanel* gridLayoutPanel = nullptr;
  GridLayoutPanel* titlePanel = nullptr;
  Label* windowLabel = nullptr;
  IconButton* closeButton = nullptr;
  Panel* contentPanel = nullptr;

  Button* resizeDrag = nullptr;

  ResizeDragControl* resizeBox = nullptr;


  std::function<void()> OnRequestTopMost = nullptr;

  FloatingWindowControl();
  ~FloatingWindowControl() override = default;

  void DoLayout(Graphics* renderTarget) override;
  void OnRender(Graphics* rendTarget) override;
  void OnOverlayDismissed() override;

  bool ShouldClipToBounds() override { return false; }

  std::optional<std::string> GetControlName() const override { return "FloatingWindowControl"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }
  Panel& GetContentPanel() const { return *contentPanel; }

  void SetTitle(const std::string& title);

  bool WantsClickCapture() const override { return true; }
  bool WantsDragCapture() const override { return true; }

  void OnPointerDown(const PointerEvent &e) override;
  void OnPointerUp(const PointerEvent &e) override;
  void OnMouseDrag(float deltaX, float deltaY) override;
  bool IsDraggable() const override { return true; }

  std::function<void()> OnCloseRequested = nullptr;
  std::function<void()> OnOverlayDismissedHook = nullptr;
};

#endif //TEST_IOS_APP_FLOATINGWINDOWCONTROL_H
