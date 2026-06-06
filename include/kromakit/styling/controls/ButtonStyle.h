#pragma once

#include <kromakit/Button.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::ButtonStyles {
inline const DUIStyleName Default = "button.default";
inline const DUIStyleName Primary = "button.primary";
inline const DUIStyleName TextAction = "button.text.action";

inline void Register() {
  DUIStyleRegistry::Register<Button>(Default, [](Button& b) {
    b.size = {150, 30};
    b.cornerRadius = DUIInsets(4);
    b.useHoverBackground = true;


    b.UseStyle(SurfaceBaseStyles::NeutralSurfaces::RaisedPanel);
    b.UseStyle(ForegroundBaseStyles::Primary);

    b.hoverBackground = Lighten(b.background, -10);
  });

  DUIStyleRegistry::Register<Button>(Primary, [](Button& b) {
    b.UseStyle(Default);

    b.background = Colors::SuccessA0;
    b.hoverBackground = Lighten(Colors::SuccessA0, -30);

    // b.foreground
  });

  DUIStyleRegistry::Register<Button>(TextAction, [](Button& b) {
    b.UseStyle(Default);

    b.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Clear);
    b.SetFont({ "default", 15, FontWeight::Regular });

    b.foreground = Colors::AppleTextActionAccentBlue;

    b.cornerRadius = DUIInsets(4);
  });
}
}
