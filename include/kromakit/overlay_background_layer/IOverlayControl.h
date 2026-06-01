//
// Created by code on 4/28/26.
//

#ifndef TEST_IOS_APP_IOVERLAYCONTROL_H
#define TEST_IOS_APP_IOVERLAYCONTROL_H
#include <kromakit/Control.h>

class OverlayBackgroundLayer;

class IOverlayControl :
  public Control {

public:
  ~IOverlayControl() override;

  virtual bool ShouldBeCentered() { return false; }

  virtual void OnOverlayShown(OverlayBackgroundLayer* layer);
  virtual void OnOverlayDismissed();
  virtual bool ShouldDismissOnBackgroundTap() const;

  OverlayBackgroundLayer* background_layer = nullptr;

protected:
  void DismissBackgroundLayer();
};

#endif //TEST_IOS_APP_IOVERLAYCONTROL_H
