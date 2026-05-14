//
// Created by code on 5/3/26.
//

#include <kromakit/overlay_background_layer/ContentStack.h>

#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace {
  constexpr float kContentStackEdgeOffsetPx = 0.0f;
  constexpr float kContentStackCornerRadiusPx = 6.0f;
  constexpr float kOverlayDepthZoomIntensity = 0.05f;
}

ContentStack::ContentStack() : Panel() {
  UseStyle(DUIDefaultStyles
    ::SurfaceBaseStyles
    ::NeutralSurfaces::Panel);
}

void ContentStack::SetZoomScale(float scale) {
  scale = std::clamp(scale, 0.1f, 4.0f);

  if (std::abs(zoomScale - scale) < 0.0001f)
    return;

  zoomScale = scale;
  SetCentredScale(zoomScale);
}

float ContentStack::GetZoomScale() const {
  return zoomScale;
}

void ContentStack::SetOverlayDepthEffect(float t) {
  t = std::clamp(t, 0.0f, 1.0f);

  if (std::abs(overlayDepthEffect - t) < 0.0001f)
    return;

  overlayDepthEffect = t;
  SetZoomScale(GetActualScale());
}

void ContentStack::OnRender(Graphics *rendTarget) {
  const float scale = GetActualScale();
  if (scale > 0.0f) {
    const float inverseScale = 1.0f / scale;

    const float extraWidth = size.width * (inverseScale - 1.0f);
    const float extraHeight = size.height * (inverseScale - 1.0f);

    const float x = -(extraWidth * 0.5f) - kContentStackEdgeOffsetPx;
    const float y = -(extraHeight * 0.5f) - kContentStackEdgeOffsetPx;
    const float w = size.width + extraWidth + kContentStackEdgeOffsetPx * 2.0f;
    const float h = size.height + extraHeight + kContentStackEdgeOffsetPx * 2.0f;

    rendTarget->FillRoundedRectangle(
      x, y,
      w, h,
      background,
      kContentStackCornerRadiusPx
    );
  }

  // Panel::OnRender(rendTarget);
}

bool ContentStack::ShouldClipToBounds() {
  return false;
}

std::optional<std::string> ContentStack::GetControlName() const {
  return "ContentStack";
}

float ContentStack::GetActualScale() const {
  return std::clamp(
    1.0f - overlayDepthEffect * kOverlayDepthZoomIntensity,
    0.1f,
    4.0f
  );
}
