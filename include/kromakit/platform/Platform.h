//
// Created by code on 5/4/26.
//

#ifndef TEST_IOS_APP_PLATFORM_H
#define TEST_IOS_APP_PLATFORM_H

#include <cstdio>
#include <string>

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
    std::printf(
      "Platform {\n"
      "  versionName: \"%s\"\n"
      "  formFactor: %s\n"
      "  os: %s\n"
      "  renderer: %s\n"
      "  scaleFactor: %.2f\n"
      "  hasTouch: %s\n"
      "  hasMouse: %s\n"
      "  hasKeyboard: %s\n"
      "  hasPen: %s\n"
      "  hasHaptics: %s\n"
      "  prefersModalSheets: %s\n"
      "  prefersFloatingWindows: %s\n"
      "}\n",
      versionName.c_str(),
      ToString(formFactor),
      ToString(os),
      ToString(renderer),
      scaleFactor,
      BoolString(hasTouch),
      BoolString(hasMouse),
      BoolString(hasKeyboard),
      BoolString(hasPen),
      BoolString(hasHaptics),
      BoolString(PrefersModalSheets()),
      BoolString(PrefersFloatingWindows())
    );
  }
};

inline Platform Platform::Instance{};

#endif // TEST_IOS_APP_PLATFORM_H