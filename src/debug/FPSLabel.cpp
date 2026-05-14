//
// Created by code on 4/20/26.
//

#include <kromakit/debug/FPSLabel.h>
#include <kromakit/styling/controls/FPSLabelStyle.h>

FPSLabel::FPSLabel() {
    UseStyle(DUIDefaultStyles::FPSLabelStyles::Default);
}

void FPSLabel::DoLayout(Graphics *renderTarget) {

}

void FPSLabel::OnRender(Graphics *rendTarget) {
    rendTarget->RenderTextCentered(
        text.c_str(), font, foreground, this);

    avg_p += (int)rendTarget->GetFPS();
    avg_c++;

    if (time > updateInterval) {
        text = std::to_string(avg_p / avg_c) + " FPS";
        time -= updateInterval;

        avg_c = 0;
        avg_p = 0;
    }
    time += rendTarget->DeltaTime;
}
