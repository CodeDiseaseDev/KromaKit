//
// Created by code on 5/5/26.
//

#ifndef TEST_IOS_APP_DUIDESKTOPGLFWHOST_H
#define TEST_IOS_APP_DUIDESKTOPGLFWHOST_H
#include <kromakit/platform/desktop/DUIDesktopGLFWHostConfig.h>
#include <kromakit/platform/desktop/GLFWKeyboard.h>
#include <kromakit/platform/DUIKeyboard.h>
#include <kromakit/platform/AppHostControl.h>


class DUIDesktopGLFWHost {
public:
  explicit DUIDesktopGLFWHost(DUIDesktopGLFWHostConfig config);

  int Run();

  bool InitGLFWKeyboard();

  DUIWindow& GetWindow();
  const DUIWindow& GetWindow() const;

private:
  bool InitialiseGLFW();
  bool CreateWindow();
  bool CreateNanoVGContext();
  bool CreateGraphics();
  bool LoadFonts();
  bool CreateRootControl();

  void InstallCallbacks();
  void SyncWindowMetrics();
  void MainLoop();
  void Shutdown();

  static DUIDesktopGLFWHost* GetHost(GLFWwindow* window);

  static void CursorPositionCallback(GLFWwindow* window, double x, double y);
  static void CursorEnterCallback(GLFWwindow* window, int entered);
  static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void CharCallback(GLFWwindow *window, unsigned int codepoint);

  static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
  DUIDesktopGLFWHostConfig config_;

  DUIWindow window_;
  GLFWwindow* glfwWindow_ = nullptr;
  Control* rootControl_ = nullptr;

  DUIKeyboard duiKeyboard_;
  GLFWKeyboard keyboard_;
};



#endif //TEST_IOS_APP_DUIDESKTOPGLFWHOST_H
