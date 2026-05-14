//
// Created by code on 4/27/26.
//

#pragma once

#include <kromakit/styling/DUIStyleRegistry.h>
#include <kromakit/styling/DUIStyleName.h>
#include <kromakit/graphics/Graphics.h>

#include <kromakit/context_menu/IContextMenuItem.h>
#include <kromakit/context_menu/ContextMenu.h>
#include <kromakit/styling/DUIDefaultColors.h>
#include <kromakit/context_menu/item_types/ContextMenuSeparator.h>
#include <kromakit/styling/base_styles/BorderBaseStyles.h>
#include <kromakit/styling/base_styles/ForegroundBaseStyles.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>
#include <kromakit/styling/controls/SeparatorStyle.h>

namespace DUIDefaultStyles::ContextMenuStyle {
  namespace Parent {
    inline const DUIStyleName Default = "contextmenustyle.parent.default";
  }

  namespace Child {
    inline const DUIStyleName Default = "contextmenustyle.child.default";
    inline const DUIStyleName Item = "contextmenustyle.child.item";
    inline const DUIStyleName Separator = "contextmenustyle.child.separator";
  }

  inline void Register() {
    // constexpr float OpticalBias = 1.0f;

    // constexpr float Spacing = 0;
    constexpr float ParentCornerRadius = 15;
    constexpr float Padding = 15;

    // constexpr float ChildCornerRadius =
        // ParentCornerRadius - (Spacing * 0.5f) - OpticalBias;


    /*      Context menu container                                             */
    DUIStyleRegistry::Register<ContextMenu>(Parent::Default, [](ContextMenu& b) {
      b.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Modal);
      b.UseStyle(ForegroundBaseStyles::Primary);
      b.UseStyle(BorderBaseStyles::Strong);
      b.SetRenderLayer(RenderLayer::Overlay);

      b.cornerRadius = DUIInsets(ParentCornerRadius);
      b.menuItemPanel->padding = DUIInsets(0, 0);

      b.size = { 200, 200 };
      // b.menuItemPanel.spacing = 0;
    });


    /*      Context menu item                                                           */
    DUIStyleRegistry::Register<IContextMenuItem>(Child::Default, [](IContextMenuItem& b) {
      b.UseStyle(SurfaceBaseStyles::NeutralSurfaces::Clear);
      b.UseStyle(ForegroundBaseStyles::Primary);
      b.cornerRadius = DUIInsets(0);

      b.margin = DUIInsets(0);
    });

    DUIStyleRegistry::Register<IContextMenuItem>(Child::Item, [](IContextMenuItem& b) {
      b.UseStyle(Child::Default);

      // set tap color and make background invisible by default
      b.tapColor = Colors::SurfaceA20;
      b.background = Alpha(b.tapColor, 0);



      b.size.height = Padding * 2 + 18;
      b.LabelXSpacing = Padding;
      b.margin = DUIInsets(0);
      b.cornerRadius = DUIInsets(ParentCornerRadius); // rounded sides
      b.label->SetFont({ "Ubuntu", 18, FontWeight::Regular });
    });

    DUIStyleRegistry::Register<ContextMenuSeparator>(Child::Separator, [](ContextMenuSeparator& b) {
      b.UseStyle(Child::Default);

      b.UseStyle(SeparatorStyles::Default);
      b.XSpacing = 2;
    });
  }
}
