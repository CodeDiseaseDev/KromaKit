//
// Created by code on 5/18/26.
//

#ifndef SYNTHEM_APPHOSTCONTROL_H
#define SYNTHEM_APPHOSTCONTROL_H
#include <optional>

#include "AppHostMode.h"
#include <kromakit/Control.h>
#include <kromakit/platform/Platform.h>

class MobileViewport;
class AppRootControl;

class AppHostControl : public Control {
public:
  AppHostControl();

  void SetMode(AppHostMode mode);
  AppHostMode GetMode() const;

  Platform& GetPlatform();
  const Platform& GetPlatform() const;

  std::optional<std::string> GetControlName() const override {
    return "AppHostControl";
  }

  void DoLayout(Graphics* g) override;

private:
  Platform platform;

  AppHostMode mode = AppHostMode::FullScreen;

  Control* appRoot = nullptr;
  MobileViewport* mobileViewport = nullptr;

  void RebuildModeTree();
};


#endif //SYNTHEM_APPHOSTCONTROL_H
