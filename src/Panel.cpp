#include <kromakit/Panel.h>
#include <kromakit/styling/controls/PanelStyle.h>

Panel::Panel()
{
	UseStyle(DUIDefaultStyles::PanelStyles::Default);
}

void Panel::OnRender(Graphics* rendTarget)
{
	rendTarget->FillControlBackground(this);

	// rendTarget->FillRoundedRectangle(
	// 	0, 0, size.width, size.height,
	// 	background, 6
	// );

	// rendTarget->DrawRoundedRectangle(
	// 	0, 0, size.width, size.height,
	// 	foreground, 6
	// );
}

void Panel::DoLayout(Graphics* renderTarget)
{

}

void Panel::OnMouseDown()
{

}

std::optional<std::string> Panel::GetControlName() const {
	return "Panel";
}

const std::vector<std::string> Panel::GetDebugOverlayItems() {
	return {};
}
