//
// Created by code on 5/3/26.
//

#include <kromakit/RootStackCoordinator.h>

RootStackCoordinator::RootStackCoordinator() {
  overlayAnimation.SetCurve(AnimationCurve::EaseOut);
  overlayAnimation.SetDuration(0.2);
}

void RootStackCoordinator::Attach(
  ContentStack *cs,
  OverlayStack *os) {

  contentStack = cs;
  overlayStack = os;
}


void RootStackCoordinator::Update(
  float deltaTime) {

  if (contentStack == nullptr || overlayStack == nullptr)
    return;

  const bool hasOverlay = overlayStack->HasActiveOverlay();

  if (hasOverlay != lastHasVisibleOverlay) {
    lastHasVisibleOverlay = hasOverlay;

    overlayAnimation.SetRange(
      overlayAnimation.GetValue(),
      hasOverlay ? 1.0f : 0.0f
    );

    overlayAnimation.Reset();
    overlayAnimation.Start();
  }

  overlayAnimation.Update(deltaTime);

  contentStack->SetOverlayDepthEffect(
    overlayAnimation.GetValue());
}
