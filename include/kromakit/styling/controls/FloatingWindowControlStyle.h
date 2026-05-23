//
// Created by code on 5/7/26.
//

#ifndef TEST_IOS_APP_FLOATINGWINDOWCONTROLSTYLE_H
#define TEST_IOS_APP_FLOATINGWINDOWCONTROLSTYLE_H

#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIStyleName.h>
#include <kromakit/graphics/Graphics.h>

#include <kromakit/presentation/FloatingWindowControl.h>

#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::FloatingWindowControlStyles {
  inline const DUIStyleName Default = "floatingwindowcontrol.default";

  inline void Register() {
    DUIStyleRegistry::Register<FloatingWindowControl>(Default,
      [=](FloatingWindowControl& window) {
        window.UseStyle(SurfaceBaseStyles::NeutralSurfaces::AppBackground);
        // window.background = Alpha(window.background, 200);
        window.titlePanel->cornerRadius = window.cornerRadius =
          DUIInsets(6, 6, 0, 0);
        window.titlePanel->UseStyle(SurfaceBaseStyles::NeutralSurfaces::RaisedPanel);
        window.windowLabel->UseStyle(ForegroundBaseStyles::Primary);
        window.closeButton->SetTint(Colors::PrimaryA20);

        // window.outlineThickness = DUIInsets(1,0.01,1,1);
        // window.outline = Colors::SurfaceA10;
      });
  }
}

#endif //TEST_IOS_APP_FLOATINGWINDOWCONTROLSTYLE_H
