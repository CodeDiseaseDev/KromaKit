//
// Created by code on 5/18/26.
//

#pragma once

#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIDefaultStyles.h>

namespace DUIDefaultStyles::MobileViewportStyle {
  inline const DUIStyleName& LayerPanel() {
    static const DUIStyleName c("mobileviewport.layerpanel");
    return c;
  }

  inline const DUIStyleName& LayerPanelLeft() {
    static const DUIStyleName c("mobileviewport.layerpanel.left");
    return c;
  }

  inline const DUIStyleName& LayerPanelTop() {
    static const DUIStyleName c("mobileviewport.layerpanel.top");
    return c;
  }

  inline const DUIStyleName& LayerPanelBottom() {
    static const DUIStyleName c("mobileviewport.layerpanel.bottom");
    return c;
  }

  inline const DUIStyleName& LayerPanelRight() {
    static const DUIStyleName c("mobileviewport.layerpanel.right");
    return c;
  }

  inline void Register() {
    DUIStyleRegistry::Register<Panel>(LayerPanel(), [](Panel& p) {
      p.background = ColorFromRGB(0, 0, 0, 200);
      p.zIndex = 100; // top-most by default.
      p.outline = ColorFromRGB(200, 200, 200, 150);
    });

    DUIStyleRegistry::Register<Panel>(LayerPanelLeft(), [](Panel& p) {
      p.UseStyle(LayerPanel());
      p.outlineThickness = DUIInsets::RightInset(1);
    });

    DUIStyleRegistry::Register<Panel>(LayerPanelTop(), [](Panel& p) {
      p.UseStyle(LayerPanel());
      p.outlineThickness = DUIInsets::BottomInset(1);
    });

    DUIStyleRegistry::Register<Panel>(LayerPanelRight(), [](Panel& p) {
      p.UseStyle(LayerPanel());
      p.outlineThickness = DUIInsets::LeftInset(1);
    });

    DUIStyleRegistry::Register<Panel>(LayerPanelBottom(), [](Panel& p) {
      p.UseStyle(LayerPanel());
      p.outlineThickness = DUIInsets::TopInset(1);
    });
  }
}
