#import <Foundation/Foundation.h>

#include <cstdio>
#include <string>
#include <utility>

#include <kromakit/HapticsBridge.h>
#include <kromakit/platform/Platform.h>
#include <kromakit/platform/macos/DUIMacOSMetalHost.h>
#include <kromakit/platform/macos/FontLoaderMacOS.h>
#include <kromakit/styling/DUIDefaultStyles.h>

namespace {

std::string NSStringToStdString(NSString* value) {
  if (value == nil) {
    return {};
  }

  return std::string([value UTF8String]);
}

Platform CreateMacOSPlatform() {
  Platform platform;

  platform.UseDesktopProperties(
    Platform::OS::macOS,
    Platform::Renderer::Metal
  );

  NSOperatingSystemVersion version =
    NSProcessInfo.processInfo.operatingSystemVersion;

  platform.versionName =
    "macOS " +
    std::to_string(version.majorVersion) + "." +
    std::to_string(version.minorVersion) + "." +
    std::to_string(version.patchVersion);

  /*platform.versionName = NSStringToStdString(
    NSProcessInfo.processInfo.operatingSystemVersionString
  );*/

  // Runtime backing scale is synced by the AppKit host from NSWindow.
  platform.scaleFactor = 1.0f;

  platform.hasTouch = false;
  platform.hasMouse = true;
  platform.hasKeyboard = true;
  platform.hasPen = false;
  platform.hasHaptics = false;

  return platform;
}

} // namespace

int main(int argc, char* argv[]) {
  Logging::TerminalColor::Enable();

  DUIMacOSMetalHostConfig config;

  config.platform = CreateMacOSPlatform();
  config.windowTitle = "kromakit for macOS";
  config.initialWidth = 1280;
  config.initialHeight = 800;

  config.registerStyles = []() {
    fprintf(stderr, "----------------------------------------\n");
    //fprintf(stderr, "Synthem - %s\n", AppVersion::FullString().c_str());
    fprintf(stderr, "----------------------------------------\n\n");
    fflush(stderr);

    DUIDefaultStyles::RegisterAll();
    //SynthemTheme::RegisterAll();
  };

  config.registerHaptics = []() {
    /*DirectUIHaptics::Register(
      Haptics_Impact,
      Haptics_Selection,
      Haptics_Notify
    );*/
  };

  config.configureWindow = [](DUIWindow& window) {
    //window.UseStyle(SynthemTheme::RootWindow::Default);
  };

  config.createRootControl = [](DUIWindow& window, const Platform& platform) -> Control* {
    static Panel root;

    //root.synthemRoot->devicePlatform = platform;

    //if (!root.synthemRoot->UserInitialise(&window)) {
    //  return nullptr;
    //}

    return &root;
  };

  config.loadFonts = [](NVGcontext* ctx) {
    LoadBundledFontsMacOS(ctx);
  };

  config.redirectLogs = true;
  config.logFilePath = "/tmp/synthem.log";
  config.normalScale = true;
  config.preferredFramesPerSecond = 120;

  return DUIRunMacOSMetalHost(argc, argv, std::move(config));
}
