//
// Created by code on 4/27/26.
//

#include <kromakit/context_menu/item_types/ContextMenuTextItem.h>

#include <utility>
#include <kromakit/styling/DUIDefaultStyles.h>


ContextMenuTextItem::ContextMenuTextItem() :
  IContextMenuItem() {

  label = CreateControl<Label>();


  UseStyle(DUIDefaultStyles::ContextMenuStyle::Child::Item);
}

ContextMenuTextItem::ContextMenuTextItem(std::string str) :
  ContextMenuTextItem() {
  SetLabelText(str);
}



void ContextMenuTextItem::DoLayout(Graphics *renderTarget) {
  IContextMenuItem::DoLayout(renderTarget);
}

void ContextMenuTextItem::OnRender(Graphics *rendTarget) {

  rendTarget->FillControlBackground(this);


}
