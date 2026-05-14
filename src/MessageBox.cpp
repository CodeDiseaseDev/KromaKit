// #include <kromakit/MessageBox.h>
//
// #include <kromakit/Logging.h>
// #include <kromakit/Utils.h>
// #include <kromakit/Application.h>
//
// MessageBoxCtrl::MessageBoxCtrl()
// {
// 	this->isVisible = false;
//
// 	background = D2D1::ColorF(0x333333, 1);
// 	foreground = D2D1::ColorF(0x444444, 1);
//
// 	content = CreateControl<Label>();
// 	title = CreateControl<Label>();
// 	okBtn = CreateControl<Button>();
// 	cancelBtn = CreateControl<Button>();
//
// 	title->font.FontSize = 20;
// 	okBtn->sContent = L"Ok";
// 	cancelBtn->sContent = L"Cancel";
//
// 	okBtn->OnClick = [&]() {
// 		if (OnResult != nullptr) OnResult(MSGBOX_TRUE);
// 	};
//
// 	cancelBtn->OnClick = [&]() {
// 		if (OnResult != nullptr) OnResult(MSGBOX_FALSE);
// 	};
// }
//
// void MessageBoxCtrl::Show(
// 	const wchar_t* content,
// 	const wchar_t* title)
// {
// 	this->content->autoHeight = true;
// 	this->title->autoHeight = true;
//
// 	this->content->sContent = content;
// 	this->title->sContent = title;
// 	this->isVisible = true;
//
// 	//RerenderWindow();
// }
//
// void MessageBoxCtrl::OnRender(Graphics* rendTarget)
// {
// 	DoLayout(rendTarget);
//
// 	rendTarget->FillOutlinedRoundedRectangle(
// 		0, 0, size.width, size.height,
// 		background, foreground, 6
// 	);
// }
//
// void MessageBoxCtrl::DoLayout(Graphics* rendTarget)
// {
// 	cancelBtn->size = okBtn->size = { 60, 25 };
//
// 	title->location = { Padding, Padding };
// 	content->location = { Padding, title->location.y + title->size.height + Padding };
//
// 	this->size.height = content->location.y + content->size.height + okBtn->size.height + (Padding * 3);
//
// 	this->size.width = (Padding * 2) + max(
// 		content->size.height,
// 		title->size.height);
//
// 	this->size.width = max(this->size.width, 300);
//
// 	okBtn->location = {
// 		size.width - Padding - okBtn->size.width,
// 		size.height - okBtn->size.height - Padding
// 	};
//
// 	cancelBtn->location = {
// 		okBtn->location.x - Padding - okBtn->size.width,
// 		okBtn->location.y
// 	};
//
// 	this->location = {
// 		parent->size.width / 2 - size.width / 2,
// 		parent->size.height / 2 - size.height / 2
// 	};
//
// 	if (size.height < 150)
// 	{
// 		// printf("hi\n");
// 	}
//
// 	// printf("%.2f, %.2f\n", size.width, size.height);
// }
//
// void MessageBoxCtrl::OnUsed()
// {
// 	Logging::Log("OnUsed()\n");
// 	DoLayout(GetRootWindow()->graphics);
// }
