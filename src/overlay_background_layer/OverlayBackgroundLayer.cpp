//
// Created by code on 4/28/26.
//

#include <kromakit/overlay_background_layer/OverlayBackgroundLayer.h>

// OverlayBackgroundLayer::OverlayBackgroundLayer() = default;

void OverlayBackgroundLayer::SetIsOpen(bool visible) {
  isVisible = visible;
  isHitTestVisible = visible;
}

void OverlayBackgroundLayer::DismissChildren() {
  for (auto& child : Children)
  if (auto* control = dynamic_cast<IOverlayControl*>(child))
    control->OnOverlayDismissed();
}

void OverlayBackgroundLayer::ResetAnimation() {
  opacity_animation.SetRange(0, 1);
  opacity_animation.Reset();
  isClosing = false;
}

OverlayBackgroundLayer::OverlayBackgroundLayer() {
  SetIsOpen(false);
  ResetAnimation();
  SetRenderLayer(RenderLayer::Overlay);
}

OverlayBackgroundLayer::OverlayBackgroundLayer(
  IOverlayControl *overlayControl) {

  SetIsOpen(false);

  if (overlayControl == nullptr) {
    Logging::Log("OverlayBackgroundLayer provided with a nullptr control\n");
    return;
  }

  Logging::Log("BG Overlay created for %s\n",
    overlayControl->GetControlName().value_or("<name unset>").c_str());

  AddOverlayControl(overlayControl);
  SetRenderLayer(RenderLayer::Overlay);
}

void OverlayBackgroundLayer::RenderBackgroundBlur(
  Graphics* rendTarget,
  const float opacity) const {

  rendTarget->SaveDrawingState();
  nvgResetTransform(rendTarget->_RenderTarget);
  nvgResetScissor(rendTarget->_RenderTarget);

  // Example backdrop blur usage inside an existing control:
  // sample the pipeline-provided blurred texture across full screen.
  const int blurImage = rendTarget->GetBackdropBlurImage();
  const DUISize blurTextureSize = rendTarget->GetBackdropBlurTextureSize();

  if (blurImage > 0 &&
      blurTextureSize.width > 0 &&
      blurTextureSize.height > 0) {

    float zoom = 1.0f;

    float w = blurTextureSize.width / zoom;
    float h = blurTextureSize.height / zoom;
    float offsetX = (blurTextureSize.width - w) * 0.5f;
    float offsetY = (blurTextureSize.height - h) * 0.5f;

    NVGpaint blurPaint = nvgImagePattern(
      rendTarget->_RenderTarget, offsetX, offsetY,
      w, h, 0.0f, blurImage, opacity);

    nvgBeginPath(rendTarget->_RenderTarget);
    nvgRect(rendTarget->_RenderTarget, 0, 0,
            rendTarget->viewportSize.width,
            rendTarget->viewportSize.height);
    nvgFillPaint(rendTarget->_RenderTarget, blurPaint);
    nvgFill(rendTarget->_RenderTarget);
  }

  rendTarget->RestoreDrawingState();
}

void OverlayBackgroundLayer::Present() {
  SetIsOpen(true);

  for (auto& child : Children)
  if (auto* control = dynamic_cast<IOverlayControl*>(child))
    control->OnOverlayShown(this);

  MarkLayoutDirty();

  ResetAnimation();
  opacity_animation.Start();
}

void OverlayBackgroundLayer::Dismiss() {
  isClosing = true;

  opacity_animation.SetRange(1, 0);
  opacity_animation.Reset();
  opacity_animation.Start();

  DismissChildren();
}

void OverlayBackgroundLayer::DismissImmediately() {
  SetIsOpen(false);

  DismissChildren();
  ResetAnimation();

  opacity_animation.Reset();
  MarkLayoutDirty();
}

void OverlayBackgroundLayer::SetDismissOnBackgroundTap(const bool enabled) {
  options.dismissOnBackgroundTap = enabled;
}

void OverlayBackgroundLayer::UpdateScaleAnimationValue() {
  for (auto& child : Children)
  if (auto* control = dynamic_cast<IOverlayControl*>(child))
    control->SetCentredScale(mapc(animValue, 0, 1, 0.9, 1));
}

bool OverlayBackgroundLayer::RequiresBackdropBlur() const {
  if (options.blurBackground) return true;
  return Control::RequiresBackdropBlur();
}

void OverlayBackgroundLayer::OnRender(Graphics *rendTarget) {
  if (options.blurBackground) {
    RenderBackgroundBlur(rendTarget, 1.0f);
  }

  if (options.dimBackground) {
    rendTarget->FillRectangle(
      0, 0, size.width, size.height,
      options.dimBackgroundColor);
  }
}

void OverlayBackgroundLayer::DoLayout(
  Graphics *renderTarget) {}

void OverlayBackgroundLayer::Update(float deltaTime) {
  if (isClosing && opacity_animation.GetValue() < 0.1f) {
    isClosing = false;
    DismissImmediately();
  }




  opacity_animation.Update(deltaTime);


  animValue = opacity_animation.IsRunning() ?
      opacity_animation.GetValue() : 1;

  opacity = animValue;
  UpdateScaleAnimationValue();

  MarkVisualDirty();
}

void OverlayBackgroundLayer::OnMouseUp() {
  Control::OnMouseUp();


  if (options.dismissOnBackgroundTap) {
    Dismiss();

  }
}

void OverlayBackgroundLayer::AddOverlayControl(
  IOverlayControl *overlayControl) {
  if (overlayControl == nullptr) {
    return;
  }

  AddBorrowedControl(overlayControl);
  options.dismissOnBackgroundTap = overlayControl->ShouldDismissOnBackgroundTap();

  overlayControl->background_layer = this;
}

bool OverlayBackgroundLayer::IsOpenOrClosing() {
  return (isClosing && opacity_animation.IsRunning())
    || (isVisible && opacity > 0);
}
