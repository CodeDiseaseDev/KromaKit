//
// Created by code on 4/28/26.
//

#ifndef TEST_IOS_APP_CONTEXTMENUSUBMENUITEM_H
#define TEST_IOS_APP_CONTEXTMENUSUBMENUITEM_H

#include <kromakit/context_menu/IContextMenuItem.h>

class ContextMenuSubMenuItem :
  public IContextMenuItem {

public:
  ContextMenuSubMenuItem();
  ContextMenuSubMenuItem(std::string str);


  void DoLayout(Graphics *renderTarget) override;
  void OnRender(Graphics *rendTarget) override;

  std::optional<std::string> GetControlName() const override { return "ContextMenuSubMenuItem"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }
};



#endif //TEST_IOS_APP_CONTEXTMENUSUBMENUITEM_H
