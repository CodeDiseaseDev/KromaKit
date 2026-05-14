//
// Created by code on 4/27/26.
//

#pragma once

#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/styling/DUIStyleName.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/Control.h>

namespace DUIDefaultStyles::BorderBaseStyles {

  inline const DUIStyleName Subtle = "synthem.border.subtle";
  inline const DUIStyleName Strong = "synthem.border.strong";
  inline const DUIStyleName Accent = "synthem.border.strong";

  inline void Register() {

    DUIStyleRegistry::Register<Control>(Subtle,
      [=](Control& c) {
        c.outline = Colors::BorderSubtle;
        c.outlineThickness = DUIInsets(1);
      });

    DUIStyleRegistry::Register<Control>(Strong,
      [=](Control& c) {
        c.outline = Colors::BorderStrong;
        c.outlineThickness = DUIInsets(1);
      });

    DUIStyleRegistry::Register<Control>(Accent,
      [=](Control& c) {
        c.outline = Colors::PrimaryAccent;
        c.outlineThickness = DUIInsets(1);
      });
  }
}