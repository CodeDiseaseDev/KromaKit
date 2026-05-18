//
// Created by code on 5/18/26.
//

#ifndef SYNTHEMKIT_MOBILEVIEWPORT_H
#define SYNTHEMKIT_MOBILEVIEWPORT_H

#include <kromakit/Control.h>
#include <kromakit/Panel.h>

class MobileViewport : public Control {

  Control* borrowedControl = nullptr;

public:
  void UseBorrowedControl(Control* control);
  explicit MobileViewport();

  void InitPanels();
  Panel *leftLayer = nullptr,  *topLayer = nullptr,
        *rightLayer = nullptr, *bottomLayer = nullptr;

  std::optional<std::string> GetControlName() const override {
    return "MobileViewport";
  }


  void RepositionViewportControl();
  void RepositionViewportSidePanels();
  DUIRect GetViewportRect() const;

  void DoLayout(Graphics* rendTarget) override;
  void OnRender(Graphics* rendTarget) override;
};


#endif //SYNTHEMKIT_MOBILEVIEWPORT_H
