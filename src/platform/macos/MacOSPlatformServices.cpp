//
// Created by code on 5/30/26.
//

#include <kromakit/platform/PlatformServices.h>
#include <kromakit/platform/NullFilePicker.h>
#include <kromakit/platform/PlatformServices.h>
#include <kromakit/platform/apple/AppleMainThreadDispatcher.h>

#include <memory>

std::shared_ptr<IFilePicker> PlatformServices::FilePicker() {
  static std::shared_ptr<IFilePicker> picker =
    std::make_shared<NullFilePicker>();

  return picker;
}



std::shared_ptr<IMainThreadDispatcher>
PlatformServices::MainThreadDispatcher() {
  static std::shared_ptr<IMainThreadDispatcher> dispatcher =
    std::make_shared<AppleMainThreadDispatcher>();

  return dispatcher;
}

void PlatformServices::PostToMainThread(
  std::function<void()> action
) {
  MainThreadDispatcher()->Post(std::move(action));
}

void PlatformServices::DispatchOrRunOnMainThread(
  std::function<void()> action
) {
  MainThreadDispatcher()->DispatchOrRun(std::move(action));
}

void PlatformServices::DrainMainThreadTasks() {
  MainThreadDispatcher()->Drain();
}