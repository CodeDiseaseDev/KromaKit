//
// Created by code on 5/5/26.
//

#ifndef TEST_IOS_APP_DUIMACOSMETALHOSTCONFIG_H
#define TEST_IOS_APP_DUIMACOSMETALHOSTCONFIG_H

#include <functional>
#include <string>

#include <kromakit/Control.h>
#include <kromakit/DUIWindow.h>
#include <kromakit/platform/Platform.h>

struct NVGcontext;

struct DUIMacOSMetalHostConfig {
  Platform platform;

  std::function<void()> registerStyles;
  std::function<void()> registerHaptics;
  std::function<void(DUIWindow&)> configureWindow;
  std::function<Control*(DUIWindow&, const Platform&)> createRootControl;
  std::function<void(NVGcontext*)> loadFonts;

  bool redirectLogs = false;
  std::string logFilePath = "/tmp/synthem.log";

  bool normalScale = true;
  int preferredFramesPerSecond = 120;

  std::string windowTitle = "DirectUI";
  int initialWidth = 1280;
  int initialHeight = 720;
};

#endif //TEST_IOS_APP_DUIMACOSMETALHOSTCONFIG_H
