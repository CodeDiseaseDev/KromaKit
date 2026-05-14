//
// Created by code on 5/8/26.
//

#ifndef TEST_IOS_APP_SVGSHAPE_H
#define TEST_IOS_APP_SVGSHAPE_H
#include <kromakit/graphics/svg/SvgSubPath.h>
#include <nanovg.h>

enum class SvgPaintType : unsigned char {
  None = 0,
  Color = 1,
  Unsupported = 2
};

enum class SvgFillRule : unsigned char {
  NonZero = 0,
  EvenOdd = 1
};

enum class SvgLineCap : unsigned char {
  Butt = 0,
  Round = 1,
  Square = 2
};

enum class SvgLineJoin : unsigned char {
  Miter = 0,
  Round = 1,
  Bevel = 2
};

struct SvgShape {
  std::vector<SvgSubPath> paths;

  bool visible = true;
  float opacity = 1.0f;

  SvgPaintType fillType = SvgPaintType::None;
  SvgPaintType strokeType = SvgPaintType::None;
  bool hasFill = false;
  bool hasStroke = false;

  NVGcolor fillColor;
  NVGcolor strokeColor;
  float strokeWidth = 1.0f;
  SvgFillRule fillRule = SvgFillRule::NonZero;
  unsigned char paintOrder = 0;
  float miterLimit = 4.0f;
  SvgLineCap strokeLineCap = SvgLineCap::Butt;
  SvgLineJoin strokeLineJoin = SvgLineJoin::Miter;
};

#endif //TEST_IOS_APP_SVGSHAPE_H
