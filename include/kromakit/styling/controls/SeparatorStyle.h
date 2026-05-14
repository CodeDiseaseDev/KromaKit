#pragma once

#include <kromakit/Separator.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/base_styles/BorderBaseStyles.h>

namespace DUIDefaultStyles::SeparatorStyles {
  inline const DUIStyleName Default = "separator.default";

  inline const DUIStyleName AsHorizontal_ = "separator.default.as_horizontal";
  inline const DUIStyleName AsVertical_ = "separator.default.as_vertical";

inline void Register() {
  DUIStyleRegistry::Register<Separator>(AsHorizontal_,
    [](Separator& s) { s.size.height = 1; });

  DUIStyleRegistry::Register<Separator>(AsVertical_,
    [](Separator& s) { s.size.width = 1; });

  DUIStyleRegistry::Register<Separator>(Default, [](Separator& s) {
    s.UseStyle(AsHorizontal_);
    s.UseStyle(BorderBaseStyles::Subtle);

    s.cornerRadius = DUIInsets(0);
  });
}
}
