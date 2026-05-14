//
// Created by code on 5/5/26.
//

#ifndef TEST_IOS_APP_DUIIOSMETALHOSTCONFIG_H
#define TEST_IOS_APP_DUIIOSMETALHOSTCONFIG_H

#include <functional>
#include <string>

#include <kromakit/Control.h>
#include <kromakit/DUIWindow.h>
#include <kromakit/platform/Platform.h>

struct NVGcontext;

struct DUIIOSMetalHostConfig {
  Platform platform;

  std::function<void()> registerStyles;
  std::function<void()> registerHaptics;
  std::function<void(DUIWindow&)> configureWindow;

  std::function<void(bool, DUIRect)> onKeyboardFrameChanged;

  // App-owned borrowed root control, attached to the host window.
  std::function<Control*(DUIWindow&, const Platform&)> createRootControl;

  // Optional callback for bundled/platform font loading.
  std::function<void(NVGcontext*)> loadFonts;

  bool redirectLogs = false;
  std::string logFilePath = "/tmp/synthem.log";

  bool normalScale = true;
  int preferredFramesPerSecond = 120;
};

#endif //TEST_IOS_APP_DUIIOSMETALHOSTCONFIG_H
