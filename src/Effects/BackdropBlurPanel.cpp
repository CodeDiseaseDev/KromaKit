#include <kromakit/Effects/BackdropBlurPanel.h>

#include <algorithm>

BackdropBlurPanel::BackdropBlurPanel()
{
	SetRenderLayer(RenderLayer::Overlay);
	background = ColorFromRGB(0, 0, 0, 0);
}

void BackdropBlurPanel::SetBlurOpacity(float blurOpacity)
{
	blurOpacity_ = std::clamp(blurOpacity, 0.0f, 1.0f);
}

void BackdropBlurPanel::SetTintColor(Color tintColor)
{
	tintColor_ = tintColor;
}

void BackdropBlurPanel::SetTintOpacity(float tintOpacity)
{
	tintOpacity_ = std::clamp(tintOpacity, 0.0f, 1.0f);
}

RenderLayer BackdropBlurPanel::GetRenderLayer() const
{
	return RenderLayer::Overlay;
}

bool BackdropBlurPanel::RequiresBackdropBlur() const
{
	return true;
}

void BackdropBlurPanel::OnRender(Graphics* rendTarget)
{
	RenderBlurLayer(rendTarget);
	RenderTintLayer(rendTarget);
}

void BackdropBlurPanel::DoLayout(Graphics* renderTarget)
{
	(void)renderTarget;
}

std::optional<std::string> BackdropBlurPanel::GetControlName() const
{
	return "BackdropBlurPanel";
}

const std::vector<std::string> BackdropBlurPanel::GetDebugOverlayItems()
{
	return {};
}

void BackdropBlurPanel::RenderBlurLayer(Graphics* rendTarget) const
{
	if (rendTarget == nullptr)
		return;

	const int blurredImage = rendTarget->GetBackdropBlurImage();
	if (blurredImage <= 0 || blurOpacity_ <= 0.0f)
		return;

	const DUISize textureSize = rendTarget->GetBackdropBlurTextureSize();
	if (textureSize.width <= 0 || textureSize.height <= 0)
		return;

	NVGpaint blurPaint = nvgImagePattern(
		rendTarget->_RenderTarget,
		-cachedScreenLocation.x,
		-cachedScreenLocation.y,
		textureSize.width,
		textureSize.height,
		0.0f,
		blurredImage,
		blurOpacity_);

	rendTarget->FillRoundedRectangle(
		0,
		0,
		size.width,
		size.height,
		blurPaint,
		cornerRadius);
}

void BackdropBlurPanel::RenderTintLayer(Graphics* rendTarget) const
{
	if (rendTarget == nullptr || tintOpacity_ <= 0.0f)
		return;

	Color tint = tintColor_;
	tint.a *= tintOpacity_;

	rendTarget->FillRoundedRectangle(
		0,
		0,
		size.width,
		size.height,
		tint,
		cornerRadius);
}
