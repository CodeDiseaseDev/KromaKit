//
// Created by code on 4/28/26.
//

#include <kromakit/context_menu/item_types/ContextMenuSubMenuItem.h>
#include <kromakit/styling/DUIDefaultStyles.h>

ContextMenuSubMenuItem::ContextMenuSubMenuItem() :
    IContextMenuItem() {

  label = CreateControl<Label>(); // defined in IContextMenuItem
  UseStyle(DUIDefaultStyles::ContextMenuStyle::Child::Item);
}

ContextMenuSubMenuItem::ContextMenuSubMenuItem(std::string str) :
  ContextMenuSubMenuItem() {

  SetLabelText(str);
}

void ContextMenuSubMenuItem::DoLayout(Graphics *renderTarget) {
  IContextMenuItem::DoLayout(renderTarget);
}

void ContextMenuSubMenuItem::OnRender(Graphics *rendTarget) {
  rendTarget->FillControlBackground(this);

  rendTarget->RenderText(">", font, foreground, size.width - 30, size.height / 2 - 10);
}
