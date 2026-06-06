//
// Created by code on 5/4/26.
//

#include <kromakit/platform/desktop/DUIDesktopGLFWHost.h>
// #include "../DUIDesktopGLFWHost.h"

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>

#if defined(_WIN32)
  #ifndef GLFW_INCLUDE_NONE
  #define GLFW_INCLUDE_NONE 1
  #endif
  #include <glad/glad.h>
  #include <GLFW/glfw3.h>
#elif defined(__APPLE__)
  #define GL_SILENCE_DEPRECATION
  #include <OpenGL/gl3.h>
  #include <GLFW/glfw3.h>
#else
  #ifndef GL_GLEXT_PROTOTYPES
  #define GL_GLEXT_PROTOTYPES 1
  #endif
  #include <GLFW/glfw3.h>
#endif

#include <kromakit/platform/desktop/DUIClipboardGLFW.h>
#include <nanovg_gl.h>

namespace {
MouseButton MapGLFWMouseButton(const int button) {
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      return MouseButton::Left;
    case GLFW_MOUSE_BUTTON_RIGHT:
      return MouseButton::Right;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      return MouseButton::Middle;
    default:
      return MouseButton::None;
  }
}
}


DUIDesktopGLFWHost::DUIDesktopGLFWHost(DUIDesktopGLFWHostConfig config)
  : config_(std::move(config)) {
}

DUIWindow& DUIDesktopGLFWHost::GetWindow() {
  return window_;
}

const DUIWindow& DUIDesktopGLFWHost::GetWindow() const {
  return window_;
}

int DUIDesktopGLFWHost::Run() {
  if (!InitGLFWKeyboard()) {
    return EXIT_FAILURE;
  }

  if (!InitialiseGLFW()) {
    return EXIT_FAILURE;
  }

  if (!CreateWindow()) {
    Shutdown();
    return EXIT_FAILURE;
  }

  if (config_.initialiseOpenGL != nullptr && !config_.initialiseOpenGL()) {
    std::cerr << "Failed to initialise OpenGL loader.\n";
    Shutdown();
    return EXIT_FAILURE;
  }

  if (!CreateNanoVGContext()) {
    Shutdown();
    return EXIT_FAILURE;
  }

  if (!CreateGraphics()) {
    Shutdown();
    return EXIT_FAILURE;
  }

  if (!LoadFonts()) {
    Shutdown();
    return EXIT_FAILURE;
  }

  if (!CreateRootControl()) {
    Shutdown();
    return EXIT_FAILURE;
  }

  InstallCallbacks();
  SyncWindowMetrics();

  MainLoop();
  Shutdown();

  return EXIT_SUCCESS;
}

bool DUIDesktopGLFWHost::InitGLFWKeyboard() {
  keyboard_.OnKeyDown = [this](const DUIKeyEvent& event) {
    duiKeyboard_.OnKeyDown(event);
  };
  keyboard_.OnKeyUp = [this](const DUIKeyEvent& event) {
    duiKeyboard_.OnKeyUp(event);
  };
  keyboard_.OnTextInput = [this](const DUITextInputEvent& event) {
    duiKeyboard_.OnTextInput(event);
  };

  duiKeyboard_.OnKeyDownEvent = [this](const DUIKeyEvent& event) {
    window_.OnKeyDown(event);
  };

  duiKeyboard_.OnKeyUpEvent = [this](const DUIKeyEvent& event) {
    window_.OnKeyUp(event);
  };

  duiKeyboard_.OnTextInput = [this](const DUITextInputEvent& event) {
    window_.OnTextInput(event);
  };

  return true;
}

bool DUIDesktopGLFWHost::InitialiseGLFW() {
  if (glfwInit() != GLFW_TRUE) {
    std::cerr << "Failed to initialize GLFW.\n";
    return false;
  }

  return true;
}

bool DUIDesktopGLFWHost::CreateWindow() {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  glfwWindowHint(GLFW_STENCIL_BITS, 8);

  glfwWindow_ = glfwCreateWindow(
    config_.initialWidth,
    config_.initialHeight,
    config_.windowTitle.c_str(),
    nullptr,
    nullptr
  );

  if (glfwWindow_ == nullptr) {
    std::cerr << "Failed to create GLFW window.\n";
    return false;
  }

  DUIClipboard_SetGLFWWindow(glfwWindow_);

  glfwMakeContextCurrent(glfwWindow_);
  glfwSwapInterval(config_.enableVSync ? 1 : 0);

  glfwSetWindowUserPointer(glfwWindow_, this);

  return true;
}

