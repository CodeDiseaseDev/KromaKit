//
// Created by code on 4/15/26.
//

#include <kromakit/ModalPresentation.h>
#include <cmath>

#include <kromakit/Utils.h>
#include <kromakit/HapticsBridge.h>
#include <kromakit/styling/controls/ButtonStyle.h>
#include <kromakit/styling/controls/ModalPresentationStyle.h>
#include <kromakit/styling/controls/PanelStyle.h>

namespace {
float EaseOut8(float t) {
  return 1.0f - std::pow(1.0f - t, 8.0f);
}

float InverseEaseOut8(float value) {
  return 1.0f - std::pow(1.0f - value, 1.0f / 8.0f);
}

float ResolveModalAnimationHeight(const Control* modal) {
  if (modal == nullptr)
    return 0.0f;

  if (modal->size.height > 0.0f)
    return modal->size.height;

  if (modal->parent != nullptr && modal->parent->size.height > 0.0f)
    return modal->parent->size.height;

  return 0.0f;
}
}

ModalPresentation::ModalPresentation() {
  // _callingControl = caller;
  isVisible = false;

  HeaderPanel = CreateControl<Panel>();
  close_btn = CreateControl<Button>();
  title = HeaderPanel->CreateControl<Label>();

  UseStyle(DUIDefaultStyles::ModalPresentationStyles::Default);
  close_btn->UseStyle(DUIDefaultStyles::ButtonStyles::TextAction);
  close_btn->SetContent("Done");
  close_btn->OnClick = [&] {
    DismissBackgroundLayer();
  };
  
  HeaderPanel->isHitTestVisible = false;
}

void ModalPresentation::SetTitle(std::string str) {
  title->SetContent(str);
}

void ModalPresentation::SetBorrowedContent(Control *ctrl) {
  if (content != nullptr && content != ctrl) {
    content->isVisible = false;
  }

	content = ctrl;
  content->isVisible = true;

	AddBorrowedControl(this->content);
	MarkRenderTreeDirty();
}

void ModalPresentation::OnOverlayShown(
  OverlayBackgroundLayer *layer) {

  anim_.time = 0.0f;
  isVisible = true;
  isOpening = true;
  ResetSwipe();

  anim_value = 0.0f;
  opacity = 1.0f;

  // Use best-known height immediately so first frame doesn't render as open
  // before this control receives its first layout pass.
  renderOffset.y = ResolveModalAnimationHeight(this);

  MarkLayoutDirty();
  MarkRenderTreeDirty();

}

void ModalPresentation::OnOverlayDismissed() {
  isOpening = false;

  DirectUIHaptics::Impact(2);
}

void ModalPresentation::Update(float dt) {
  const bool oldVisible = isVisible;
  const float oldRenderOffsetY = renderOffset.y;
  const float oldOpacity = opacity;

  if (swipe.dragging) {
    swipe.Tick(dt);
  }

  // Advance animation state first, so this frame renders the current state,
  // not the previous/stale state.
  if (ShouldAnimationContinue()) {
    if (isOpening) {
      anim_.time += dt;
      if (anim_.time > anim_.duration)
        anim_.time = anim_.duration;
    } else {
      anim_.time -= dt;
      if (anim_.time <= 0.0f) {
        anim_.time = 0.0f;
        isVisible = false;
      }
    }
  }

  float slideInValue = 0.0f;

  if (swipe.dragging) {
    const float dragProgress = GetDragProgress();
    slideInValue = 1.0f - dragProgress;
  } else {
    const float t = std::clamp(anim_.GetProgress(), 0.0f, 1.0f);
    slideInValue = EaseOut8(t);
  }

  const float resolvedHeight = ResolveModalAnimationHeight(this);
  renderOffset.y = resolvedHeight * (1.0f - slideInValue);

  opacity = 1.0f;
  anim_value = slideInValue;

  if (oldVisible != isVisible ||
      oldRenderOffsetY != renderOffset.y ||
      oldOpacity != opacity) {
    MarkRenderTreeDirty();
  }
}

void ModalPresentation::OnRender(Graphics *rendTarget) {


  DUIInsets cornerRadius = {
    CornerRadius, CornerRadius,
    0,  0
  };

  rendTarget->SaveDrawingState();

  background.a = anim_value;
  rendTarget->FillRoundedRectangle(
    0, 0, size.width, size.height,
    background, cornerRadius);

  rendTarget->RestoreDrawingState();
}

void ModalPresentation::DoLayout(Graphics *renderTarget) {
  (void)DoLayout2(renderTarget);
}

