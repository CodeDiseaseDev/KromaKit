//
// Created by code on 5/8/26.
//

#ifndef TEST_IOS_APP_PLATFORMRESOURCELOADER_H
#define TEST_IOS_APP_PLATFORMRESOURCELOADER_H

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class PlatformResourceLoader {
public:
  static std::string ResolveResourcePath(std::string_view relativePath);
  static std::optional<std::string> ReadTextFile(std::string_view relativePath);
  static std::optional<std::vector<uint8_t>> ReadBinaryFile(std::string_view relativePath);
};

#endif //TEST_IOS_APP_PLATFORMRESOURCELOADER_H
