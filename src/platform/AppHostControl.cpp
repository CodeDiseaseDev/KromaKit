//
// Created by code on 5/18/26.
//

#include <kromakit/platform/AppHostControl.h>
#include <kromakit/developer_tools/MobileViewport.h>


void AppHostControl::SetMode(AppHostMode nextMode) {
  if (mode == nextMode)
    return;

  mode = nextMode;
  RebuildModeTree();
  MarkLayoutDirty();
  MarkVisualDirty();
}

void AppHostControl::RebuildModeTree() {
  // ClearChildrenButDoNotDeleteAppRootSomehow();

  if (mode == AppHostMode::FullScreen) {
    AddBorrowedControl(appRoot);
  }

  if (mode == AppHostMode::DevMobileViewport) {
    AddBorrowedControl(mobileViewport);
    mobileViewport->UseBorrowedControl(appRoot);
  }
}

void AppHostControl::DoLayout(Graphics* g) {
  const DUISize hostSize = size;

  if (mode == AppHostMode::FullScreen) {
    appRoot->SetLayoutFrame({0, 0}, hostSize);

    platform.formFactor = Platform::FormFactor::Desktop;
    platform.hasMouse = true;
    platform.hasKeyboard = true;
    platform.hasTouch = false;
    // platform.scaleFactor = 1.0f; // or actual window scale
  }

  if (mode == AppHostMode::DevMobileViewport) {
    mobileViewport->SetLayoutFrame({0, 0}, hostSize);

    platform.formFactor = Platform::FormFactor::Mobile;
    platform.hasMouse = false; // maybe true if desktop-simulated pointer still exists
    platform.hasTouch = true;
    platform.hasKeyboard = true; // unless simulating keyboard
    // platform.scaleFactor = 2.0f; // iPhone 7-ish
  }
}