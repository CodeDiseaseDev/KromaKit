#pragma once

#include <kromakit/gesture/SwipeActionButton.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::SwipeActionButtonStyles {
inline const DUIStyleName Default = "button.swipe.action";

inline void Register() {
  constexpr float cornerRadius = 8;

  DUIStyleRegistry::Register<SwipeActionButton>(Default, [](SwipeActionButton& b) {
    b.UseStyle(SurfaceBaseStyles::SemanticSurfaces::DangerSoft);
    b.UseStyle(ForegroundBaseStyles::Primary);

    b.cornerRadius = DUIInsets(0, cornerRadius, 0, cornerRadius);
  });
}
}
