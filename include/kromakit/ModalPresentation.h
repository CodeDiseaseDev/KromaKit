//
// Created by code on 4/15/26.
//

#ifndef TEST_IOS_APP_MODALPRESENTATION_H
#define TEST_IOS_APP_MODALPRESENTATION_H
#include <kromakit/Control.h>
#include <memory>

#include <kromakit/Animation.h>
#include <kromakit/Button.h>
#include <kromakit/stack_panels/HStackPanel.h>
#include <kromakit/overlay_background_layer/IOverlayControl.h>
#include <kromakit/Label.h>
#include <kromakit/Panel.h>
#include <kromakit/gesture/SwipeInteraction.h>


class ModalPresentation :
  public IOverlayControl {

  SwipeInteraction swipe;



public:
  ModalPresentation();

  Label* title = nullptr;


  void SetTitle(std::string str);

  float CornerRadius = 14;

  Button* close_btn = nullptr;

  Control* content = nullptr;

  std::optional<std::string> GetControlName() const override { return "ModalPresentation"; }
  const std::vector<std::string> GetDebugOverlayItems() override {return{};}

  bool WantsClickCapture() const override { return true; }



  void Dismiss() = delete;

  Panel* HeaderPanel = nullptr;

  float YSpacing = 30;
  const float YSpacingPadding = 15;

  float XContentSpacing = 0,
        YContentSpacing = 0,
        TitleBarHeight = 45;

  float anim_value = 0.0f;


  [[deprecated("please use CreateContent<T>(...)")]]
  void SetBorrowedContent(Control* content);

  template<typename T, typename... Args>
  T* CreateContent(Args&&... args) {
    content = CreateControl<T>(
      std::forward<Args>(args)...);

    return content;
  }

  // IOverlayControl
  void OnOverlayShown(OverlayBackgroundLayer* layer) override;
  void OnOverlayDismissed() override;

  bool RequiresBackdropBlur() const override { return false; }

  void Update(float dt) override;

  void OnRender(Graphics *rendTarget) override;
  void DoLayout(Graphics *renderTarget) override;
  LayoutResult DoLayout2(Graphics *renderTarget) override;
  bool NeedsLayoutWhenOwnSizeChanges() const override { return true; }
  void OnPointerDown(const PointerEvent& e) override;
  void OnPointerMove(const PointerEvent& e) override;
  void OnPointerUp(const PointerEvent& e) override;
  void OnPointerCancel(const PointerEvent& e) override;

private:
  bool isOpening = true;
  bool pointerSequenceActive_ = false;


  Animation anim_;

  void BeginSwipeAt(const DUIPoint& point);
  void MoveSwipeAt(const DUIPoint& point);
  void EndSwipeAt(const DUIPoint& point);

  bool ShouldAnimationContinue() const;
  void ConfigureSwipe();
  void ResetSwipe();
  float GetDragProgress() const;
  void HandoffSwipeToAnimation(bool shouldDismiss, float dragProgressAtRelease);
};




#endif //TEST_IOS_APP_MODALPRESENTATION_H