LayoutResult ModalPresentation::DoLayout2(Graphics *renderTarget) {
  LayoutResult result;

  // ios safe area
  YSpacing = renderTarget->viewportSafeArea.top + YSpacingPadding;

  result.geometryChanged |= SetLayoutLocation({
    0, // always to the left
    YSpacing
  });

  result.geometryChanged |= SetLayoutSize({
    parent->size.width,
    parent->size.height - YSpacing
  });

  HeaderPanel->cornerRadius = {
    CornerRadius, CornerRadius, 0, 0
  };
  result.geometryChanged |= HeaderPanel->SetLayoutLocation({
    0, 0
  });
  result.geometryChanged |= HeaderPanel->SetLayoutSize({
    size.width,
    TitleBarHeight
  });


  result.geometryChanged |= close_btn->SetLayoutSize({37,20});
  result.geometryChanged |= close_btn->SetLayoutLocation({
    size.width - close_btn->size.width - 13,
    HeaderPanel->size.height / 2 - close_btn->size.height / 2
  });

  if (content != nullptr) {
    result.geometryChanged |= content->SetLayoutLocation({
      XContentSpacing,
      YContentSpacing + TitleBarHeight
    });
    result.geometryChanged |= content->SetLayoutSize({
      size.width - XContentSpacing * 2,
      size.height - (YContentSpacing * 2) - TitleBarHeight,
    });
  }

  result.geometryChanged |= title->SetLayoutLocation({
    HeaderPanel->size.width / 2 - title->size.width / 2,
    HeaderPanel->size.height / 2 - title->size.height / 2
  });

  return result;
}

void ModalPresentation::OnPointerDown(const PointerEvent& e) {
  Control::OnPointerDown(e);
  pointerSequenceActive_ = true;
  BeginSwipeAt(e.position);
}

void ModalPresentation::OnPointerMove(const PointerEvent& e) {
  Control::OnPointerMove(e);
  if (!pointerSequenceActive_)
    return;
  MoveSwipeAt(e.position);
}

void ModalPresentation::OnPointerUp(const PointerEvent& e) {
  Control::OnPointerUp(e);
  if (e.mouseButton != MouseButton::Left)
    return;
  if (!pointerSequenceActive_)
    return;
  EndSwipeAt(e.position);
  pointerSequenceActive_ = false;
}

void ModalPresentation::OnPointerCancel(const PointerEvent& e) {
  Control::OnPointerCancel(e);
  pointerSequenceActive_ = false;
  ResetSwipe();
}

void ModalPresentation::BeginSwipeAt(const DUIPoint& point) {
  ConfigureSwipe();

  // Seed swipe baseline from the current animated visual state so there is no
  // one-frame jump when touch starts mid-animation.
  float visibleOpen = EaseOut8(std::clamp(anim_.GetProgress(), 0.0f, 1.0f));
  float dragProgress = std::clamp(1.0f - visibleOpen, 0.0f, 1.0f);
  swipe.offset = swipe.maxOffset * dragProgress;

  swipe.TouchStart(point);
}

void ModalPresentation::MoveSwipeAt(const DUIPoint& point) {
  swipe.TouchMove(point);
}

void ModalPresentation::EndSwipeAt(const DUIPoint& point) {
  const float dragProgressAtRelease = std::clamp(GetDragProgress(), 0.0f, 1.0f);
  swipe.TouchEnd(point);
  HandoffSwipeToAnimation(
    swipe.IsCompleting(),
    dragProgressAtRelease);
}

bool ModalPresentation::ShouldAnimationContinue() const {
  if (isOpening)
    return anim_.time < anim_.duration;
  else
    return anim_.time > 0.0f;
}

void ModalPresentation::ConfigureSwipe() {
  swipe.Configure(
    SwipeInteraction::Direction::Vertical,
    size.height, // full dismiss distance
    100.0f
  );
}

void ModalPresentation::ResetSwipe() {
  swipe.Reset();
}

float ModalPresentation::GetDragProgress() const {
  return std::max(0.0f, swipe.GetSignedProgress());
}

void ModalPresentation::HandoffSwipeToAnimation(
  bool shouldDismiss,
  float dragProgressAtRelease) {
  float visibleOpen = 1.0f - dragProgressAtRelease;

  // Convert from rendered (eased) value to animation timeline value so there
  // is no jump at release.
  anim_.SetProgress(InverseEaseOut8(std::clamp(visibleOpen, 0.0f, 1.0f)));

  if (shouldDismiss) {
    DismissBackgroundLayer();
  }
  else
    isOpening = true;

  // Hand off control to anim_ immediately; don't let SwipeInteraction spring.
  ResetSwipe();
}
