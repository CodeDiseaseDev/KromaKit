#pragma once

#include <nanovg.h>
#include <kromakit/DUIFont.h>

#include <filesystem>
#include <string>

class FontLoaderLinux {
public:
  explicit FontLoaderLinux(NVGcontext* context);

  bool LoadNanoVGFontFace(
    const std::string& faceName,
    const std::string& directory,
    const std::string& filename);

  bool RegisterFontFamily(
    const std::string& family,
    const std::string& style,
    FontWeight weight,
    FontStyle fontStyle,
    const std::string& directory,
    const std::string& filename);

  void LoadBundledFonts();

private:
  std::string FontFaceNameForFamily(
    const std::string& family,
    const std::string& style) const;

  std::filesystem::path ResolveFontPath(
    const std::string& directory,
    const std::string& filename) const;

  void RegisterLegacyAliases() const;

  NVGcontext* context_ = nullptr;
};
