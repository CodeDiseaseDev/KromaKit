#pragma once

#include <kromakit/stack_panels/VStackPanel.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::VStackPanelStyles {
inline const DUIStyleName Default = "vstack.default";

inline void Register() {
  DUIStyleRegistry::Register<VStackPanel>(Default, [](VStackPanel& s) {
    s.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Clear);
    // s.spacing = 25;
  });
}
}
