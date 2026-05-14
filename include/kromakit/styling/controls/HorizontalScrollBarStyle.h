#pragma once

#include <kromakit/HorizontalScrollBar.h>
#include <kromakit/styling/DUIStyleRegistry.h>

namespace DUIDefaultStyles::HorizontalScrollBarStyles {
inline const DUIStyleName Default = "scrollbar.horizontal.default";

inline void Register() {
  DUIStyleRegistry::Register<HorizontalScrollBar>(Default, [](HorizontalScrollBar& s) {
    // dw about it (cause its not even on the ios version anyway (for now))
    // s.size = {300, 18};
    // s.background = ColorFromRGB(28, 30, 40, 255);
    // s.foreground = ColorFromRGB(40, 40, 40);
  });
}
}
