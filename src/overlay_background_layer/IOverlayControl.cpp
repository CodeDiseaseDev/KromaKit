//
// Created by code on 4/28/26.
//

#include <kromakit/overlay_background_layer/IOverlayControl.h>
#include <kromakit/overlay_background_layer/OverlayBackgroundLayer.h>

IOverlayControl::~IOverlayControl() = default;

void IOverlayControl::OnOverlayShown(OverlayBackgroundLayer *layer) {}

void IOverlayControl::OnOverlayDismissed() {}

bool IOverlayControl::ShouldDismissOnBackgroundTap() const {
  return true;
}

void IOverlayControl::DismissBackgroundLayer() {
  if (background_layer != nullptr)
    background_layer->Dismiss();
}
