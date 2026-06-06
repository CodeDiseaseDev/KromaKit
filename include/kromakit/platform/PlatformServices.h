//
// Created by code on 5/30/26.
//

#ifndef SYNTHEM_PLATFORMSERVICES_H
#define SYNTHEM_PLATFORMSERVICES_H
#include "IFilePicker.h"
#include <kromakit/platform/IMainThreadDispatcher.h>

#include <memory>

class PlatformServices {
public:
  static std::shared_ptr<IFilePicker> FilePicker();

  static void DisplayMessageBox_(std::string title, std::string message);

  static std::shared_ptr<IMainThreadDispatcher> MainThreadDispatcher();

  static void PostToMainThread(
    std::function<void()> action
  );

  static void DispatchOrRunOnMainThread(
    std::function<void()> action
  );

  static void DrainMainThreadTasks();
};

#endif //SYNTHEM_PLATFORMSERVICES_H
