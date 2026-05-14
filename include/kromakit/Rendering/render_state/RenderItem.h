//
// Created by code on 4/20/26.
//

#ifndef TEST_IOS_APP_RENDERITEM_H
#define TEST_IOS_APP_RENDERITEM_H
#include <kromakit/Control.h>

struct RenderItem {
    Control* ctrl{};
    // Layout-space rect. This intentionally excludes renderOffset and is
    // stable for effects that need background-aligned sampling.
    DUIRect screenRect{};
    // Visual-space rect after applying renderOffset hierarchy.
    DUIRect visualRect{};
    bool opaque{};
    int z{};
    bool visible = true;
    DUIPoint renderOffset{};
    DUIRect clipRect{};
    float opacity{};
    RenderLayer layer{RenderLayer::Content};
    // Preorder traversal range for O(1) ancestor/descendant checks.
    uint32_t treeStart = 0;
    uint32_t treeEnd = 0;
    float inheritedCentredScale = 1.0f;
    DUIPoint inheritedCentredScaleOrigin{0, 0};
};

#endif //TEST_IOS_APP_RENDERITEM_H
