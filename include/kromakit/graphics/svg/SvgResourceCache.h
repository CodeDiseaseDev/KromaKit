//
// Created by code on 5/8/26.
//

#ifndef TEST_IOS_APP_SVGRESOURCECACHE_H
#define TEST_IOS_APP_SVGRESOURCECACHE_H
#include <string>
#include <memory>
#include <unordered_map>

#include <kromakit/graphics/svg/SvgDocument.h>
#include <kromakit/graphics/svg/SvgLoader.h>

class SvgResourceCache {
public:
  static std::shared_ptr<SvgDocument> Load(
    const std::string& resourcePath);

private:
  static std::unordered_map<
    std::string,
    std::weak_ptr<SvgDocument>> cache;
};

#endif //TEST_IOS_APP_SVGRESOURCECACHE_H
