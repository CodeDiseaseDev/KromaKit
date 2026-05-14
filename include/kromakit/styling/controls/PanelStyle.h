#pragma once

#include <kromakit/Panel.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::PanelStyles {
inline const DUIStyleName Default = "panel.default";
inline const DUIStyleName Clear = "panel.clear";

inline void Register() {
  DUIStyleRegistry::Register<Panel>(Default, [](Panel& p) {
    p.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Panel);
  });

  DUIStyleRegistry::Register<Panel>(Clear, [](Panel& p) {
    p.UseStyle(Default);
    p.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Clear);
  });
}
}
