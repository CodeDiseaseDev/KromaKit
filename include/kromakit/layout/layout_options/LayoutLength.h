//
// Created by code on 4/29/26.
//

#ifndef TEST_IOS_APP_LAYOUTLENGTH_H
#define TEST_IOS_APP_LAYOUTLENGTH_H
#include <kromakit/layout/layout_options/LayoutUnit.h>

struct LayoutLength {
  LayoutUnit unit = LayoutUnit::Auto;
  float value = 0.0f;

  static LayoutLength Auto()    { return { LayoutUnit::Auto, 0.0f }; }
  static LayoutLength Fixed(float v) { return { LayoutUnit::Fixed, v }; }
  static LayoutLength Fill(float weight = 1.0f) { return { LayoutUnit::Fill, weight }; }
  static LayoutLength Percent(float v) { return { LayoutUnit::Percent, v }; }
};

#endif //TEST_IOS_APP_LAYOUTLENGTH_H
