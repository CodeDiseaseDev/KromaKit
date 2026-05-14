//
// Created by code on 4/20/26.
//

#ifndef TEST_IOS_APP_FPSLABEL_H
#define TEST_IOS_APP_FPSLABEL_H
#include <kromakit/Control.h>


class FPSLabel :
    public Control {

    std::string text;

    float updateInterval = 1.0f; // 1 second
    float time = 0.0f;

    int avg_p = 0.0f;
    int avg_c = 0;

public:
    FPSLabel();


    void DoLayout(Graphics *renderTarget) override;
    void OnRender(Graphics *rendTarget) override;

    std::optional<std::string> GetControlName() const override {
        return "FPSLabel";
    }
    const std::vector<std::string> GetDebugOverlayItems() override {
        return {};
    }
};



#endif //TEST_IOS_APP_FPSLABEL_H
