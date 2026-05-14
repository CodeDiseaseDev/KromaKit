//
// Created by code on 5/2/26.
//

#include <kromakit/grid_layout_panel/GridLayoutPanel.h>
// layout/GridLayoutPanel.cpp
#include <kromakit/grid_layout_panel/GridLayoutPanel.h>

#include <algorithm>

#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

GridLayoutPanel::GridLayoutPanel() {
  UseStyle(DUIDefaultStyles
    ::SurfaceBaseStyles
    ::NeutralSurfaces::Clear);

  cornerRadius = DUIInsets(6);
}

void GridLayoutPanel::OnRender(Graphics *rendTarget) {
  rendTarget->FillControlBackground(this);
}

void GridLayoutPanel::SetGridRows(const std::vector<LayoutLength>& value)
{
  SetRows(value);
  MarkLayoutDirty();
}

void GridLayoutPanel::SetGridColumns(const std::vector<LayoutLength>& value)
{
  SetColumns(value);
  MarkLayoutDirty();
}

void GridLayoutPanel::SetGridPadding(const DUIInsets& value)
{
  SetPadding(value);
  MarkLayoutDirty();
}

void GridLayoutPanel::RegisterLayoutChild(
  Control* control,
  const GridLayoutOptions& placement,
  const bool ensureVisualChildAttached)
{
  if (control == nullptr) {
    return;
  }

  LayoutItem item;
  item.control = control;

  item.options.gridLayout.column = placement.column;
  item.options.gridLayout.row = placement.row;
  item.options.gridLayout.columnSpan = std::max(1, placement.columnSpan);
  item.options.gridLayout.rowSpan = std::max(1, placement.rowSpan);

  layoutChildren.push_back(item);

  if (ensureVisualChildAttached) {
    AddBorrowedControl(control);
  }

  MarkLayoutDirty();
}

void GridLayoutPanel::AddBorrowedLayoutChild(
    Control* control,
    const int column,
    const int row,
    const int columnSpan,
    const int rowSpan)
{
  RegisterLayoutChild(
    control,
    GridLayoutOptions{
      column,
      row,
      columnSpan,
      rowSpan
    },
    true);
}

// DUISize GridLayoutPanel::MeasureOverride(const DUISize& available)
// {
//   return IGridLayout::Measure(layoutChildren, available);
// }

void GridLayoutPanel::DoLayout(Graphics* g)
{
  const DUISize size = GetSize();

  const DUIRect rect = {
    0.0f,
    0.0f,
    size.width,
    size.height
  };

  IGridLayout::Arrange(layoutChildren, rect);
}
