//
// Created by code on 4/19/26.
//

#include <kromakit/layout/FlexColumnLayout.h>

DUISize FlexColumnLayout::Measure(
    const std::vector<LayoutItem>& children,
    const DUISize& available) {
    (void)children;
    (void)available;

    return {0.0f, 0.0f};
}

void FlexColumnLayout::Arrange(
    std::vector<LayoutItem>& children,
    const DUIRect& rect) {
    (void)children;
    (void)rect;
}
