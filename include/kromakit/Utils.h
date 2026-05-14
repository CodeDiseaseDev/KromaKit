#pragma once

#define ___VAR_NAME(var) (#var)
#define ___VAR_NAME_W(var) (#var)
#include <cstdio>
#include <cstdarg>

#include <kromakit/Logging.h>
#include <kromakit/graphics/Graphics.h>

// #include <source_location>
// #include <print>

namespace DirectUIUtils
{




  template <class T>
  inline T clamp(T val, T min, T max)
  {
    if (val >= max) return max;
    if (val <= min) return min;
    return val;
  }

  inline void PrintColorRGB(Color color)
  {
    float fr = color.r * 255.0f;
    float fg = color.g * 255.0f;
    float fb = color.b * 255.0f;
    float fa = color.a * 255.0f;

    int r = (int)fr;
    int g = (int)fg;
    int b = (int)fb;
    int a = (int)fa;

    Logging::Log(
      "rgba(r:%i, g:%i, b:%i, a:%i)\n",
      r, g, b, a
    );
  }

  inline void PrintColorHEX(Color color)
  {
    float fr = color.r * 255.0f;
    float fg = color.g * 255.0f;
    float fb = color.b * 255.0f;
    float fa = color.a * 255.0f;

    int r = (int)fr;
    int g = (int)fg;
    int b = (int)fb;
    int a = (int)fa;

    printf(
      "0x%X%X%X(%X)\n",
      r, g, b, a
    );
  }

  inline void PrintColorNormalised(Color color)
  {
    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;
    float a = color.a / 255.0f;

    printf(
      "rgba{r:%.2f, g:%.2f, b:%.2f, a:%.2f}\n",
      r, g, b, a
    );
  }



}
