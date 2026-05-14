//
// Created by code on 5/4/26.
//

#ifndef TEST_IOS_APP_PLATFORMPROVIDER_H
#define TEST_IOS_APP_PLATFORMPROVIDER_H

#include <kromakit/platform/Platform.h>

#include <string>
#include <cstring>
#include <fstream>

#ifdef __linux__
#include <sys/utsname.h>
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#endif

class PlatformProvider {
public:
  static Platform Create(Platform::Renderer renderer) {
#if defined(_WIN32)
    return CreateWindowsPlatform(renderer);
#elif defined(__linux__)
    return CreateLinuxPlatform(renderer);
#elif defined(__APPLE__)
    // Apple targets should usually provide a more specific platform from
    // main.mm, because iOS/macOS/tvOS detection may need Foundation/UIKit.
    return CreateAppleFallbackPlatform(renderer);
#else
    return CreateUnknownPlatform(renderer);
#endif
  }

  static Platform CreateDesktop(
    Platform::OS os,
    Platform::Renderer renderer,
    const std::string& versionName
  ) {
    Platform platform;

    platform.UseDesktopProperties(os, renderer);
    platform.versionName = versionName;

    return platform;
  }

  static Platform CreateMobile(
    Platform::OS os,
    Platform::Renderer renderer,
    float scaleFactor,
    const std::string& versionName
  ) {
    Platform platform;

    platform.UseMobileProperties(os, renderer, scaleFactor);
    platform.versionName = versionName;

    return platform;
  }

private:
  static std::string TrimQuotes(std::string value) {
    if (value.size() >= 2 &&
        value.front() == '"' &&
        value.back() == '"') {
      return value.substr(1, value.size() - 2);
    }

    return value;
  }

#if defined(__linux__)
  static std::string GetLinuxDistroName() {
    std::ifstream file("/etc/os-release");
    if (!file.is_open()) {
      return {};
    }

    std::string line;
    while (std::getline(file, line)) {
      constexpr const char* prettyNameKey = "PRETTY_NAME=";
      constexpr std::size_t prettyNameKeyLength = 12;

      if (line.rfind(prettyNameKey, 0) == 0) {
        return TrimQuotes(line.substr(prettyNameKeyLength));
      }
    }

    return {};
  }

  static std::string GetLinuxVersionName() {
    const std::string distro = GetLinuxDistroName();

    utsname info{};
    if (uname(&info) != 0) {
      return distro.empty() ? "Linux" : distro;
    }

    std::string name = distro.empty() ? "Linux" : distro;

    if (info.release[0] != '\0') {
      name += " (kernel ";
      name += info.release;
      name += ")";
    }

    return name;
  }

  static Platform CreateLinuxPlatform(Platform::Renderer renderer) {
    Platform platform;

    platform.UseDesktopProperties(
      Platform::OS::Linux,
      renderer
    );

    platform.versionName = GetLinuxVersionName();

    return platform;
  }
#endif

#if defined(_WIN32)
  using RtlGetVersionPtr = LONG (WINAPI*)(PRTL_OSVERSIONINFOW);

  static std::string GetWindowsVersionName() {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll == nullptr) {
      return "Windows";
    }

    auto rtlGetVersion = reinterpret_cast<RtlGetVersionPtr>(
      GetProcAddress(ntdll, "RtlGetVersion")
    );

    if (rtlGetVersion == nullptr) {
      return "Windows";
    }

    RTL_OSVERSIONINFOW info{};
    info.dwOSVersionInfoSize = sizeof(info);

    if (rtlGetVersion(&info) != 0) {
      return "Windows";
    }

    std::string friendlyName = "Windows ";

    if (info.dwMajorVersion == 10 && info.dwMinorVersion == 0) {
      friendlyName += (info.dwBuildNumber >= 22000)
        ? "11"
        : "10";
    } else {
      friendlyName += std::to_string(info.dwMajorVersion);
      friendlyName += ".";
      friendlyName += std::to_string(info.dwMinorVersion);
    }

    friendlyName += " build ";
    friendlyName += std::to_string(info.dwBuildNumber);

    return friendlyName;
  }

  static Platform CreateWindowsPlatform(Platform::Renderer renderer) {
    Platform platform;

    platform.UseDesktopProperties(
      Platform::OS::Windows,
      renderer
    );

    platform.versionName = GetWindowsVersionName();

    return platform;
  }
#endif

#if defined(__APPLE__)
  static Platform CreateAppleFallbackPlatform(Platform::Renderer renderer) {
    Platform platform;

    // Keep this conservative. iOS/macOS/tvOS should ideally be created
    // from the Apple host file where UIKit/Foundation is available.
    platform.UseDesktopProperties(
      Platform::OS::macOS,
      renderer
    );

    platform.versionName = "Apple";

    return platform;
  }
#endif

  static Platform CreateUnknownPlatform(Platform::Renderer renderer) {
    Platform platform;

    platform.UseDesktopProperties(
      Platform::OS::Unknown,
      renderer
    );

    platform.versionName = "Unknown";

    return platform;
  }
};

#endif // TEST_IOS_APP_PLATFORMPROVIDER_H
