#pragma once

#include <kromakit/Label.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::LabelStyles {
inline const DUIStyleName Default = "label.default";
inline const DUIStyleName Caption = "label.caption";
inline const DUIStyleName BadgeCode = "label.badge.code";

inline void Register() {
  DUIStyleRegistry::Register<Label>(Default, [](Label& l) {
    l.UseStyle(ForegroundBaseStyles::Primary);
    l.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Clear);
  });

  DUIStyleRegistry::Register<Label>(Caption, [](Label& l) {
    l.UseStyle(Default);
    l.SetFont({ "default", 14, FontWeight::Regular });
  });

  DUIStyleRegistry::Register<Label>(BadgeCode, [](Label& l) {
    l.UseStyle(Default);
    l.UseStyle(SurfaceBaseStyles::NeutralSurfaces::RaisedPanel);

    l.backgroundPadding = DUIInsets(7, 4);
    l.cornerRadius = DUIInsets(4);
    l.SetFont({ "JetBrainsMono", 10, FontWeight::Regular });
  });

}
}
