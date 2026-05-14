#pragma once

#include <kromakit/TextBlock.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::TextBlockStyles {
inline const DUIStyleName Default = "textblock.default";
inline const DUIStyleName BodyDefault = "textblock.body.default";

inline void Register() {
  DUIStyleRegistry::Register<TextBlock>(Default, [](TextBlock& t) {
    t.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Clear);
    t.UseStyle(ForegroundBaseStyles::Primary);
  });

  DUIStyleRegistry::Register<TextBlock>(BodyDefault, [](TextBlock& t) {
    t.UseStyle(Default);

    t.SetFont({ "default", 16, FontWeight::Regular });
  });
}
}
