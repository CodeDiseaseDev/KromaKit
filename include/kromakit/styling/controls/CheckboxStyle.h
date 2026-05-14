#pragma once

#include <kromakit/Checkbox.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::CheckboxStyles {
inline const DUIStyleName Default = "checkbox.default";

inline void Register() {
  DUIStyleRegistry::Register<Checkbox>(Default, [](Checkbox& c) {
    c.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Clear);
    c.UseStyle(ForegroundBaseStyles::Primary);
  });
}
}
