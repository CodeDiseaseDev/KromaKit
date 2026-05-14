//
// Created by code on 5/8/26.
//

#ifndef TEST_IOS_APP_SVGLOADER_H
#define TEST_IOS_APP_SVGLOADER_H

#include <memory>

#include <kromakit/graphics/svg/SvgDocument.h>

class SvgLoader {
  // enum class SvgLineCap {
  //   Butt,
  //   Round,
  //   Square
  // };
  //
  // enum class SvgLineJoin {
  //   Miter,
  //   Round,
  //   Bevel
  // };

public:
  static std::shared_ptr<SvgDocument> LoadFromFile(const std::string& path);
  // static std::unique_ptr<SvgDocument> LoadFromString(const std::string& svgText);
};



#endif //TEST_IOS_APP_SVGLOADER_H
