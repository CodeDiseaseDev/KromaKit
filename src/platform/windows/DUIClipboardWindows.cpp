#include <kromakit/platform/DUIClipboard.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct GLFWwindow;

void DUIClipboard_SetGLFWWindow(GLFWwindow* /*window*/) {
  // Windows clipboard implementation does not depend on GLFW window ownership.
}

std::string DUIClipboard::GetText() {
  if (!OpenClipboard(nullptr))
    return {};

  HANDLE handle = GetClipboardData(CF_TEXT);

  if (handle == nullptr) {
    CloseClipboard();
    return {};
  }

  char* data = static_cast<char*>(GlobalLock(handle));

  if (data == nullptr) {
    CloseClipboard();
    return {};
  }

  std::string result(data);

  GlobalUnlock(handle);
  CloseClipboard();

  return result;
}

void DUIClipboard::SetText(std::string str) {
  if (!OpenClipboard(nullptr))
    return;

  EmptyClipboard();

  HGLOBAL handle = GlobalAlloc(
    GMEM_MOVEABLE,
    str.size() + 1);

  if (handle == nullptr) {
    CloseClipboard();
    return;
  }

  char* data = static_cast<char*>(GlobalLock(handle));

  if (data == nullptr) {
    GlobalFree(handle);
    CloseClipboard();
    return;
  }

  memcpy(data, str.c_str(), str.size() + 1);

  GlobalUnlock(handle);

  SetClipboardData(CF_TEXT, handle);

  // Clipboard now owns handle after SetClipboardData succeeds.
  CloseClipboard();
}
