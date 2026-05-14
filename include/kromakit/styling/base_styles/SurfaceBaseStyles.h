//
// Created by code on 4/27/26.
//

#pragma once
#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/Control.h>

namespace DUIDefaultStyles::SurfaceBaseStyles {

  namespace NeutralSurfaces {
    inline const DUIStyleName AppBackground = "synthem.surface.app";
    inline const DUIStyleName Panel         = "synthem.surface.panel";
    inline const DUIStyleName RaisedPanel   = "synthem.surface.raised";
    inline const DUIStyleName Modal         = "synthem.surface.modal";
    inline const DUIStyleName Overlay         = "synthem.surface.overlay";

    inline const DUIStyleName Clear         = "synthem.surface.clear";

    inline const DUIStyleName PanelTranslucent = "synthem.surface.panel.translucent";
    inline const DUIStyleName RaisedPanelTranslucent = "synthem.surface.panel.raised-translucent";

    inline const DUIStyleName SurfaceWhiteSmoke = "synthem.surface.solid-white";
  }

  namespace SemanticSurfaces {
    inline const DUIStyleName Success       = "synthem.surface.success";
    inline const DUIStyleName SuccessSoft   = "synthem.surface.success.soft";
    inline const DUIStyleName SuccessMuted  = "synthem.surface.success.muted";

    inline const DUIStyleName Warning       = "synthem.surface.warning";
    inline const DUIStyleName WarningSoft   = "synthem.surface.warning.soft";
    inline const DUIStyleName WarningMuted  = "synthem.surface.warning.muted";

    inline const DUIStyleName Danger        = "synthem.surface.danger";
    inline const DUIStyleName DangerSoft    = "synthem.surface.danger.soft";
    inline const DUIStyleName DangerMuted   = "synthem.surface.danger.muted";

    inline const DUIStyleName Info          = "synthem.surface.info";
    inline const DUIStyleName InfoSoft      = "synthem.surface.info.soft";
    inline const DUIStyleName InfoMuted     = "synthem.surface.info.muted";

    inline const DUIStyleName Tonal         = "synthem.surface.tonal";
    inline const DUIStyleName TonalSoft     = "synthem.surface.tonal.soft";
    inline const DUIStyleName TonalMuted    = "synthem.surface.tonal.muted";
  }

  inline void Register() {
    const auto AppBackgroundColor = Colors::SurfaceA0;
    const auto PanelColor         = Colors::SurfaceA10;
    const auto RaisedColor        = Colors::SurfaceA20;
    const auto ModalColor         = Colors::SurfaceA30;
    const auto OverlayColor       = Colors::SurfaceA40;
    const auto WhiteSurfaceColor    = Colors::SurfaceA100;

    const auto PanelTranslucentColor   = Colors::TranslucentA10;
    const auto RaisedPanelTranslucentColor   = Colors::TranslucentA10;

    /* neutral colours */
    DUIStyleRegistry::Register<Control>(NeutralSurfaces::AppBackground,
      [=](Control& c) { c.background = AppBackgroundColor; });

    DUIStyleRegistry::Register<Control>(NeutralSurfaces::Panel,
      [=](Control& c) { c.background = PanelColor; });

    DUIStyleRegistry::Register<Control>(NeutralSurfaces::RaisedPanel,
      [=](Control& c) { c.background = RaisedColor; });

    DUIStyleRegistry::Register<Control>(NeutralSurfaces::Modal,
      [=](Control& c) { c.background = ModalColor; });

    DUIStyleRegistry::Register<Control>(NeutralSurfaces::Clear,
      [=](Control& c) { c.background = {0,0,0,0}; });

    DUIStyleRegistry::Register<Control>(NeutralSurfaces::Overlay,
      [=](Control& c) { c.background = OverlayColor; });

    DUIStyleRegistry::Register<Control>(NeutralSurfaces::PanelTranslucent,
      [=](Control& c) { c.background = PanelTranslucentColor; });

    DUIStyleRegistry::Register<Control>(NeutralSurfaces::RaisedPanelTranslucent,
      [=](Control& c) { c.background = RaisedPanelTranslucentColor; });

    DUIStyleRegistry::Register<Control>(NeutralSurfaces::SurfaceWhiteSmoke,
      [=](Control& c) { c.background = WhiteSurfaceColor; });


    /* semantic colours */
    {
      DUIStyleRegistry::Register<Control>(SemanticSurfaces::Success,
        [=](Control& c) { c.background = Colors::SuccessA0; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::SuccessSoft,
        [=](Control& c) { c.background = Colors::SuccessA10; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::SuccessMuted,
        [=](Control& c) { c.background = Colors::SuccessA20; });


      DUIStyleRegistry::Register<Control>(SemanticSurfaces::Warning,
        [=](Control& c) { c.background = Colors::WarningA0; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::WarningSoft,
        [=](Control& c) { c.background = Colors::WarningA10; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::WarningMuted,
        [=](Control& c) { c.background = Colors::WarningA20; });


      DUIStyleRegistry::Register<Control>(SemanticSurfaces::Danger,
        [=](Control& c) { c.background = Colors::DangerA0; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::DangerSoft,
        [=](Control& c) { c.background = Colors::DangerA10; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::DangerMuted,
        [=](Control& c) { c.background = Colors::DangerA20; });


      DUIStyleRegistry::Register<Control>(SemanticSurfaces::Info,
        [=](Control& c) { c.background = Colors::InfoA0; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::InfoSoft,
        [=](Control& c) { c.background = Colors::InfoA10; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::InfoMuted,
        [=](Control& c) { c.background = Colors::InfoA20; });


      DUIStyleRegistry::Register<Control>(SemanticSurfaces::Tonal,
        [=](Control& c) { c.background = Colors::SurfaceTonalA0; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::TonalSoft,
        [=](Control& c) { c.background = Colors::SurfaceTonalA10; });

      DUIStyleRegistry::Register<Control>(SemanticSurfaces::TonalMuted,
        [=](Control& c) { c.background = Colors::SurfaceTonalA20; });
    }
  }
}