bool DUIDesktopGLFWHost::CreateNanoVGContext() {
  if (config_.createRenderTarget == nullptr ||
      config_.getRenderTarget == nullptr ||
      config_.resetRenderTarget == nullptr) {
    std::cerr << "DUIDesktopGLFWHost missing render target callbacks.\n";
    return false;
  }

  int nvgFlags = NVG_ANTIALIAS | NVG_STENCIL_STROKES;

  if (config_.enableNanoVGDebug) {
    nvgFlags |= NVG_DEBUG;
  }

  if (!config_.createRenderTarget(nvgFlags)) {
    std::cerr << "Failed to create NanoVG GL3 context.\n";
    return false;
  }

  return true;
}

bool DUIDesktopGLFWHost::CreateGraphics() {
  window_.create_render_target(config_.getRenderTarget());

  if (window_.graphics == nullptr) {
    std::cerr << "Failed to create DirectUI Graphics wrapper.\n";
    return false;
  }

  return true;
}

bool DUIDesktopGLFWHost::LoadFonts() {
  if (config_.loadFonts == nullptr) {
    return true;
  }

  config_.loadFonts(config_.getRenderTarget());
  return true;
}

bool DUIDesktopGLFWHost::CreateRootControl() {
  if (config_.createRootControl == nullptr) {
    std::cerr << "DUIDesktopGLFWHost missing root control callback.\n";
    return false;
  }

  rootControl_ = config_.createRootControl(window_, config_.platform);

  if (rootControl_ == nullptr) {
    std::cerr << "Failed to create DirectUI root control.\n";
    return false;
  }

  window_.AddBorrowedControl(rootControl_);

  rootControl_->location = {0.0f, 0.0f};
  rootControl_->size = window_.size;

  return true;
}

void DUIDesktopGLFWHost::InstallCallbacks() {
  glfwSetCursorPosCallback(glfwWindow_, CursorPositionCallback);
  glfwSetCursorEnterCallback(glfwWindow_, CursorEnterCallback);
  glfwSetMouseButtonCallback(glfwWindow_, MouseButtonCallback);
  glfwSetKeyCallback(glfwWindow_, KeyCallback);
  glfwSetScrollCallback(glfwWindow_, ScrollCallback);
  glfwSetFramebufferSizeCallback(glfwWindow_, FramebufferSizeCallback);

  glfwSetKeyCallback(glfwWindow_, KeyCallback);
  glfwSetCharCallback(glfwWindow_, CharCallback);
}

