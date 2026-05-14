//
// Created by code on 5/8/26.
//
#include <kromakit/platform/PlatformResourceLoader.h>

#include <fstream>
#include <iterator>

std::string PlatformResourceLoader::ResolveResourcePath(
  std::string_view relativePath) {
  constexpr std::string_view prefix = "resources/";

  if (relativePath.rfind(prefix, 0) == 0) {
    return std::string(relativePath);
  }

  return std::string(prefix) + std::string(relativePath);
}

std::optional<std::string> PlatformResourceLoader::ReadTextFile(
  std::string_view relativePath) {
  const std::string path = ResolveResourcePath(relativePath);

  std::ifstream file(path);
  if (!file.is_open()) {
    return std::nullopt;
  }

  std::string text(
    (std::istreambuf_iterator<char>(file)),
    std::istreambuf_iterator<char>()
  );

  return text;
}

std::optional<std::vector<uint8_t>> PlatformResourceLoader::ReadBinaryFile(
  std::string_view relativePath) {
  const std::string path = ResolveResourcePath(relativePath);

  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    return std::nullopt;
  }

  const std::ifstream::pos_type endPos = file.tellg();
  if (endPos < 0) {
    return std::nullopt;
  }

  std::vector<uint8_t> bytes(static_cast<size_t>(endPos));

  file.seekg(0, std::ios::beg);

  if (!bytes.empty()) {
    file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    if (!file) {
      return std::nullopt;
    }
  }

  return bytes;
}
