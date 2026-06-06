#pragma once
#include <kromakit/Control.h>

class Label :
	public Control
{

	DUISize cachedSize = {0,0};

public:
	Label();
	Label(std::string str);

	bool disableAutoTextPlacement = false;

	bool autoHeight = true;
	bool autoWidth = true;

	void OnRender(Graphics* rendTarget) override;
	void DoLayout(Graphics* rendTarget) override;

	std::optional<std::string> GetControlName() const override;
	const std::vector<std::string> GetDebugOverlayItems() override;

	bool ShouldClipToBounds() override;

	[[deprecated("Use DUIStyleRegistry::Register(...) and Control::SetTyle(...)")]]
	void UseCodeStylePreset();
};

