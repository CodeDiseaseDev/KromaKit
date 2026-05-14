#pragma once

#include <kromakit/Control.h>

class BackdropBlurPanel : public Control {
public:
	BackdropBlurPanel();

	void SetBlurOpacity(float blurOpacity);
	void SetTintColor(Color tintColor);
	void SetTintOpacity(float tintOpacity);

	RenderLayer GetRenderLayer() const override;
	bool RequiresBackdropBlur() const override;

	void OnRender(Graphics* rendTarget) override;
	void DoLayout(Graphics* renderTarget) override;

	std::optional<std::string> GetControlName() const override;
	const std::vector<std::string> GetDebugOverlayItems() override;

private:
	void RenderBlurLayer(Graphics* rendTarget) const;
	void RenderTintLayer(Graphics* rendTarget) const;

	float blurOpacity_ = 1.0f;
	Color tintColor_ = ColorFromRGB(255, 255, 255, 255);
	float tintOpacity_ = 0.12f;
};
