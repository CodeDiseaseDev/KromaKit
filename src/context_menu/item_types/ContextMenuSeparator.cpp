//
// Created by code on 4/28/26.
//

#include <kromakit/context_menu/item_types/ContextMenuSeparator.h>
#include <kromakit/styling/DUIDefaultStyles.h>

ContextMenuSeparator::ContextMenuSeparator() {
  // UseStyle(DUIDefaultStyles::ContextMenuStyle::Parent::Default);
  UseStyle(DUIDefaultStyles::ContextMenuStyle::Child::Separator);
}

std::optional<std::string> ContextMenuSeparator::GetControlName() const { return "IContextMenuItem"; }

const std::vector<std::string> ContextMenuSeparator::GetDebugOverlayItems() { return {}; }

void ContextMenuSeparator::DoLayout(Graphics *renderTarget) {
  Separator::DoLayout(renderTarget);
}

void ContextMenuSeparator::OnRender(Graphics *rendTarget) {
  Separator::OnRender(rendTarget);
}
