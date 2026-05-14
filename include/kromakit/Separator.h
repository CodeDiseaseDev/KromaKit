//
// Created by code on 4/21/26.
//

#ifndef TEST_IOS_APP_SEPARATOR_H
#define TEST_IOS_APP_SEPARATOR_H
#include <kromakit/Control.h>


class Separator :
    public virtual Control {

public:
    Separator();

    float XSpacing = 0.0f;

    void OnRender(Graphics *rendTarget) override;
    void DoLayout(Graphics *renderTarget) override;

    std::optional<std::string> GetControlName() const override {return "Separator";}
    const std::vector<std::string> GetDebugOverlayItems() override {return{};}
};



#endif //TEST_IOS_APP_SEPARATOR_H
