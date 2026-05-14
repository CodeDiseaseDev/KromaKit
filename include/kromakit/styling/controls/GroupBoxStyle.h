#pragma once

#include <kromakit/GroupBox.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::GroupBoxStyles {
inline const DUIStyleName Default = "groupbox.default";

inline void Register() {
  DUIStyleRegistry::Register<GroupBox>(Default, [](GroupBox& g) {
    g.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Panel);
    g.UseStyle(ForegroundBaseStyles::Secondary);
  });
}
}
