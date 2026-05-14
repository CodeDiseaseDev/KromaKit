#include <kromakit/Button.h>
#include <kromakit/styling/controls/ButtonStyle.h>
#include <kromakit/HapticsBridge.h>

// #include "Application.h"
// #include "GaussianBlurProvider.h"

void Button::OnRender(Graphics* rendTarget)
{
	Color rectColor = background;

	if (isMouseHovering)
		rectColor = rendTarget->ColorBrightnessOffset(
			rectColor, -0.05);

	if (isMouseDown)
		rectColor = rendTarget->ColorBrightnessOffset(
			rectColor, -0.05);

	rendTarget->FillControlBackground(this);

	rectColor = rendTarget->ColorBrightnessOffset(
		rectColor, +0.2);

	// rendTarget->DrawRoundedRectangle(
	// 	0, 0, size.width, size.height,
	// 	rectColor, 4
	// );

	rendTarget->PushRectangleClip(size.width, size.height);

	rendTarget->RenderTextCentered(
		sContent.c_str(), font,
		foreground, this
	);

	rendTarget->PopRectangleClip();
}

bool Button::WantsClickCapture() const {
	return true;
}

void Button::DoLayout(Graphics* renderTarget)
{
	// if (usesFrostedBackground)
	// 	bg_blur_provider->SetCornerRadius(5);
}

void Button::OnMouseDown()
{
}

void Button::OnMouseUp()
{
	DirectUIHaptics::Impact(2);

	if (OnClick != nullptr)
	{
		OnClick();
	}

}

void Button::OnMouseIn()
{
}

void Button::OnMouseOut()
{
}

void Button::OnIsSelectedChanged()
{

}

std::optional<std::string> Button::GetControlName() const {
	return "Button";
}

const std::vector<std::string> Button::GetDebugOverlayItems() {
	return {};
}

Button::Button()
{
	UseStyle(DUIDefaultStyles::ButtonStyles::Default);
}

Button::Button(std::string content) : Button() {
	SetContent(content);
}
