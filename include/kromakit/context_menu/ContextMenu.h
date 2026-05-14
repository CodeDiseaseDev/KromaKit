//
// Created by code on 4/27/26.
//

#ifndef TEST_IOS_APP_CONTEXTMENU_H
#define TEST_IOS_APP_CONTEXTMENU_H
#include <kromakit/context_menu/IContextMenuItem.h>
#include <kromakit/Control.h>
#include <kromakit/stack_panels/VStackPanel.h>
#include <type_traits>

#include <kromakit/overlay_background_layer/IOverlayControl.h>
#include <kromakit/scrollable_stack_panels/ScrollableVStackPanel.h>

class ContextMenu :
  public IOverlayControl {

public:
  ContextMenu();

  ScrollableVStackPanel* menuItemPanel = nullptr;

  void DoLayout(Graphics *renderTarget) override;
  LayoutResult DoLayout2(Graphics *renderTarget) override;
  bool NeedsLayoutWhenOwnSizeChanges() const override { return true; }
  void OnRender(Graphics *rendTarget) override;

  std::optional<std::string> GetControlName() const override { return "ContextMenu"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }

  bool ShouldClipToBounds() override { return false; }

  void DismissMenu();


  template<typename T, typename... Args>
  requires std::is_base_of_v<IContextMenuItem, T>
  T* AddItem(Args&&... args) {
    // auto item = std::make_unique<T>(std::forward<Args>(args)...);
    // T* ptr = item.get();

    T* ptr = menuItemPanel->Stack().CreateControl<T>(
      std::forward<Args>(args)...);

    OnItemAdded(ptr);
    return ptr;
  }

  std::vector<IContextMenuItem*> GetItems() {
    std::vector<IContextMenuItem*> items;

    for (auto& child : menuItemPanel->Stack().Children) {
      if (auto* item = dynamic_cast<IContextMenuItem*>(child)) {
        items.push_back(item);
      }
    }

    return items;
  }

protected:
  void OnItemAdded(IContextMenuItem* item);
};




#endif //TEST_IOS_APP_CONTEXTMENU_H
