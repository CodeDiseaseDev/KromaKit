#include <kromakit/Effects/BackdropBlurManager.h>

BackdropBlurManager::~BackdropBlurManager()
{
	Release();
}

void BackdropBlurManager::SetBlurStandardDeviation(float standardDeviation)
{
	blurPass_.SetStandardDeviation(standardDeviation);
}

void BackdropBlurManager::EnsureResources(
	NVGcontext* context,
	int pixelWidth,
	int pixelHeight)
{
	if (context == nullptr || pixelWidth <= 0 || pixelHeight <= 0) {
		Release();
		return;
	}

	const bool contextChanged = context_ != context;
	const bool sizeChanged = pixelWidth_ != pixelWidth || pixelHeight_ != pixelHeight;
	if (!contextChanged && !sizeChanged && contentFramebuffer_ != nullptr &&
		blurredFramebuffer_ != nullptr) {
		return;
	}

	context_ = context;
	pixelWidth_ = pixelWidth;
	pixelHeight_ = pixelHeight;
	RecreateFramebuffers();
}

void BackdropBlurManager::BeginContentCapture()
{
	DirectUIRenderBackend::BindFramebuffer(contentFramebuffer_);
}

void BackdropBlurManager::EndContentCapture()
{
	DirectUIRenderBackend::BindDefaultFramebuffer();
}

bool BackdropBlurManager::ExecuteBlurPass() const
{
	if (context_ == nullptr || contentFramebuffer_ == nullptr ||
		blurredFramebuffer_ == nullptr) {
		return false;
	}

	return blurPass_.Apply(
		context_,
		DirectUIRenderBackend::GetFramebufferImage(contentFramebuffer_),
		DirectUIRenderBackend::GetFramebufferImage(blurredFramebuffer_));
}

int BackdropBlurManager::GetBlurredImage() const
{
	return DirectUIRenderBackend::GetFramebufferImage(blurredFramebuffer_);
}

void BackdropBlurManager::Release()
{
	DestroyFramebuffers();
	context_ = nullptr;
	pixelWidth_ = 0;
	pixelHeight_ = 0;
}

void BackdropBlurManager::RecreateFramebuffers()
{
	DestroyFramebuffers();

	if (context_ == nullptr || pixelWidth_ <= 0 || pixelHeight_ <= 0)
		return;

	contentFramebuffer_ = DirectUIRenderBackend::CreateFramebuffer(
		context_, pixelWidth_, pixelHeight_, NVG_IMAGE_PREMULTIPLIED);
	blurredFramebuffer_ = DirectUIRenderBackend::CreateFramebuffer(
		context_, pixelWidth_, pixelHeight_, NVG_IMAGE_PREMULTIPLIED);

	if (contentFramebuffer_ == nullptr || blurredFramebuffer_ == nullptr) {
		DestroyFramebuffers();
	}
}

void BackdropBlurManager::DestroyFramebuffers()
{
	if (contentFramebuffer_ != nullptr) {
		DirectUIRenderBackend::DeleteFramebuffer(contentFramebuffer_);
		contentFramebuffer_ = nullptr;
	}

	if (blurredFramebuffer_ != nullptr) {
		DirectUIRenderBackend::DeleteFramebuffer(blurredFramebuffer_);
		blurredFramebuffer_ = nullptr;
	}
}
