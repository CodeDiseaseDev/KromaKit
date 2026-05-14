#include <kromakit/Rendering/RenderBackendHooks.h>

#define GL_GLEXT_PROTOTYPES 1
#include <GLFW/glfw3.h>

namespace DirectUIRenderBackend {

struct Framebuffer {};

bool SupportsBackdropBlur()
{
  return false;
}

void BindDefaultFramebuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BindFramebuffer(Framebuffer* framebuffer)
{
  (void)framebuffer;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer* CreateFramebuffer(
  NVGcontext* context,
  int width,
  int height,
  int imageFlags)
{
  (void)context;
  (void)width;
  (void)height;
  (void)imageFlags;
  return nullptr;
}

void DeleteFramebuffer(Framebuffer* framebuffer)
{
  (void)framebuffer;
}

int GetFramebufferImage(const Framebuffer* framebuffer)
{
  (void)framebuffer;
  return 0;
}

void Clear(NVGcontext* context, NVGcolor color)
{
  (void)context;
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool ApplyGaussianBlur(
  NVGcontext* context,
  int sourceImage,
  int destinationImage,
  float standardDeviation)
{
  (void)context;
  (void)sourceImage;
  (void)destinationImage;
  (void)standardDeviation;
  return false;
}

} // namespace DirectUIRenderBackend