void DUIDesktopGLFWHost::SyncWindowMetrics() {
  if (glfwWindow_ == nullptr) {
    return;
  }

  int windowWidth = 0;
  int windowHeight = 0;
  int framebufferWidth = 0;
  int framebufferHeight = 0;

  glfwGetWindowSize(glfwWindow_, &windowWidth, &windowHeight);
  glfwGetFramebufferSize(glfwWindow_, &framebufferWidth, &framebufferHeight);

  if (windowWidth <= 0 || windowHeight <= 0 ||
      framebufferWidth <= 0 || framebufferHeight <= 0) {
    return;
  }

  const float pixelRatioX =
    static_cast<float>(framebufferWidth) / static_cast<float>(windowWidth);

  window_.render_scale = pixelRatioX;

  window_.SetSize(
    static_cast<float>(windowWidth),
    static_cast<float>(windowHeight)
  );

  if (rootControl_ != nullptr) {
    rootControl_->location = {0.0f, 0.0f};
    rootControl_->size = {
      static_cast<float>(windowWidth),
      static_cast<float>(windowHeight)
    };
  }

  glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void DUIDesktopGLFWHost::MainLoop() {
  using Clock = std::chrono::steady_clock;

  auto lastFrameTime = Clock::now();

  while (!glfwWindowShouldClose(glfwWindow_)) {
    const auto now = Clock::now();

    float deltaTime = std::chrono::duration<float>(
      now - lastFrameTime
    ).count();

    lastFrameTime = now;

    // Avoid giant physics jumps after dragging windows, breakpoints, stalls, etc.
    deltaTime = std::clamp(deltaTime, 0.0f, 1.0f / 15.0f);

    window_.WindowRender(deltaTime);

    glfwSwapBuffers(glfwWindow_);
    glfwPollEvents();
  }
}

void DUIDesktopGLFWHost::Shutdown() {
  window_.BeginShutdown(DUIWindowCloseReason
    ::NativeWindowClose);

  if (config_.resetRenderTarget != nullptr) {
    config_.resetRenderTarget();
  }

  if (glfwWindow_ != nullptr) {
    glfwDestroyWindow(glfwWindow_);
    glfwWindow_ = nullptr;
  }

  glfwTerminate();
}

DUIDesktopGLFWHost* DUIDesktopGLFWHost::GetHost(GLFWwindow* window) {
  if (window == nullptr) {
    return nullptr;
  }

  return static_cast<DUIDesktopGLFWHost*>(
    glfwGetWindowUserPointer(window)
  );
}

// void DUIDesktopGLFWHost::KeyCallback(
//   GLFWwindow* window,
//   int key, int scancode,
//   int action, int mods) {
//
//
// }

void DUIDesktopGLFWHost::CharCallback(
  GLFWwindow* window, unsigned int codepoint) {

  auto* host = static_cast<DUIDesktopGLFWHost*>(
        glfwGetWindowUserPointer(window));

  if (host == nullptr)
    return;

  host->keyboard_.HandleChar(
    window,
    codepoint);
}

void DUIDesktopGLFWHost::CursorPositionCallback(
  GLFWwindow* window,
  double x,
  double y
) {
  auto* host = GetHost(window);
  if (host == nullptr) {
    return;
  }

  host->window_.OnMouseMove(
    static_cast<int>(x),
    static_cast<int>(y)
  );
}

void DUIDesktopGLFWHost::CursorEnterCallback(
  GLFWwindow* window,
  int entered
) {
  auto* host = GetHost(window);
  if (host == nullptr) {
    return;
  }

  if (entered == GLFW_TRUE) {
    host->window_.OnMouseIn();
  } else {
    host->window_.OnMouseOut();
  }
}

void DUIDesktopGLFWHost::MouseButtonCallback(
  GLFWwindow* window,
  int button,
  int action,
  int mods
) {
  (void)mods;

  auto* host = GetHost(window);
  if (host == nullptr) {
    return;
  }

  const MouseButton mouseButton = MapGLFWMouseButton(button);
  if (mouseButton == MouseButton::None) {
    return;
  }

  if (action == GLFW_PRESS) {
    host->window_.OnMouseDown(mouseButton);
  } else if (action == GLFW_RELEASE) {
    host->window_.OnMouseUp(mouseButton);
  }
}

void DUIDesktopGLFWHost::KeyCallback(
  GLFWwindow* window,
  int key,
  int scancode,
  int action,
  int mods
) {
  (void)scancode;
  (void)mods;

  auto* host = GetHost(window);
  if (host == nullptr) {
    return;
  }

  // if (action == GLFW_PRESS || action == GLFW_REPEAT) {
  //   host->window_.OnKeyDown(key);
  // } else if (action == GLFW_RELEASE) {
  //   host->window_.OnKeyUp(key);
  // }

  // auto* host = static_cast<DUIDesktopGLFWHost*>(
  //       glfwGetWindowUserPointer(window));


  host->keyboard_.HandleKey(
    window,
    key,
    scancode,
    action,
    mods);
}

void DUIDesktopGLFWHost::ScrollCallback(
  GLFWwindow* window,
  double xoffset,
  double yoffset
) {
  auto* host = GetHost(window);
  if (host == nullptr) {
    return;
  }

  host->window_.OnMouseWheel(
    static_cast<float>(xoffset),
    static_cast<float>(yoffset),
    false);
}

void DUIDesktopGLFWHost::FramebufferSizeCallback(
  GLFWwindow* window,
  int width,
  int height
) {
  (void)width;
  (void)height;

  auto* host = GetHost(window);
  if (host == nullptr) {
    return;
  }

  host->SyncWindowMetrics();
}
