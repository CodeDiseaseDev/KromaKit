#include <kromakit/platform/linux/OpenGLNanoVGContextOwner.h>

#define GL_GLEXT_PROTOTYPES 1
#include <GLFW/glfw3.h>

#define NANOVG_GL3 1
#include <nanovg_gl.h>

OpenGLNanoVGContextOwner::~OpenGLNanoVGContextOwner() {
  Reset();
}

bool OpenGLNanoVGContextOwner::Create(int nvgFlags) {
  Reset();
  context_ = nvgCreateGL3(nvgFlags);
  return context_ != nullptr;
}

void OpenGLNanoVGContextOwner::Reset() {
  if (context_ == nullptr) {
    return;
  }

  nvgDeleteGL3(context_);
  context_ = nullptr;
}
