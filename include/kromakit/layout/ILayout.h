//
// Created by code on 4/19/26.
//

#ifndef TEST_IOS_APP_ILAYOUT_H
#define TEST_IOS_APP_ILAYOUT_H
#include <kromakit/layout/LayoutItem.h>
#include <kromakit/graphics/Graphics.h>


class ILayout {
public:
    virtual ~ILayout() = default;

    virtual DUISize Measure(
        const std::vector<LayoutItem>& children,
        const DUISize& available
    ) = 0;

    virtual void Arrange(
        std::vector<LayoutItem>& children,
        const DUIRect& rect
    ) = 0;
};


#endif //TEST_IOS_APP_ILAYOUT_H
