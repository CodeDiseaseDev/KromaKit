//
// Created by code on 4/19/26.
//

#include <kromakit/gesture/SwipeActionButton.h>
#include <kromakit/styling/controls/SwipeActionButtonStyle.h>

SwipeActionButton::SwipeActionButton() {
    // AddControl(&_internalButtonLabel);
    UseStyle(DUIDefaultStyles::SwipeActionButtonStyles::Default);
}

void SwipeActionButton::DoLayout(Graphics *renderTarget) {
    // _internalButtonLabel.location = {
    //     size.width / 2 - _internalButtonLabel.size.width / 2,
    //     size.height / 2 - _internalButtonLabel.size.height / 2
    // };
    // _internalButtonLabel.font = font;
    // _internalButtonLabel.foreground = foreground;
    // _internalButtonLabel.sContent = sContent;
}

void SwipeActionButton::OnRender(Graphics *rendTarget) {
    rendTarget->FillControlBackground(this);

    nvgIntersectScissor(rendTarget->_RenderTarget,
        0, 0, size.width, size.height);

    rendTarget->RenderTextCentered(
        sContent.c_str(), font, foreground, this
    );
}
