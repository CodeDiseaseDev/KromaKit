#pragma once

#include <nanovg.h>

namespace DirectUIRenderBackend {

struct Framebuffer;

bool SupportsBackdropBlur();

void BindDefaultFramebuffer();
void BindFramebuffer(Framebuffer* framebuffer);

Framebuffer* CreateFramebuffer(
	NVGcontext* context,
	int width,
	int height,
	int imageFlags
);

void DeleteFramebuffer(Framebuffer* framebuffer);

int GetFramebufferImage(const Framebuffer* framebuffer);

void Clear(NVGcontext* context, NVGcolor color);

bool ApplyGaussianBlur(
	NVGcontext* context,
	int sourceImage,
	int destinationImage,
	float standardDeviation
);

} // namespace DirectUIRenderBackend
