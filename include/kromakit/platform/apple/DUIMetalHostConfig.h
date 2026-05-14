//
// Created by code on 5/5/26.
//

#ifndef TEST_IOS_APP_DUIMETALHOSTCONFIG_H
#define TEST_IOS_APP_DUIMETALHOSTCONFIG_H

#include <kromakit/platform/Platform.h>
#include <kromakit/DUIWindow.h>

struct DUIMetalHostConfig {
  Platform platform;
  std::function<Control*(DUIWindow&, const Platform&)> createRootControl;
  std::function<void(NVGcontext*)> loadFonts;
};

#endif //TEST_IOS_APP_DUIMETALHOSTCONFIG_H
