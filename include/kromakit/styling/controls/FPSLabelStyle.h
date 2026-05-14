#pragma once

#include <kromakit/debug/FPSLabel.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>

namespace DUIDefaultStyles::FPSLabelStyles {
inline const DUIStyleName Default = "debug.fps.label";

inline void Register() {
  DUIStyleRegistry::Register<FPSLabel>(Default, [](FPSLabel& l) {
    l.size = {60, 15};
    l.UseStyle(ForegroundBaseStyles::Secondary);
    l.SetFont({ "default", 11, FontWeight::Regular });
  });
}
}
