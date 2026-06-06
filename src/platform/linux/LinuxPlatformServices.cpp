//
// Created by code on 5/30/26.
//

#if defined(__linux__)

#include <kromakit/platform/PlatformServices.h>
#include <kromakit/platform/linux/LinuxFilePicker.h>
#include <kromakit/platform/linux/LinuxMainThreadDispatcher.h>

#include <memory>

#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace {
  bool TryZenityMessageBox(
    const std::string& title,
    const std::string& message
  ) {
    const pid_t pid = fork();

    if (pid < 0) {
      return false;
    }

    if (pid == 0) {
      execlp(
        "zenity",
        "zenity",
        "--info",
        "--title",
        title.c_str(),
        "--text",
        message.c_str(),
        nullptr);

      _exit(127);
    }

    int status = 0;
    waitpid(pid, &status, 0);

    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
  }
}

std::shared_ptr<IMainThreadDispatcher>
PlatformServices::MainThreadDispatcher() {
  static std::shared_ptr<IMainThreadDispatcher> dispatcher =
    std::make_shared<LinuxMainThreadDispatcher>();

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

void PlatformServices::DisplayMessageBox_(
  std::string title,
  std::string message
) {
  if (TryZenityMessageBox(title, message)) {
    return;
  }

  std::cerr
    << "\n[" << title << "]\n"
    << message << "\n";
}

std::shared_ptr<IFilePicker> PlatformServices::FilePicker() {
  static std::shared_ptr<IFilePicker> picker =
    std::make_shared<LinuxFilePicker>();

  return picker;
}

#endif
