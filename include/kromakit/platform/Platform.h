//
// Created by code on 5/4/26.
//

#ifndef TEST_IOS_APP_PLATFORM_H
#define TEST_IOS_APP_PLATFORM_H

#include <cstdio>
#include <string>
#include <kromakit/Logging.h>

// struct Platform;

struct Platform {

  static Platform Instance;

  enum class FormFactor {
    Mobile, Tablet, Desktop, TV, Embedded
  };

  enum class OS {
    iOS, Windows, Linux, macOS, tvOS, Unknown
  };

  enum class Renderer {
    Metal,
    OpenGL,
    Direct3D,
    Vulkan,
    Unknown
  };

  std::string versionName;
  float scaleFactor = 1.0f;

  bool hasTouch = false;
  bool hasMouse = true;
  bool hasKeyboard = true;
  bool hasPen = false;
  bool hasHaptics = false;

  FormFactor formFactor = FormFactor::Desktop;
  OS os = OS::Unknown;
  Renderer renderer = Renderer::Unknown;

  void UseDesktopProperties(OS os_, Renderer renderer_) {
    hasTouch = false;
    hasMouse = true;
    hasKeyboard = true;
    hasPen = false;
    hasHaptics = false;
    scaleFactor = 1.0f;

    formFactor = FormFactor::Desktop;
    os = os_;
    renderer = renderer_;
  }

  void UseMobileProperties(
    OS os_, Renderer renderer_,
    float scale = 1.0f) {

    hasTouch = true;
    hasMouse = false;
    hasKeyboard = false;
    hasPen = false;
    hasHaptics = true;
    scaleFactor = scale;

    formFactor = FormFactor::Mobile;
    os = os_;
    renderer = renderer_;
  }

  static const char* ToString(FormFactor value) {
    switch (value) {
      case FormFactor::Mobile:   return "Mobile";
      case FormFactor::Tablet:   return "Tablet";
      case FormFactor::Desktop:  return "Desktop";
      case FormFactor::TV:       return "TV";
      case FormFactor::Embedded: return "Embedded";
    }

    return "Unknown";
  }

  static const char* ToString(OS value) {
    switch (value) {
      case OS::iOS:     return "iOS";
      case OS::Windows: return "Windows";
      case OS::Linux:   return "Linux";
      case OS::macOS:   return "macOS";
      case OS::tvOS:    return "tvOS";
      case OS::Unknown: return "Unknown";
    }

    return "Unknown";
  }

  static const char* ToString(Renderer value) {
    switch (value) {
      case Renderer::Metal:   return "Metal";
      case Renderer::OpenGL:  return "OpenGL";
      case Renderer::Direct3D:return "Direct3D";
      case Renderer::Vulkan:  return "Vulkan";
      case Renderer::Unknown: return "Unknown";
    }

    return "Unknown";
  }

  static const char* BoolString(bool value) {
    return value ? "true" : "false";
  }

  bool IsMobile() const {
    return formFactor == FormFactor::Mobile ||
           formFactor == FormFactor::Tablet;
  }

  bool IsDesktop() const {
    return formFactor == FormFactor::Desktop;
  }

  bool PrefersModalSheets() const {
    return IsMobile();
  }

  bool PrefersFloatingWindows() const {
    return IsDesktop();
  }

  void Print() const {
    const bool prefersModalSheets = PrefersModalSheets();
    const bool prefersFloatingWindows = PrefersFloatingWindows();

    const char* reset   = Logging::TerminalColor::Reset();
    const char* dim     = Logging::TerminalColor::Dim();

    const char* title   = Logging::TerminalColor::BrightGreen();
    const char* key     = Logging::TerminalColor::BrightBlue();
    const char* string  = Logging::TerminalColor::BrightMagenta();
    const char* value   = Logging::TerminalColor::BrightYellow();
    const char* number  = Logging::TerminalColor::BrightBlue();

    const char* boolOn  = Logging::TerminalColor::BrightGreen();
    const char* boolOff = Logging::TerminalColor::BrightBlack();

    auto BoolColor = [boolOn, boolOff](bool v) {
      return v ? boolOn : boolOff;
    };

    Logging::Log(
      "\n"
      "%sPlatform %s{\n"
      "  %sversionName%s: %s\"%s\"%s\n"
      "  %sformFactor%s: %s%s%s\n"
      "  %sos%s: %s%s%s\n"
      "  %srenderer%s: %s%s%s\n"
      "  %sscaleFactor%s: %s%.2f%s\n"
      "  %shasTouch%s: %s%s%s\n"
      "  %shasMouse%s: %s%s%s\n"
      "  %shasKeyboard%s: %s%s%s\n"
      "  %shasPen%s: %s%s%s\n"
      "  %shasHaptics%s: %s%s%s\n"
      "  %sprefersModalSheets%s: %s%s%s\n"
      "  %sprefersFloatingWindows%s: %s%s%s\n"
      "%s}%s\n",

      title, reset,

      key, reset, string, versionName.c_str(), reset,
      key, reset, value, ToString(formFactor), reset,
      key, reset, value, ToString(os), reset,
      key, reset, value, ToString(renderer), reset,
      key, reset, number, scaleFactor, reset,

      key, reset, BoolColor(hasTouch), BoolString(hasTouch), reset,
      key, reset, BoolColor(hasMouse), BoolString(hasMouse), reset,
      key, reset, BoolColor(hasKeyboard), BoolString(hasKeyboard), reset,
      key, reset, BoolColor(hasPen), BoolString(hasPen), reset,
      key, reset, BoolColor(hasHaptics), BoolString(hasHaptics), reset,

      key, reset, BoolColor(prefersModalSheets), BoolString(prefersModalSheets), reset,
      key, reset, BoolColor(prefersFloatingWindows), BoolString(prefersFloatingWindows), reset,

      dim, reset
    );
  }
};

inline Platform Platform::Instance{};

#endif // TEST_IOS_APP_PLATFORM_H