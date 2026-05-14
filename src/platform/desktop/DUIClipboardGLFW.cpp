#include <kromakit/platform/desktop/DUIClipboardGLFW.h>

#include <kromakit/platform/DUIClipboard.h>


#define GLFW_INCLUDE_NONE 1
#include <GLFW/glfw3.h>

namespace {
  GLFWwindow* gClipboardWindow = nullptr;
}

void DUIClipboard_SetGLFWWindow(GLFWwindow* window) {
  gClipboardWindow = window;
}

std::string DUIClipboard::GetText() {
  const char* text = glfwGetClipboardString(gClipboardWindow);

  if (text == nullptr)
    return {};

  return std::string(text);
}

void DUIClipboard::SetText(std::string str) {
  glfwSetClipboardString(
    gClipboardWindow,
    str.c_str());
}
