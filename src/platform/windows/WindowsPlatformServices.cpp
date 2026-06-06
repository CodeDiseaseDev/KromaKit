//
// Created by code on 5/30/26.
//



#ifdef _WIN32
#include <kromakit/platform/PlatformServices.h>
#include <kromakit/platform/NullFilePicker.h>

#include <memory>

#include <kromakit/platform/PlatformProvider.h>
#include <kromakit/platform/windows/WindowsFilePicker.h>
#include <kromakit/platform/windows/WindowsMainThreadDispatcher.h>

#include <windows.h>
#include <string>

namespace {
  std::wstring ToWide(const std::string& text) {
    if (text.empty()) {
      return {};
    }

    const int size = MultiByteToWideChar(
      CP_UTF8,
      0,
      text.c_str(),
      -1,
      nullptr,
      0);

    std::wstring result(size, L'\0');

    MultiByteToWideChar(
      CP_UTF8,
      0,
      text.c_str(),
      -1,
      result.data(),
      size);

    if (!result.empty() && result.back() == L'\0') {
      result.pop_back();
    }

    return result;
  }
}

std::shared_ptr<IFilePicker> PlatformServices::FilePicker() {
  static std::shared_ptr<IFilePicker> picker =
    std::make_shared<WindowsFilePicker>();

  return picker;
}

void PlatformServices::DisplayMessageBox_(
  std::string title,
  std::string message
) {
  const auto wideTitle = ToWide(title);
  const auto wideMessage = ToWide(message);

  MessageBoxW(
    nullptr,
    wideMessage.c_str(),
    wideTitle.c_str(),
    MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);
}



std::shared_ptr<IMainThreadDispatcher>
PlatformServices::MainThreadDispatcher() {
  static std::shared_ptr<IMainThreadDispatcher> dispatcher =
    std::make_shared<WindowsMainThreadDispatcher>();

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


#endif