//
// Created by code on 5/18/26.
//

#include <kromakit/developer_tools/MobileViewport.h>

#include "kromakit/styling/DUIDefaultStyles.h"

// #include "theme/controls/MobileViewportStyle.h"

void MobileViewport::UseBorrowedControl(Control *control) {
  borrowedControl = control;

  if (control == nullptr) {
    Logging::Log("control == nullptr\n");
    return;
  }

  if (control->parent != nullptr) {
    std::string ctrlName = control->GetControlName().value_or("<no name>");
    std::string parentName = control->parent->GetControlName().value_or("<no name>");

    Logging::Log("MobileViewport initialised with control '%s', which is already attached to %s\n",
      ctrlName.c_str(), parentName.c_str());

    return;
  }

  AddBorrowedControl(control);
}

MobileViewport::MobileViewport() {
  InitPanels();
}

void MobileViewport::InitPanels() {
  leftLayer   = CreateControl<Panel>();
  topLayer    = CreateControl<Panel>();
  rightLayer  = CreateControl<Panel>();
  bottomLayer = CreateControl<Panel>();

  leftLayer  ->UseStyle(DUIDefaultStyles::MobileViewportStyle::LayerPanelLeft());
  topLayer   ->UseStyle(DUIDefaultStyles::MobileViewportStyle::LayerPanelTop());
  rightLayer ->UseStyle(DUIDefaultStyles::MobileViewportStyle::LayerPanelRight());
  bottomLayer->UseStyle(DUIDefaultStyles::MobileViewportStyle::LayerPanelBottom());
}

void MobileViewport::OnRender(Graphics *rendTarget) {
  (void)rendTarget;
}

void MobileViewport::RepositionViewportControl() {
  if (borrowedControl == nullptr)
    return;

  const DUIRect rect = GetViewportRect();
  // do NOT use this rectangles X/Y (we are overriding it)

  const DUISize thisSize = size;
  const DUISize vpSize = { rect.width, rect.height };

  const float halfWidth = thisSize.width / 2;
  const float halfHeight = thisSize.height / 2;

  {
    const DUIPoint nextLocation = {
      halfWidth - vpSize.width / 2,
      halfHeight - vpSize.height / 2
    };

    borrowedControl->SetLayoutFrame(
      nextLocation, vpSize);
  }
}

void MobileViewport::RepositionViewportSidePanels() {
  const DUISize thisSize = size;
  const DUIRect vpRect = GetViewportRect();

  const DUIRect leftRect = {
    0.0f, 0.0f,
    vpRect.x, thisSize.height };

  const DUIRect topRect = {
    vpRect.x, 0.0f,
    vpRect.width, vpRect.y };

  const DUIRect rightRect = {
    vpRect.x + vpRect.width, 0.0f,
    thisSize.width - (vpRect.x + vpRect.width), thisSize.height };

  const DUIRect bottomRect = {
    vpRect.x, vpRect.y + vpRect.height,
    vpRect.width, thisSize.height - (vpRect.y + vpRect.height) };

  leftLayer->SetLayoutFrame({ leftRect.x, leftRect.y },
    { leftRect.width, leftRect.height });

  topLayer->SetLayoutFrame({ topRect.x, topRect.y },
    { topRect.width, topRect.height });

  rightLayer->SetLayoutFrame({ rightRect.x, rightRect.y },
    { rightRect.width, rightRect.height });

  bottomLayer->SetLayoutFrame({ bottomRect.x, bottomRect.y },
    { bottomRect.width, bottomRect.height });
}

DUIRect MobileViewport::GetViewportRect() const {
  if (borrowedControl != nullptr) {
    return {
      borrowedControl->location,
      borrowedControl->size
    };
  }

  return DUIRect{};
}

void MobileViewport::DoLayout(Graphics *rendTarget) {
  (void)rendTarget;

  RepositionViewportControl();
  RepositionViewportSidePanels();
}
