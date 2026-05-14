//
// Created by code on 4/27/26.
//

#ifndef TEST_IOS_APP_CONTEXTMENUTEXTITEM_H
#define TEST_IOS_APP_CONTEXTMENUTEXTITEM_H

#include <kromakit/context_menu/IContextMenuItem.h>
#include <functional>
#include <string>


class ContextMenuTextItem :
  public IContextMenuItem
{
public:
  ContextMenuTextItem(std::string str);
  ContextMenuTextItem();



  // void OnPointerUp(const PointerEvent &e) override;

  void DoLayout(Graphics *renderTarget) override;
  void OnRender(Graphics *rendTarget) override;

  std::optional<std::string> GetControlName() const override { return "ContextMenuTextItem"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }


};

#endif // TEST_IOS_APP_CONTEXTMENUTEXTITEM_H
