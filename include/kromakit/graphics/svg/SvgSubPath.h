//
// Created by code on 5/8/26.
//

#ifndef TEST_IOS_APP_SVGPATH_H
#define TEST_IOS_APP_SVGPATH_H

#include <vector>

struct SvgSubPath {
  std::vector<float> points;
  bool closed = false;
  bool isHole = false;
};



#endif //TEST_IOS_APP_SVGPATH_H
