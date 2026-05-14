//
// Created by code on 4/28/26.
//

#ifndef TEST_IOS_APP_IOVERLAYCONTROL_H
#define TEST_IOS_APP_IOVERLAYCONTROL_H
#include <kromakit/Control.h>


// TODO: Long-press / force-touch context menu presentation
// - Add pressure to DirectUI pointer/touch event
// - iPhone 7: map UITouch.force / maximumPossibleForce to 0..1
// - fallback: long-press timer for devices without 3D Touch
// - ContextMenu open progress:
//     opacity 0 -> 1
//     scale 0.94 -> 1.0
//     y offset 6 -> 0
// - Haptic when threshold is reached
// - Prevent normal click firing after long-press trigger
// - OverlayBackgroundLayer animates blur/dim opacity



class OverlayBackgroundLayer;

class IOverlayControl :
  public Control {

public:
  virtual ~IOverlayControl();

  virtual void OnOverlayShown(OverlayBackgroundLayer* layer);

  virtual void OnOverlayDismissed();

  virtual bool ShouldDismissOnBackgroundTap() const;

  OverlayBackgroundLayer* background_layer = nullptr;

protected:
  void DismissBackgroundLayer();
};



#endif //TEST_IOS_APP_IOVERLAYCONTROL_H
