#pragma once
#include <kromakit/Control.h>

class TestControl :
    public Control
{
    void OnRender(Graphics* rendTarget) override;
    void DoLayout(Graphics* renderTarget) override;

    void OnMouseDown() override;
};

