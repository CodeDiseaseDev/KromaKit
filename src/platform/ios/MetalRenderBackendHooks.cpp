#include <kromakit/Rendering/RenderBackendHooks.h>

#include <nanovg_mtl.h>

namespace DirectUIRenderBackend {

struct Framebuffer {
	MNVGframebuffer* handle = nullptr;
};

bool SupportsBackdropBlur()
{
	return true;
}

void BindDefaultFramebuffer()
{
	mnvgBindFramebuffer(nullptr);
}

void BindFramebuffer(Framebuffer* framebuffer)
{
	mnvgBindFramebuffer(framebuffer ? framebuffer->handle : nullptr);
}

Framebuffer* CreateFramebuffer(
	NVGcontext* context,
	int width,
	int height,
	int imageFlags)
{
	if (context == nullptr || width <= 0 || height <= 0)
		return nullptr;

	MNVGframebuffer* framebuffer =
		mnvgCreateFramebuffer(context, width, height, imageFlags);
	if (framebuffer == nullptr)
		return nullptr;

	Framebuffer* wrapped = new Framebuffer();
	wrapped->handle = framebuffer;
	return wrapped;
}

void DeleteFramebuffer(Framebuffer* framebuffer)
{
	if (framebuffer == nullptr)
		return;

	if (framebuffer->handle != nullptr) {
		mnvgDeleteFramebuffer(framebuffer->handle);
		framebuffer->handle = nullptr;
	}

	delete framebuffer;
}

int GetFramebufferImage(const Framebuffer* framebuffer)
{
	if (framebuffer == nullptr || framebuffer->handle == nullptr)
		return 0;

	return framebuffer->handle->image;
}

void Clear(NVGcontext* context, NVGcolor color)
{
	if (context == nullptr)
		return;

	mnvgClearWithColor(context, color);
}

bool ApplyGaussianBlur(
	NVGcontext* context,
	int sourceImage,
	int destinationImage,
	float standardDeviation)
{
	if (context == nullptr)
		return false;

	return mnvgApplyGaussianBlur(
		context,
		sourceImage,
		destinationImage,
		standardDeviation) == 1;
}

} // namespace DirectUIRenderBackend
