//
// Created by code on 4/28/26.
//

#ifndef TEST_IOS_APP_OVERLAYSTACK_H
#define TEST_IOS_APP_OVERLAYSTACK_H
#include <kromakit/Control.h>


class OverlayBackgroundLayer;

class OverlayStack :
  public Control {

public:
  ~OverlayStack() override;
  OverlayStack();

  bool HasActiveOverlay() const;

  std::optional<std::string> GetControlName() const override { return "OverlayStack"; }
  const std::vector<std::string> GetDebugOverlayItems() override {return{};}

  void DoLayout(Graphics *renderTarget) override;
  LayoutResult DoLayout2(Graphics *renderTarget) override;
  bool NeedsLayoutWhenOwnSizeChanges() const override { return true; }
  void OnRender(Graphics *rendTarget) override;

  OverlayBackgroundLayer *PushBorrowedLayer(OverlayBackgroundLayer *layer);

  bool WantsClickCapture() const override { return false; }
  bool ReturnsSelfOnHitTestMiss() const override { return false;}

  bool BringLayerToFront(OverlayBackgroundLayer* layer);
};




#endif //TEST_IOS_APP_OVERLAYSTACK_H
