
#include <cstdlib>

#include <kromakit/platform/desktop/DUIDesktopGLFWHost.h>
#include <kromakit/platform/desktop/DUIDesktopGLFWHostConfig.h>
#include <kromakit/platform/linux/OpenGLNanoVGContextOwner.h>
#include <include/kromakit/platform/desktop/FontLoaderDesktop.h>
#include <kromakit/platform/PlatformProvider.h>
#include <kromakit/styling/DUIDefaultStyles.h>
#include <kromakit/Button.h>

namespace {
  const bool kStylesRegistered = []() {
    DUIDefaultStyles::RegisterAll();
    //SynthemTheme::RegisterAll();
    return true;
  }();

  static OpenGLNanoVGContextOwner gRenderTargetOwner;
  //static AppRootControl gMainControl;


}



int main() {
  Logging::TerminalColor::Enable();

  DUIDesktopGLFWHostConfig config;

  Platform::Instance = PlatformProvider::Create(
    Platform::Renderer::OpenGL
  );

  config.windowTitle = "kromakit for Linux(btw)";
  config.platform = Platform::Instance;

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
    FontLoaderDesktop fontLoader(ctx);
    fontLoader.LoadBundledFonts();
  };

  config.createRootControl = [](DUIWindow& window, const Platform& platform) -> Control* {
    //gMainControl.synthemRoot->devicePlatform = platform;
    //gMainControl.synthemRoot->devicePlatform.Print();

    //if (!gMainControl.synthemRoot->UserInitialise(&window)) {
   //   return nullptr;
    //}

    static ScrollableVStackPanel scrollableVStackPanel;
    // static Button button;
    //
    // button.location = { 20, 20 };
    // button.size = { 150, 30 };
    // button.SetContent("Click Me");

    static Button* btn = scrollableVStackPanel.Stack().CreateControl<Button>("click me");
    btn->OnClick = [] {
      btn->SetContent("hi");
    };

    return &scrollableVStackPanel;
  };

  DUIDesktopGLFWHost host(config);
  return host.Run();
}
