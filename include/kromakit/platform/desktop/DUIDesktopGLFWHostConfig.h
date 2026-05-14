//
// Created by code on 5/5/26.
//

#ifndef TEST_IOS_APP_DUIDESKTOPGLFWHOSTCONFIG_H
#define TEST_IOS_APP_DUIDESKTOPGLFWHOSTCONFIG_H
#include <nanovg.h>

#include <kromakit/DUIWindow.h>
#include <kromakit/Control.h>

#include <kromakit/platform/Platform.h>

#include <functional>

struct NVGcontext;
struct GLFWwindow;

struct DUIDesktopGLFWHostConfig {
  std::string windowTitle = "DirectUI";
  int initialWidth = 1280;
  int initialHeight = 720;

  Platform platform;

  // Called after GLFW has created the window and made the OpenGL context current.
  // Windows can use this to initialise GLAD.
  // Linux can leave this empty.
  std::function<bool()> initialiseOpenGL;

  // Render target ownership is platform/backend-specific.
  std::function<bool(int nvgFlags)> createRenderTarget;
  std::function<NVGcontext*()> getRenderTarget;
  std::function<void()> resetRenderTarget;

  // Fonts are platform/resource-path specific.
  std::function<void(NVGcontext* ctx)> loadFonts;

  // App root creation is app-specific.
  // The host will add the returned control to the DUIWindow.
  std::function<Control*(DUIWindow& window, const Platform& platform)> createRootControl;

  bool enableVSync = true;
  bool enableNanoVGDebug =
#ifndef NDEBUG
    true;
#else
      false;
#endif
};

#endif //TEST_IOS_APP_DUIDESKTOPGLFWHOSTCONFIG_H
