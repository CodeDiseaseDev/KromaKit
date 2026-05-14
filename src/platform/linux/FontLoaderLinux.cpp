#include <kromakit/platform/linux/FontLoaderLinux.h>

#include <nanovg.h>
#include <kromakit/platform/PlatformResourceLoader.h>

#include <array>
#include <iostream>

namespace {

struct FontRegistration {
  const char* style;
  FontWeight weight;
  FontStyle fontStyle;
  const char* filename;
};

} // namespace

FontLoaderLinux::FontLoaderLinux(NVGcontext* context)
  : context_(context) {}

std::string FontLoaderLinux::FontFaceNameForFamily(
  const std::string& family,
  const std::string& style) const
{
  if (style.empty()) {
    return family;
  }

  return family + "-" + style;
}

std::filesystem::path FontLoaderLinux::ResolveFontPath(
  const std::string& directory,
  const std::string& filename) const
{
  const std::string relativePath = directory + "/" + filename + ".ttf";
  const std::string resolvedPath =
    PlatformResourceLoader::ResolveResourcePath(relativePath);

  return std::filesystem::path(resolvedPath);
}

bool FontLoaderLinux::LoadNanoVGFontFace(
  const std::string& faceName,
  const std::string& directory,
  const std::string& filename)
{
  if (context_ == nullptr) {
    std::cerr << "Cannot load font '" << faceName
              << "': NanoVG context is null.\n";
    return false;
  }

  const std::filesystem::path fontPath = ResolveFontPath(directory, filename);
  if (fontPath.empty() || !std::filesystem::exists(fontPath)) {
    std::cerr << "Cannot load font '" << faceName
              << "': missing " << directory << "/" << filename << ".ttf\n";
    return false;
  }

  const int fontId = nvgCreateFont(
    context_,
    faceName.c_str(),
    fontPath.string().c_str());

  if (fontId == -1) {
    std::cerr << "Failed to register NanoVG font '" << faceName
              << "' from " << fontPath << "\n";
    return false;
  }

  return true;
}

bool FontLoaderLinux::RegisterFontFamily(
  const std::string& family,
  const std::string& style,
  FontWeight weight,
  FontStyle fontStyle,
  const std::string& directory,
  const std::string& filename)
{
  const std::string faceName = FontFaceNameForFamily(family, style);
  if (!LoadNanoVGFontFace(faceName, directory, filename)) {
    return false;
  }

  FontRegistry::Register(family, weight, fontStyle, faceName);
  return true;
}

void FontLoaderLinux::RegisterLegacyAliases() const
{
  FontRegistry::Register(
    "default",
    FontWeight::Regular,
    FontStyle::Normal,
    "Ubuntu-Regular");

  FontRegistry::Register(
    "Poppins",
    FontWeight::Regular,
    FontStyle::Normal,
    "Poppins-Regular");

  FontRegistry::Register(
    "JetBrainsMono",
    FontWeight::Regular,
    FontStyle::Normal,
    "JetBrainsMono-Regular");
}

void FontLoaderLinux::LoadBundledFonts()
{
  const std::array<FontRegistration, 18> poppins = {{
    {"Thin", FontWeight::Thin, FontStyle::Normal, "Poppins-Thin"},
    {"ThinItalic", FontWeight::Thin, FontStyle::Italic, "Poppins-ThinItalic"},
    {"ExtraLight", FontWeight::ExtraLight, FontStyle::Normal, "Poppins-ExtraLight"},
    {"ExtraLightItalic", FontWeight::ExtraLight, FontStyle::Italic, "Poppins-ExtraLightItalic"},
    {"Light", FontWeight::Light, FontStyle::Normal, "Poppins-Light"},
    {"LightItalic", FontWeight::Light, FontStyle::Italic, "Poppins-LightItalic"},
    {"Regular", FontWeight::Regular, FontStyle::Normal, "Poppins-Regular"},
    {"Italic", FontWeight::Regular, FontStyle::Italic, "Poppins-Italic"},
    {"Medium", FontWeight::Medium, FontStyle::Normal, "Poppins-Medium"},
    {"MediumItalic", FontWeight::Medium, FontStyle::Italic, "Poppins-MediumItalic"},
    {"SemiBold", FontWeight::SemiBold, FontStyle::Normal, "Poppins-SemiBold"},
    {"SemiBoldItalic", FontWeight::SemiBold, FontStyle::Italic, "Poppins-SemiBoldItalic"},
    {"Bold", FontWeight::Bold, FontStyle::Normal, "Poppins-Bold"},
    {"BoldItalic", FontWeight::Bold, FontStyle::Italic, "Poppins-BoldItalic"},
    {"ExtraBold", FontWeight::ExtraBold, FontStyle::Normal, "Poppins-ExtraBold"},
    {"ExtraBoldItalic", FontWeight::ExtraBold, FontStyle::Italic, "Poppins-ExtraBoldItalic"},
    {"Black", FontWeight::Black, FontStyle::Normal, "Poppins-Black"},
    {"BlackItalic", FontWeight::Black, FontStyle::Italic, "Poppins-BlackItalic"},
  }};

  for (const auto& entry : poppins) {
    RegisterFontFamily(
      "Poppins",
      entry.style,
      entry.weight,
      entry.fontStyle,
      "fonts/Poppins",
      entry.filename);
  }

  const std::array<FontRegistration, 8> ubuntu = {{
    {"Light", FontWeight::Light, FontStyle::Normal, "Ubuntu-Light"},
    {"LightItalic", FontWeight::Light, FontStyle::Italic, "Ubuntu-LightItalic"},
    {"Regular", FontWeight::Regular, FontStyle::Normal, "Ubuntu-Regular"},
    {"Italic", FontWeight::Regular, FontStyle::Italic, "Ubuntu-Italic"},
    {"Medium", FontWeight::Medium, FontStyle::Normal, "Ubuntu-Medium"},
    {"MediumItalic", FontWeight::Medium, FontStyle::Italic, "Ubuntu-MediumItalic"},
    {"Bold", FontWeight::Bold, FontStyle::Normal, "Ubuntu-Bold"},
    {"BoldItalic", FontWeight::Bold, FontStyle::Italic, "Ubuntu-BoldItalic"},
  }};

  for (const auto& entry : ubuntu) {
    RegisterFontFamily(
      "Ubuntu",
      entry.style,
      entry.weight,
      entry.fontStyle,
      "fonts/Ubuntu",
      entry.filename);
  }

  RegisterFontFamily(
    "JetBrainsMono",
    "Regular",
    FontWeight::Regular,
    FontStyle::Normal,
    "fonts/JetBrainsMono",
    "JetBrainsMonoNerdFontMono-Regular");

  RegisterLegacyAliases();
}
