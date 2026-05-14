//
// Created by code on 5/8/26.
//

#ifndef TEST_IOS_APP_SVGDOCUMENT_H
#define TEST_IOS_APP_SVGDOCUMENT_H

#include <vector>

#include <kromakit/graphics/svg/SvgShape.h>

class SvgDocument {
public:
  float width = 0.0f;
  float height = 0.0f;

  std::vector<SvgShape> shapes;
};

#endif //TEST_IOS_APP_SVGDOCUMENT_H
