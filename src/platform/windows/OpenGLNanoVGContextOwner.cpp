#include <kromakit/platform/windows/OpenGLNanoVGContextOwner.h>

#define GLFW_INCLUDE_NONE 1
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define NANOVG_GL3 1
#include <nanovg_gl.h>

OpenGLNanoVGContextOwner::~OpenGLNanoVGContextOwner()
{
  Reset();
}

bool OpenGLNanoVGContextOwner::Create(int nvgFlags)
{
  Reset();

  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    return false;
  }

  context_ = nvgCreateGL3(nvgFlags);
  return context_ != nullptr;
}

void OpenGLNanoVGContextOwner::Reset()
{
  if (context_ != nullptr) {
    nvgDeleteGL3(context_);
  }
  context_ = nullptr;
}
