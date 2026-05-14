//
// Created by code on 4/19/26.
//

#ifndef TEST_IOS_APP_SWIPEACTIONBUTTON_H
#define TEST_IOS_APP_SWIPEACTIONBUTTON_H
#include <kromakit/Button.h>
#include <kromakit/Control.h>
#include <kromakit/Label.h>


class SwipeActionButton :
    public Button {

public:
    SwipeActionButton();

    // Label _internalButtonLabel;

    void DoLayout(Graphics *renderTarget) override;
    void OnRender(Graphics *rendTarget) override;

    std::optional<std::string> GetControlName() const override { return "SwipeActionButton"; }
    const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

    bool WantsClickCapture() const override {return true;}

    void SetColor(const Color col) {
        background = col;
    }
};



#endif //TEST_IOS_APP_SWIPEACTIONBUTTON_H
