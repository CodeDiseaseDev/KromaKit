//
// Created by code on 4/28/26.
//

#ifndef TEST_IOS_APP_CONTEXTMENUSEPARATOR_H
#define TEST_IOS_APP_CONTEXTMENUSEPARATOR_H
#include <kromakit/context_menu/IContextMenuItem.h>
#include <kromakit/Separator.h>

class ContextMenuSeparator :
  public IContextMenuItem,
  public Separator {

public:
  ContextMenuSeparator();

  std::optional<std::string> GetControlName() const override;

  const std::vector<std::string> GetDebugOverlayItems() override;

  void DoLayout(Graphics *renderTarget) override;

  void OnRender(Graphics *rendTarget) override;
};

#endif //TEST_IOS_APP_CONTEXTMENUSEPARATOR_H
