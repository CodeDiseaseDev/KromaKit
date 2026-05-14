#pragma once

#include <kromakit/ModalPresentation.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/styling/base_styles/BorderBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

namespace DUIDefaultStyles::ModalPresentationStyles {
inline const DUIStyleName Default = "modal.presentation.default";

namespace TitleBar {
  inline const DUIStyleName Default = "modal.presentation.titlebar.default";
}


inline void Register() {
  DUIStyleRegistry::Register<ModalPresentation>(Default, [&](ModalPresentation& m) {
    m.SetRenderLayer(RenderLayer::Overlay);
    m.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Modal);
    m.HeaderPanel->UseStyle(SurfaceBaseStyles::NeutralSurfaces::Clear);

    m.TitleBarHeight = 35;

    m.CornerRadius = 10;
  });

  DUIStyleRegistry::Register<Panel>(TitleBar::Default, [&](Panel& p) {
    p.UseStyle(BorderBaseStyles::Strong);
    p.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Clear);

    p.outlineThickness = DUIInsets(0,2,0,0);
  });
}
}
