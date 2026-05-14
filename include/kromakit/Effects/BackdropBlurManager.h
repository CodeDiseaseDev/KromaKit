#pragma once

#include <kromakit/Backend/BlurPass.h>
#include <kromakit/Rendering/RenderBackendHooks.h>

class BackdropBlurManager {
public:
	BackdropBlurManager() = default;
	~BackdropBlurManager();

	void SetBlurStandardDeviation(float standardDeviation);

	void EnsureResources(
		NVGcontext* context,
		int pixelWidth,
		int pixelHeight
	);

	void BeginContentCapture();
	void EndContentCapture();

	bool ExecuteBlurPass() const;

	int GetBlurredImage() const;

	void Release();

private:
	void RecreateFramebuffers();
	void DestroyFramebuffers();

	NVGcontext* context_ = nullptr;
	DirectUIRenderBackend::Framebuffer* contentFramebuffer_ = nullptr;
	DirectUIRenderBackend::Framebuffer* blurredFramebuffer_ = nullptr;
	int pixelWidth_ = 0;
	int pixelHeight_ = 0;
	BlurPass blurPass_;
};
