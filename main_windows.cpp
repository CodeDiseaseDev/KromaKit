

#include <cstdlib>
#include <iostream>

#define GLFW_INCLUDE_NONE 1
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <kromakit/platform/desktop/DUIDesktopGLFWHost.h>
#include <kromakit/platform/windows/OpenGLNanoVGContextOwner.h>
#include <kromakit/platform/linux/FontLoaderLinux.h>
#include <kromakit/platform/PlatformProvider.h>
#include <kromakit/styling/DUIDefaultStyles.h>

#include "synthem/Synthem.h"

#include "synthem/versioning/AppVersion.h"

#include <windows.h>

#include "synthem/AppRootControl.h"

namespace {
  const bool kStylesRegistered = []() {
    DUIDefaultStyles::RegisterAll();
    SynthemTheme::RegisterAll();
    return true;
  }();

  static OpenGLNanoVGContextOwner gRenderTargetOwner;
  static AppRootControl gMainControl;
}

int synthem_main(int argc, char* argv[]) {
  (void)argc, (void)argv;

  DUIDesktopGLFWHostConfig config;

  Platform::Instance = PlatformProvider::Create(
    Platform::Renderer::OpenGL
  );

  config.windowTitle = "Synthem for Windows " + AppVersion::FullString();
  config.platform = Platform::Instance;

  config.initialiseOpenGL = []() {
    if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) {
      std::cerr << "Failed to initialise GLAD.\n";
      return false;
    }

    return true;
  };

  config.createRenderTarget = [](int flags) {
    return gRenderTargetOwner.Create(flags);
  };

  config.getRenderTarget = []() {
    return gRenderTargetOwner.Get();
  };

  config.resetRenderTarget = []() {
    gRenderTargetOwner.Reset();
  };

  config.loadFonts = [](NVGcontext* ctx) {
    FontLoaderLinux fontLoader(ctx);
    fontLoader.LoadBundledFonts();
  };

  config.createRootControl = [](DUIWindow& window, const Platform& platform) -> Control* {
    gMainControl.synthemRoot->devicePlatform = platform;
    gMainControl.synthemRoot->devicePlatform.Print();

    if (!gMainControl.synthemRoot->UserInitialise(&window)) {
      return nullptr;
    }

    return &gMainControl;
  };

  DUIDesktopGLFWHost host(config);
  return host.Run();
}

int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow
) {
  return synthem_main(__argc, __argv);
}