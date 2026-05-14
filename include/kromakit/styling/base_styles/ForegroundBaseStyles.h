//
// Created by code on 4/27/26.
//

#pragma once

#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/Control.h>

namespace DUIDefaultStyles::ForegroundBaseStyles {

  inline const DUIStyleName Primary      = "synthem.fg.primary";
  inline const DUIStyleName Secondary    = "synthem.fg.secondary";
  inline const DUIStyleName Muted        = "synthem.fg.muted";
  inline const DUIStyleName Accent       = "synthem.fg.accent";
  inline const DUIStyleName OnAccent     = "synthem.fg.onaccent"; // text on accent bg
  inline const DUIStyleName OnWhite     = "synthem.fg.onwhite";

  inline const DUIStyleName Success      = "synthem.fg.success";
  inline const DUIStyleName SuccessSoft  = "synthem.fg.success.soft";
  inline const DUIStyleName SuccessMuted = "synthem.fg.success.muted";

  inline const DUIStyleName Warning      = "synthem.fg.warning";
  inline const DUIStyleName WarningSoft  = "synthem.fg.warning.soft";
  inline const DUIStyleName WarningMuted = "synthem.fg.warning.muted";

  inline const DUIStyleName Danger       = "synthem.fg.danger";
  inline const DUIStyleName DangerSoft   = "synthem.fg.danger.soft";
  inline const DUIStyleName DangerMuted  = "synthem.fg.danger.muted";

  inline const DUIStyleName Info         = "synthem.fg.info";
  inline const DUIStyleName InfoSoft     = "synthem.fg.info.soft";
  inline const DUIStyleName InfoMuted    = "synthem.fg.info.muted";

  inline void Register() {

    DUIStyleRegistry::Register<Control>(Primary,
      [=](Control& c) {
        c.foreground = Colors::ForegroundPrimary;
      });

    DUIStyleRegistry::Register<Control>(Secondary,
      [=](Control& c) {
        c.foreground = Colors::ForegroundSecondary;
      });

    DUIStyleRegistry::Register<Control>(Muted,
      [=](Control& c) {
        c.foreground = Colors::ForegroundMuted;
      });

    DUIStyleRegistry::Register<Control>(Accent,
      [=](Control& c) {
        c.foreground = Colors::PrimaryAccent;
      });

    DUIStyleRegistry::Register<Control>(OnAccent,
      [=](Control& c) {
        c.foreground = ColorFromRGB(255,255,255);
      });

    DUIStyleRegistry::Register<Control>(OnWhite,
      [=](Control& c) { c.foreground = Colors::ForegroundOnWhite; });

    // -------------------------
    // Success
    // -------------------------
    DUIStyleRegistry::Register<Control>(Success,
      [=](Control& c) {
        c.foreground = Colors::SuccessA0;
      });

    DUIStyleRegistry::Register<Control>(SuccessSoft,
      [=](Control& c) {
        c.foreground = Colors::SuccessA10;
      });

    DUIStyleRegistry::Register<Control>(SuccessMuted,
      [=](Control& c) {
        c.foreground = Colors::SuccessA20;
      });


    // -------------------------
    // Warning
    // -------------------------
    DUIStyleRegistry::Register<Control>(Warning,
      [=](Control& c) {
        c.foreground = Colors::WarningA0;
      });

    DUIStyleRegistry::Register<Control>(WarningSoft,
      [=](Control& c) {
        c.foreground = Colors::WarningA10;
      });

    DUIStyleRegistry::Register<Control>(WarningMuted,
      [=](Control& c) {
        c.foreground = Colors::WarningA20;
      });


    // -------------------------
    // Danger
    // -------------------------
    DUIStyleRegistry::Register<Control>(Danger,
      [=](Control& c) {
        c.foreground = Colors::DangerA0;
      });

    DUIStyleRegistry::Register<Control>(DangerSoft,
      [=](Control& c) {
        c.foreground = Colors::DangerA10;
      });

    DUIStyleRegistry::Register<Control>(DangerMuted,
      [=](Control& c) {
        c.foreground = Colors::DangerA20;
      });


    // -------------------------
    // Info
    // -------------------------
    DUIStyleRegistry::Register<Control>(Info,
      [=](Control& c) {
        c.foreground = Colors::InfoA0;
      });

    DUIStyleRegistry::Register<Control>(InfoSoft,
      [=](Control& c) {
        c.foreground = Colors::InfoA10;
      });

    DUIStyleRegistry::Register<Control>(InfoMuted,
      [=](Control& c) {
        c.foreground = Colors::InfoA20;
      });
  }
}