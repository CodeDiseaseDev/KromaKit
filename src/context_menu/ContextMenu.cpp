//
// Created by code on 4/27/26.
//

#include <kromakit/context_menu/ContextMenu.h>
#include <kromakit/styling/DUIDefaultStyles.h>


ContextMenu::ContextMenu() {
  menuItemPanel = CreateControl<ScrollableVStackPanel>();
  UseStyle(DUIDefaultStyles::ContextMenuStyle::Parent::Default);

  menuItemPanel->Stack().UseStyle(DUIDefaultStyles::SurfaceBaseStyles::NeutralSurfaces::Clear);
  menuItemPanel->UseStyle(DUIDefaultStyles::SurfaceBaseStyles::NeutralSurfaces::Clear);
}

void ContextMenu::DoLayout(Graphics* renderTarget) {
  (void)DoLayout2(renderTarget);
}

LayoutResult ContextMenu::DoLayout2(Graphics* renderTarget) {
  LayoutResult result;
  result.geometryChanged |= menuItemPanel->SetLayoutFrame({0, 0}, size);

  // const float horizontalInset = 0/*TODO*/ * 2.0f;
  const float itemWidth = menuItemPanel->size.width;

  for (auto i = menuItemPanel->Stack().Children.begin();
    i < menuItemPanel->Stack().Children.end(); ++i) {

    auto& item = *i;
    // if (i == menuItemPanel.Stack().Children.begin()) {
    //   item->margin.top = 0;
    // }
    // if (i == menuItemPanel.Stack().Children.end() - 1) {
    //   item->margin.bottom = 0;
    // }
    //

    result.geometryChanged |= item->SetLayoutSize({
      itemWidth - item->margin.Horizontal(),
      item->size.height
    });
  }

  return result;
}

void ContextMenu::OnRender(Graphics *rendTarget) {
  rendTarget->FillControlBackground(this);
}

void ContextMenu::DismissMenu() {
  DismissBackgroundLayer();
}

void ContextMenu::OnItemAdded(IContextMenuItem *item) {
  if (item == nullptr) {
    return;
  }
  item->RequestDismiss = [&] { DismissMenu(); };
}
