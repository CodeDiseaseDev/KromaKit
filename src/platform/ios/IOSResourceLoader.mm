//
// Created by code on 5/8/26.
//

#import <Foundation/Foundation.h>

#include <kromakit/platform/PlatformResourceLoader.h>

#include <fstream>
#include <string>

namespace {

std::string NSStringToStdString(NSString* value) {
  if (value == nil) {
    return {};
  }

  return std::string(value.UTF8String);
}

bool StartsWithResourcesPrefix(std::string_view path) {
  constexpr std::string_view kPrefix = "resources/";
  return path.rfind(kPrefix, 0) == 0;
}

std::string NormalizeResourceRelativePath(std::string_view path) {
  constexpr std::string_view kPrefix = "resources/";

  if (StartsWithResourcesPrefix(path)) {
    return std::string(path.substr(kPrefix.size()));
  }

  return std::string(path);
}

std::optional<std::string> ResolveFromBundle(std::string_view normalizedPath) {
  const std::string normalized(normalizedPath);
  NSString* relativePath = [NSString stringWithUTF8String:normalized.c_str()];
  if (relativePath == nil) {
    return std::nullopt;
  }

  NSString* bundledPath = [[NSBundle mainBundle] pathForResource:relativePath
                                                           ofType:nil];
  if (bundledPath != nil) {
    return NSStringToStdString(bundledPath);
  }

  NSString* resourceRoot = [[NSBundle mainBundle] resourcePath];
  if (resourceRoot != nil) {
    NSString* candidate = [resourceRoot stringByAppendingPathComponent:relativePath];
    if ([[NSFileManager defaultManager] fileExistsAtPath:candidate]) {
      return NSStringToStdString(candidate);
    }
  }

  return std::nullopt;
}

} // namespace

std::string PlatformResourceLoader::ResolveResourcePath(
  std::string_view relativePath) {
  const std::string normalized = NormalizeResourceRelativePath(relativePath);

  if (auto bundlePath = ResolveFromBundle(normalized)) {
    return *bundlePath;
  }

  // Fallback for non-bundled runs (for example launching from a repo checkout).
  return "resources/" + normalized;
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
