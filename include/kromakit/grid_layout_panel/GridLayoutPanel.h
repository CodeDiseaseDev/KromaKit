//
// Created by code on 5/2/26.
//

#ifndef TEST_IOS_APP_GRIDLAYOUTPANEL_H
#define TEST_IOS_APP_GRIDLAYOUTPANEL_H

#include <kromakit/Panel.h>
#include <kromakit/layout/layout_types/IGridLayout.h>

class GridLayoutPanel :
  public Control,
  public IGridLayout {

public:
  GridLayoutPanel();

  void AddBorrowedLayoutChild(
      Control* control,
      int column,
      int row,
      int columnSpan = 1,
      int rowSpan = 1
  );

  template<class T, class... Args>
  requires std::is_base_of_v<Control, T>
  T* CreateLayoutChild(
    GridLayoutOptions placement,
    Args&&... args
  ) {
    T* ptr = CreateControl<T>(std::forward<Args>(args)...);
    RegisterLayoutChild(ptr, placement, false);
    return ptr;
  }

  void SetGridRows(const std::vector<LayoutLength>& value);
  void SetGridColumns(const std::vector<LayoutLength>& value);
  void SetGridPadding(const DUIInsets& value);

// protected:
  void OnRender(Graphics *rendTarget) override;
  void DoLayout(Graphics* g) override;

  std::optional<std::string> GetControlName() const override { return "GridLayoutPanel"; }
  const std::vector<std::string> GetDebugOverlayItems() override { return{}; }

private:
  void RegisterLayoutChild(
    Control* control,
    const GridLayoutOptions& placement,
    bool ensureVisualChildAttached);

  std::vector<LayoutItem> layoutChildren;
};



#endif //TEST_IOS_APP_GRIDLAYOUTPANEL_H
