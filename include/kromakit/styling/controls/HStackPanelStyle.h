#pragma once

#include <kromakit/stack_panels/HStackPanel.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::HStackPanelStyles {
inline const DUIStyleName Default = "hstack.default";

inline void Register() {
  DUIStyleRegistry::Register<HStackPanel>(Default, [](HStackPanel& s) {
    // s.spacing = 25;
    s.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Panel);
  });
}
}
