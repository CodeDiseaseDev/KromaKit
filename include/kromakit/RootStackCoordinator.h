//
// Created by code on 5/3/26.
//

#ifndef TEST_IOS_APP_ROOTSTACKCOORDINATOR_H
#define TEST_IOS_APP_ROOTSTACKCOORDINATOR_H
#include <kromakit/animation/FloatAnimation.h>
#include <kromakit/animation/FloatEaseAnimation.h>
#include <kromakit/overlay_background_layer/ContentStack.h>
#include <kromakit/overlay_background_layer/OverlayStack.h>


class RootStackCoordinator {
public:
  RootStackCoordinator();
  ~RootStackCoordinator() = default;

  void Attach(
    ContentStack* contentStack,
    OverlayStack* overlayStack);

  RootStackCoordinator(
    ContentStack* contentStack,
    OverlayStack* overlayStack) {

    Attach(contentStack, overlayStack);
  }

  void Update(float deltaTime);

private:
  ContentStack* contentStack = nullptr;
  OverlayStack* overlayStack = nullptr;

  FloatAnimation overlayAnimation;
  bool lastHasVisibleOverlay = false;

};



#endif //TEST_IOS_APP_ROOTSTACKCOORDINATOR_H
