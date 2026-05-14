#pragma once

#include <kromakit/ScrollBar.h>
#include <kromakit/styling/DUIStyleRegistry.h>

namespace DUIDefaultStyles::ScrollBarStyles {
inline const DUIStyleName Default = "scrollbar.vertical.default";

inline void Register() {
  DUIStyleRegistry::Register<ScrollBar>(Default, [](ScrollBar& s) {
    // not implemented on iOS anyway lmao

  });
}
}
