#pragma once

#include <kromakit/styling/controls/ButtonStyle.h>
#include <kromakit/Button.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/IconButton.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::IconButtonStyles {
inline const DUIStyleName Default = "iconbutton.default";

inline void Register() {
  DUIStyleRegistry::Register<IconButton>(Default, [](IconButton& b) {
    b.UseStyle(ButtonStyles::Default);

    b.background = {0,0,0,0};
    b.hoverBackground = ColorFromRGB(232, 17, 35, 120);
    b.cornerRadius = DUIInsets(0);
  });
}
}
