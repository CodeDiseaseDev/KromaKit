//
// Created by code on 4/28/26.
//

#ifndef TEST_IOS_APP_OVERLAYBACKGROUNDLAYER_H
#define TEST_IOS_APP_OVERLAYBACKGROUNDLAYER_H
#include <kromakit/overlay_background_layer/IOverlayControl.h>
#include <kromakit/Control.h>
#include <kromakit/animation/FloatAnimation.h>

struct OverlayPresentationOptions {
  bool dimBackground = true;
  bool blurBackground = false;
  bool dismissOnBackgroundTap = true;
  bool backdropZoomOutEffect = true;
  bool capturesBackdropClicks = true;

  Color dimBackgroundColor = ColorFromRGB(0, 0, 0, 190);

  static OverlayPresentationOptions DesktopPreset() {
    OverlayPresentationOptions options;
    options.dimBackground = true;
    options.blurBackground = false;
    options.dismissOnBackgroundTap = false;
    options.backdropZoomOutEffect = false;
    options.capturesBackdropClicks = false;
    options.dimBackgroundColor = ColorFromRGB(0,0,0,0);
    return options;
  }
};

class OverlayBackgroundLayer :
  public Control {

  OverlayPresentationOptions options;

  bool isClosing = false;

  float animValue = 0.0f;
  FloatAnimation opacity_animation = FloatAnimation(
    0, 1, 0.15, AnimationCurve::SmoothStep);

  // IOverlayControl *overlayControl;
  void SetIsOpen(bool visible);

  void DismissChildren();
  void ResetAnimation();

public:
  // OverlayBackgroundLayer();
  OverlayBackgroundLayer(IOverlayControl *overlayControl);

  void RenderBackgroundBlur(Graphics *rendTarget, float opacity) const;

  void Present();
  void Dismiss();
  void DismissImmediately();
  void SetDismissOnBackgroundTap(bool enabled);

  void UpdateScaleAnimationValue();

  bool WantsClickCapture() const override { return options.capturesBackdropClicks; }
  bool ReturnsSelfOnHitTestMiss() const override { return options.capturesBackdropClicks; }
  bool RequiresBackdropBlur() const override;

  void OnRender(Graphics *rendTarget) override;
  void DoLayout(Graphics *renderTarget) override;
  void Update(float deltaTime) override;

  std::optional<std::string> GetControlName() const override { return "OverlayBackgroundLayer"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

  void OnMouseUp() override;

  void AddOverlayControl(IOverlayControl *overlayControl);

  bool IsOpenOrClosing();

  OverlayPresentationOptions& GetPresentationOptions() {
    return options;
  }
};



#endif //TEST_IOS_APP_OVERLAYBACKGROUNDLAYER_H
