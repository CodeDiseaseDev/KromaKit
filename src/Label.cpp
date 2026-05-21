#include <kromakit/Label.h>
#include <kromakit/styling/controls/LabelStyle.h>

constexpr bool DISABLE_CLIPPING = true;

Label::Label() {
	UseStyle(DUIDefaultStyles::LabelStyles::Default);
}

Label::Label(std::string str) {
	SetContent(str);
}

void Label::DoLayout(Graphics* rendTarget)
{
	cachedSize = rendTarget->CalculateTextSize(
		sContent.c_str(), font);

	if (autoHeight)
	{
		this->size.height = cachedSize.height;
	}

	if (autoWidth)
	{
		this->size.width = cachedSize.width;
	}
}

std::optional<std::string> Label::GetControlName() const {
	return "Label";
}

const std::vector<std::string> Label::GetDebugOverlayItems() {
	return {};
}

bool Label::ShouldClipToBounds() {
	return !DISABLE_CLIPPING &&
		!(backgroundPadding.left > 0 ||
		  backgroundPadding.top > 0 ||
		  backgroundPadding.right > 0 ||
		  backgroundPadding.bottom > 0);
}

void Label::UseCodeStylePreset() {
	UseStyle(DUIDefaultStyles::LabelStyles::BadgeCode);
}

void Label::OnRender(Graphics* rendTarget)
{
	renderOffset.x = backgroundPadding.left;
	renderOffset.y = backgroundPadding.top;

	rendTarget->FillControlBackground(this);


	(void)rendTarget->RenderText(
		sContent.c_str(), font, foreground,
		0,
		this->size.height / 2 - cachedSize.height / 2
	);

}
