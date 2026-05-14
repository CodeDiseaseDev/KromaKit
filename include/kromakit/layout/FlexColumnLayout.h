//
// Created by code on 4/19/26.
//

#ifndef TEST_IOS_APP_FLEXCOLUMNLAYOUT_H
#define TEST_IOS_APP_FLEXCOLUMNLAYOUT_H
#include <kromakit/layout/ILayout.h>
#include <kromakit/graphics/Graphics.h>

class FlexColumnLayout :
    public ILayout {
public:

    DUISize Measure(
        const std::vector<LayoutItem>& children,
        const DUISize& available
    ) override;

    void Arrange(
        std::vector<LayoutItem>& children,
        const DUIRect& rect
    ) override;
};



#endif //TEST_IOS_APP_FLEXCOLUMNLAYOUT_H
