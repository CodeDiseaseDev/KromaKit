//
// Created by code on 4/27/26.
//
#pragma once

#include <kromakit/graphics/Graphics.h>

namespace DUIDefaultStyles {
  struct Colors {
    // // -------------------------
    // // Backgrounds
    // // -------------------------
    // static inline const Color ControlBackground   = ColorFromRGB(10, 11, 16);
    // static inline const Color BackgroundPrimary   = ColorFromRGB(12, 13, 16);
    // static inline const Color BackgroundSecondary = ColorFromRGB(20, 22, 26);
    // static inline const Color BackgroundTertiary  = ColorFromRGB(26, 28, 34);
    //
    //
    // // -------------------------
    // // Foreground (text)
    // // -------------------------
    // static inline const Color ForegroundPrimary   = ColorFromRGB(235, 235, 245);
    // static inline const Color ForegroundSecondary = ColorFromRGB(180, 185, 200);
    // static inline const Color ForegroundMuted     = ColorFromRGB(120, 125, 140);
    //
    // // -------------------------
    // // Accent
    // // -------------------------
    // static inline const Color AccentPrimary = ColorFromRGB(120, 140, 255);
    // static inline const Color AccentHover   = ColorFromRGB(140, 160, 255);
    // static inline const Color AccentActive  = ColorFromRGB(90, 110, 230);
    //
    // // -------------------------
    // // Borders
    // // -------------------------
    // static inline const Color BorderSubtle = ColorFromRGB(255, 255, 255, 20);
    // static inline const Color BorderStrong = ColorFromRGB(255, 255, 255, 60);
    //
    // // -------------------------
    // // Interaction
    // // -------------------------
    // static inline const Color HoverBackground   = ColorFromRGB(255, 255, 255, 12);
    // static inline const Color PressedBackground = ColorFromRGB(255, 255, 255, 20);
    //
    // // -------------------------
    // // Semantic
    // // -------------------------
    // static inline const Color Success = ColorFromRGB(80, 200, 120);
    // static inline const Color Warning = ColorFromRGB(255, 180, 80);
    // static inline const Color Error   = ColorFromRGB(255, 90, 90);
    //
    // // -------------------------
    // // Overlay / effects
    // // -------------------------
    // static inline const Color OverlayBackground = ColorFromRGB(0, 0, 0, 120);
    // static inline const Color Shadow            = ColorFromRGB(0, 0, 0, 180);







    static inline const Color ForegroundPrimary   = ColorFromRGB(235, 235, 245);
    static inline const Color ForegroundSecondary = ColorFromRGB(170, 175, 190);
    static inline const Color ForegroundMuted     = ColorFromRGB(120, 125, 140);
    static inline const Color ForegroundOnWhite   = ColorFromRGB(0, 0, 0);


    // -------------------------
    // Primary (Accent scale)
    // -------------------------
    static inline const Color PrimaryA0  = ColorFromRGB(255, 0, 0);
    static inline const Color PrimaryA10 = ColorFromRGB(255, 64, 49);
    static inline const Color PrimaryA20 = ColorFromRGB(255, 94, 77);
    static inline const Color PrimaryA30 = ColorFromRGB(255, 119, 101);
    static inline const Color PrimaryA40 = ColorFromRGB(255, 141, 123);

    static inline const Color PrimaryAccent = PrimaryA20;

    // -------------------------
    // Surface (Background scale)
    // -------------------------
    static inline const Color SurfaceA0  = ColorFromRGB(22, 24, 28);
    static inline const Color SurfaceA10 = ColorFromRGB(36, 38, 42);
    static inline const Color SurfaceA20 = ColorFromRGB(56, 58, 62);
    static inline const Color SurfaceA30 = ColorFromRGB(78, 80, 83);
    static inline const Color SurfaceA40 = ColorFromRGB(102, 104, 106);
    static inline const Color SurfaceA50 = ColorFromRGB(128, 129, 131);
    static inline const Color SurfaceA60 = ColorFromRGB(154, 154, 156);
    static inline const Color SurfaceA70 = ColorFromRGB(180, 180, 181);
    static inline const Color SurfaceA100 = ColorFromRGB(218, 218, 214);
    static inline const Color SurfaceA130 = ColorFromRGB(255, 255, 255);

    // -------------------------
    // Tonal surfaces (slight red tint)
    // -------------------------
    static inline const Color SurfaceTonalA0  = ColorFromRGB(33, 19, 20);
    static inline const Color SurfaceTonalA10 = ColorFromRGB(52, 38, 38);
    static inline const Color SurfaceTonalA20 = ColorFromRGB(72, 59, 59);
    static inline const Color SurfaceTonalA30 = ColorFromRGB(93, 80, 80);
    static inline const Color SurfaceTonalA40 = ColorFromRGB(114, 103, 103);
    static inline const Color SurfaceTonalA50 = ColorFromRGB(136, 126, 126);
    static inline const Color SurfaceTonalA60 = ColorFromRGB(159, 150, 151);
    static inline const Color SurfaceTonalA70 = ColorFromRGB(182, 176, 176);

    // -------------------------
    // Success
    // -------------------------
    static inline const Color SuccessA0  = ColorFromRGB(34, 148, 110);
    static inline const Color SuccessA10 = ColorFromRGB(91, 169, 137);
    static inline const Color SuccessA20 = ColorFromRGB(134, 191, 166);

    // -------------------------
    // Warning
    // -------------------------
    static inline const Color WarningA0  = ColorFromRGB(168, 122, 42);
    static inline const Color WarningA10 = ColorFromRGB(186, 148, 90);
    static inline const Color WarningA20 = ColorFromRGB(203, 174, 132);

    // -------------------------
    // Danger
    // -------------------------
    static inline const Color DangerA0  = ColorFromRGB(156, 33, 33);
    static inline const Color DangerA10 = ColorFromRGB(180, 84, 76);
    static inline const Color DangerA20 = ColorFromRGB(202, 127, 119);

    // -------------------------
    // Info
    // -------------------------
    static inline const Color InfoA0  = ColorFromRGB(33, 73, 138);
    static inline const Color InfoA10 = ColorFromRGB(75, 108, 162);
    static inline const Color InfoA20 = ColorFromRGB(117, 144, 186);



    // -------------------------
    // Borders
    // -------------------------
    static inline const Color BorderSubtle = ColorFromRGB(255, 255, 255, 12);
    static inline const Color BorderDefault = ColorFromRGB(255, 255, 255, 20);
    static inline const Color BorderStrong = ColorFromRGB(255, 255, 255, 60);

    static inline const Color BorderSubtleDark = ColorFromRGB(0, 0, 0, 20);
    static inline const Color BorderStrongDark = ColorFromRGB(0, 0, 0, 60);


    static inline const Color AppleTextActionAccentBlue = ColorFromRGB(0, 100, 255);

    static inline const Color TranslucentA0  = Alpha(SurfaceA0, 120);
    static inline const Color TranslucentA10  = Alpha(SurfaceA10, 120);
    static inline const Color TranslucentA20  = Alpha(SurfaceA20, 120);
  };
}