//
// Created by code on 5/7/26.
//

#ifndef TEST_IOS_APP_FLOATINGWINDOWCONTROLSTYLE_H
#define TEST_IOS_APP_FLOATINGWINDOWCONTROLSTYLE_H

#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIStyleName.h>
#include <kromakit/graphics/Graphics.h>

#include <kromakit/presentation/FloatingWindowControl.h>

namespace DUIDefaultStyles::FloatingWindowControlStyles {
  inline const DUIStyleName Default = "floatingwindowcontrol.default";

  inline void Register() {
    DUIStyleRegistry::Register<FloatingWindowControl>(Default,
      [=](FloatingWindowControl& window) {
        window.UseStyle(SurfaceBaseStyles::NeutralSurfaces::AppBackground);
        window.background = Alpha(window.background, 200);
        window.titlePanel->cornerRadius = window.cornerRadius =
          DUIInsets(0, 0, 0, 0);
        window.titlePanel->UseStyle(SurfaceBaseStyles::NeutralSurfaces::SurfaceWhiteSmoke);
        window.windowLabel->UseStyle(ForegroundBaseStyles::OnWhite);
        window.closeButton->SetTint(Colors::ForegroundOnWhite);

        window.outlineThickness = DUIInsets(1,0.01,1,1);
        window.outline = Colors::SurfaceA10;
      });
  }
}

#endif //TEST_IOS_APP_FLOATINGWINDOWCONTROLSTYLE_H
