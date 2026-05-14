//
// Created by code on 4/21/26.
//

#include <kromakit/Separator.h>
#include <kromakit/styling/controls/SeparatorStyle.h>

Separator::Separator() {
    UseStyle(DUIDefaultStyles::SeparatorStyles::Default);
}

void Separator::OnRender(
    Graphics *rendTarget) {

    // rendTarget->FillControlBackground(this);
    rendTarget->FillRectangle(
        XSpacing, 0, size.width - XSpacing * 2, size.height, outline);
}

void Separator::DoLayout(
    Graphics *renderTarget) {}
