#import <UIKit/UIKit.h>

#include <string>
#include <utility>

#include <kromakit/HapticsBridge.h>
#include <kromakit/platform/Platform.h>
#include <kromakit/platform/ios/DUIIOSMetalHost.h>
#include <kromakit/platform/ios/FontLoader.h>
#include <kromakit/styling/DUIDefaultStyles.h>

namespace {

std::string NSStringToStdString(NSString* value) {
  if (value == nil) {
    return {};
  }

  return std::string([value UTF8String]);
}

std::string GetIOSVersionName() {
  UIDevice* device = [UIDevice currentDevice];

  NSString* systemName = [device systemName];
  NSString* systemVersion = [device systemVersion];

  std::string name = NSStringToStdString(systemName);
  std::string version = NSStringToStdString(systemVersion);

  if (name.empty()) {
    name = "iOS";
  }

  if (!version.empty()) {
    name += " ";
    name += version;
  }

  return name;
}

Platform CreateIOSPlatform() {
  Platform platform;

  platform.UseMobileProperties(
    Platform::OS::iOS,
    Platform::Renderer::Metal,
    static_cast<float>([UIScreen mainScreen].scale)
  );

  platform.versionName = GetIOSVersionName();

  platform.hasTouch = true;
  platform.hasMouse = false;
  platform.hasKeyboard = false;
  platform.hasPen = false;
  platform.hasHaptics = true;

  return platform;
}

} // namespace

int main(int argc, char* argv[]) {
  DUIIOSMetalHostConfig config;

  config.platform = CreateIOSPlatform();

  config.registerStyles = []() {
    fprintf(stderr, "----------------------------------------\n");
    //fprintf(stderr, "Synthem - %s\n", AppVersion::FullString().c_str());
    fprintf(stderr, "----------------------------------------\n\n");
    fflush(stderr);

    DUIDefaultStyles::RegisterAll();
    //SynthemTheme::RegisterAll();
  };

  config.registerHaptics = []() {

  };

  config.configureWindow = [](DUIWindow& window) {
    //window.UseStyle(SynthemTheme::RootWindow::Default);
  };

  config.onKeyboardFrameChanged = [](bool visible, DUIRect keyboardFrame) {
    /*if (root.keyboard_aware_panel != nullptr) {
      root.keyboard_aware_panel->SetKeyboardFrame(keyboardFrame);
      root.keyboard_aware_panel->SetKeyboardVisible(visible);
    }*/
  };

  config.createRootControl = [](DUIWindow& window, const Platform& platform) -> Control* {
    static Panel root;
    return &root;
  };

  config.loadFonts = [](NVGcontext* ctx) {
    static FontLoader* fontLoader = nil;

    if (fontLoader == nil) {
      fontLoader = [[FontLoader alloc] initWithContext:ctx];
    }

    [fontLoader loadBundledFonts];
  };

  config.redirectLogs = true;
  config.logFilePath = "/tmp/synthem.log";
  config.normalScale = true;
  config.preferredFramesPerSecond = 120;

  return DUIRunIOSMetalHost(argc, argv, std::move(config));
}
