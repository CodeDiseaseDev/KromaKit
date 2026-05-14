#include <kromakit/Backend/BlurPass.h>

#include <kromakit/Rendering/RenderBackendHooks.h>

#include <algorithm>

void BlurPass::SetStandardDeviation(float standardDeviation)
{
	standardDeviation_ = std::max(0.0f, standardDeviation);
}

float BlurPass::GetStandardDeviation() const
{
	return standardDeviation_;
}

bool BlurPass::Apply(
	NVGcontext* context,
	int sourceImage,
	int destinationImage) const
{
	if (context == nullptr || sourceImage <= 0 || destinationImage <= 0)
		return false;

	if (standardDeviation_ <= 0.0f)
		return false;

	return DirectUIRenderBackend::ApplyGaussianBlur(
		context,
		sourceImage,
		destinationImage,
		standardDeviation_);
}
