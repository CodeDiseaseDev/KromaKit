//
// Created by code on 4/19/26.
//

#ifndef TEST_IOS_APP_LAYOUTITEM_H
#define TEST_IOS_APP_LAYOUTITEM_H

#include <kromakit/Control.h>
#include <kromakit/layout/layout_options/LayoutAlignment.h>
#include <kromakit/layout/layout_options/LayoutOptions.h>

struct LayoutItem {
    Control* control = nullptr;

    float fixedSize = -1.0f;     // Explicit size along layout axis. -1 = auto.
    float grow = 0.0f;           // Weight for remaining space.
    float minSize = 0.0f;
    float maxSize = -1.0f;       // -1 = no max.

    DUIInsets margin{0, 0, 0, 0};
    // Per-item layout inset reservation around the arranged control bounds.
    // StackLayout treats this as extra outer spacing (like margin), not as
    // internal content padding inside the child control.
    DUIInsets padding{0, 0, 0, 0};

    LayoutOptions options;
};




#endif //TEST_IOS_APP_LAYOUTITEM_H
