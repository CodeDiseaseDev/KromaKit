//
// Created by code on 5/2/26.
//

#include <kromakit/layout/layout_types/IGridLayout.h>

#include <algorithm>
#include <numeric>

namespace {
float ClampNonNegative(const float value)
{
  return std::max(0.0f, value);
}

float ClampPercent(const float value)
{
  return std::clamp(value, 0.0f, 1.0f);
}

bool IsValidChild(const LayoutItem& child)
{
  return child.control != nullptr &&
    child.control->isVisible;
}



DUISize GetChildMeasuredSize(
  const LayoutItem& child,
  const DUISize& available)
{
  if (child.control == nullptr) {
    return {0.0f, 0.0f};
  }

  DUISize measured = child.control->Measure(available);
  if (measured.width < 0.0f || measured.height < 0.0f) {
    measured = child.control->GetSize();
  }

  measured.width = ClampNonNegative(measured.width);
  measured.height = ClampNonNegative(measured.height);
  return measured;
}

float GetHorizontalInsets(const LayoutItem& child)
{
  return
    ClampNonNegative(child.margin.left) +
    ClampNonNegative(child.margin.right) +
    ClampNonNegative(child.padding.left) +
    ClampNonNegative(child.padding.right);
}

float GetVerticalInsets(const LayoutItem& child)
{
  return
    ClampNonNegative(child.margin.top) +
    ClampNonNegative(child.margin.bottom) +
    ClampNonNegative(child.padding.top) +
    ClampNonNegative(child.padding.bottom);
}

float GetLayoutPaddingHorizontal(const DUIInsets& padding)
{
  return ClampNonNegative(padding.left) + ClampNonNegative(padding.right);
}

float GetLayoutPaddingVertical(const DUIInsets& padding)
{
  return ClampNonNegative(padding.top) + ClampNonNegative(padding.bottom);
}

int ClampTrackIndex(const int index, const int count)
{
  if (count <= 0) {
    return 0;
  }

  return std::clamp(index, 0, count - 1);
}

int ClampTrackSpan(const int index, const int span, const int count)
{
  if (count <= 0) {
    return 0;
  }

  const int safeSpan = std::max(1, span);
  return std::min(safeSpan, count - index);
}

float SumRange(
  const std::vector<float>& values,
  const int start,
  const int count)
{
  float result = 0.0f;

  for (int i = start; i < start + count && i < static_cast<int>(values.size()); ++i) {
    result += values[i];
  }

  return result;
}

std::vector<float> BuildOffsets(
  const std::vector<float>& sizes,
  const float start,
  const float spacing)
{
  std::vector<float> offsets(sizes.size(), start);

  float cursor = start;
  for (int i = 0; i < static_cast<int>(sizes.size()); ++i) {
    offsets[i] = cursor;
    cursor += sizes[i] + spacing;
  }

  return offsets;
}

std::vector<float> ResolveTracks(
  const std::vector<LayoutLength>& tracks,
  const float available,
  const bool horizontal,
  const std::vector<LayoutItem>& children)
{
  std::vector<float> resolved(tracks.size(), 0.0f);

  if (tracks.empty()) {
    return resolved;
  }

  const int trackCount = static_cast<int>(tracks.size());
  const float safeAvailable = ClampNonNegative(available);

  float used = 0.0f;
  float totalFillWeight = 0.0f;

  for (int i = 0; i < trackCount; ++i) {
    const LayoutLength& track = tracks[i];

    switch (track.unit) {
      case LayoutUnit::Fixed:
      case LayoutUnit::Spacing:
        resolved[i] = ClampNonNegative(track.value);
        used += resolved[i];
        break;

      case LayoutUnit::Percent:
        resolved[i] = safeAvailable * ClampPercent(track.value);
        used += resolved[i];
        break;

      case LayoutUnit::Fill:
        totalFillWeight += std::max(0.0f, track.value);
        break;

      case LayoutUnit::Auto:
      default:
        break;
    }
  }

  // Auto pass.
  // V1 rule: only single-span children affect Auto track measurement.
  for (const auto& child : children) {
    if (!IsValidChild(child)) {
      continue;
    }

    const GridLayoutOptions& grid = child.options.gridLayout;

    const int trackIndex = horizontal
      ? grid.column
      : grid.row;

    const int trackSpan = horizontal
      ? grid.columnSpan
      : grid.rowSpan;

    if (trackSpan != 1) {
      continue;
    }

    if (trackIndex < 0 || trackIndex >= trackCount) {
      continue;
    }

    if (tracks[trackIndex].unit != LayoutUnit::Auto) {
      continue;
    }

    const DUISize measured = GetChildMeasuredSize(child, {safeAvailable, safeAvailable});

    const float desired = horizontal
      ? measured.width + GetHorizontalInsets(child)
      : measured.height + GetVerticalInsets(child);

    resolved[trackIndex] = std::max(resolved[trackIndex], ClampNonNegative(desired));
  }

  for (int i = 0; i < trackCount; ++i) {
    if (tracks[i].unit == LayoutUnit::Auto) {
      used += resolved[i];
    }
  }

  const float remaining = std::max(0.0f, safeAvailable - used);

  if (totalFillWeight > 0.0f) {
    for (int i = 0; i < trackCount; ++i) {
      if (tracks[i].unit != LayoutUnit::Fill) {
        continue;
      }

      const float weight = std::max(0.0f, tracks[i].value);
      resolved[i] = remaining * (weight / totalFillWeight);
    }
  }

  return resolved;
}

float ResolveMeasuredLength(
  const std::vector<LayoutLength>& tracks,
  const float available,
  const bool horizontal,
  const std::vector<LayoutItem>& children)
{
  if (tracks.empty()) {
    return 0.0f;
  }

  bool hasFill = false;
  bool hasPercent = false;

  for (const auto& track : tracks) {
    if (track.unit == LayoutUnit::Fill) {
      hasFill = true;
    }

    if (track.unit == LayoutUnit::Percent) {
      hasPercent = true;
    }
  }

  // If the grid depends on parent space, report the available size.
  if (hasFill || hasPercent) {
    return ClampNonNegative(available);
  }

  const std::vector<float> resolved = ResolveTracks(
    tracks,
    available,
    horizontal,
    children);

  float total = 0.0f;
  for (const float size : resolved) {
    total += size;
  }

  return total;
}

DUIRect DeflateRectByChildInsets(
  const DUIRect& rect,
  const LayoutItem& child)
{
  const float left =
    ClampNonNegative(child.margin.left) +
    ClampNonNegative(child.padding.left);

  const float top =
    ClampNonNegative(child.margin.top) +
    ClampNonNegative(child.padding.top);

  const float right =
    ClampNonNegative(child.margin.right) +
    ClampNonNegative(child.padding.right);

  const float bottom =
    ClampNonNegative(child.margin.bottom) +
    ClampNonNegative(child.padding.bottom);

  return {
    rect.x + left,
    rect.y + top,
    ClampNonNegative(rect.width - left - right),
    ClampNonNegative(rect.height - top - bottom)
  };
}

DUIRect AlignChildInRect(
  const DUIRect& availableRect,
  const DUISize& measuredSize,
  const LayoutOptions& options)
{
  float width = std::min(ClampNonNegative(measuredSize.width), availableRect.width);
  float height = std::min(ClampNonNegative(measuredSize.height), availableRect.height);

  if (options.horizontalAlignment == LayoutAlignment::Stretch) {
    width = availableRect.width;
  }

  if (options.verticalAlignment == LayoutAlignment::Stretch) {
    height = availableRect.height;
  }

  float x = availableRect.x;
  float y = availableRect.y;

  if (options.horizontalAlignment == LayoutAlignment::Center) {
    x += (availableRect.width - width) * 0.5f;
  } else if (options.horizontalAlignment == LayoutAlignment::End) {
    x += availableRect.width - width;
  }

  if (options.verticalAlignment == LayoutAlignment::Center) {
    y += (availableRect.height - height) * 0.5f;
  } else if (options.verticalAlignment == LayoutAlignment::End) {
    y += availableRect.height - height;
  }

  return {
    x,
    y,
    ClampNonNegative(width),
    ClampNonNegative(height)
  };
}
}

void IGridLayout::SetRowSpacing(float value) {
  rowSpacing = ClampNonNegative(value);
}

void IGridLayout::SetColumnSpacing(float value) {
  columnSpacing = ClampNonNegative(value);
}

void IGridLayout::SetSpacing(float value) {
  const float safeValue = ClampNonNegative(value);
  rowSpacing = safeValue;
  columnSpacing = safeValue;
}

void IGridLayout::SetPadding(const DUIInsets& value)
{
  layoutPadding = value;
}

void IGridLayout::SetRows(const std::vector<LayoutLength>& value)
{
  rows = value;
}

void IGridLayout::SetColumns(const std::vector<LayoutLength>& value)
{
  columns = value;
}

std::vector<LayoutLength>& IGridLayout::GetRows()
{
  return rows;
}

std::vector<LayoutLength>& IGridLayout::GetColumns()
{
  return columns;
}

const std::vector<LayoutLength>& IGridLayout::GetRows() const
{
  return rows;
}

const std::vector<LayoutLength>& IGridLayout::GetColumns() const
{
  return columns;
}

DUISize IGridLayout::Measure(
  const std::vector<LayoutItem>& children,
  const DUISize& available)
{
  const float safeAvailableWidth = ClampNonNegative(available.width);
  const float safeAvailableHeight = ClampNonNegative(available.height);

  const float innerAvailableWidth = std::max(
    0.0f,
    safeAvailableWidth - GetLayoutPaddingHorizontal(layoutPadding));

  const float innerAvailableHeight = std::max(
    0.0f,
    safeAvailableHeight - GetLayoutPaddingVertical(layoutPadding));

  const float measuredWidth =
    ResolveMeasuredLength(columns, innerAvailableWidth, true, children) +
    GetLayoutPaddingHorizontal(layoutPadding);

  const float measuredHeight =
    ResolveMeasuredLength(rows, innerAvailableHeight, false, children) +
    GetLayoutPaddingVertical(layoutPadding);

  return {
    ClampNonNegative(measuredWidth),
    ClampNonNegative(measuredHeight)
  };
}

void IGridLayout::Arrange(
  std::vector<LayoutItem>& children,
  const DUIRect& rect)
{
  if (rows.empty() || columns.empty()) {
    return;
  }

  const float safeWidth = ClampNonNegative(rect.width);
  const float safeHeight = ClampNonNegative(rect.height);

  const float innerX = rect.x + ClampNonNegative(layoutPadding.left);
  const float innerY = rect.y + ClampNonNegative(layoutPadding.top);

  const float innerWidth = std::max(
    0.0f,
    safeWidth - GetLayoutPaddingHorizontal(layoutPadding));

  const float innerHeight = std::max(
    0.0f,
    safeHeight - GetLayoutPaddingVertical(layoutPadding));

  const float totalColumnSpacing =
    std::max(0, static_cast<int>(columns.size()) - 1) * columnSpacing;

  const float totalRowSpacing =
    std::max(0, static_cast<int>(rows.size()) - 1) * rowSpacing;

  const std::vector<float> columnWidths = ResolveTracks(
    columns,
    std::max(0.0f, innerWidth - totalColumnSpacing),
    true,
    children);

  const std::vector<float> rowHeights = ResolveTracks(
    rows,
    std::max(0.0f, innerHeight - totalRowSpacing),
    false,
    children);

  const std::vector<float> columnOffsets =
    BuildOffsets(columnWidths, innerX, columnSpacing);

  const std::vector<float> rowOffsets =
    BuildOffsets(rowHeights, innerY, rowSpacing);

  const int columnCount = static_cast<int>(columns.size());
  const int rowCount = static_cast<int>(rows.size());

  for (auto& child : children) {
    if (!IsValidChild(child)) {
      continue;
    }

    const GridLayoutOptions& grid = child.options.gridLayout;

    const int column = ClampTrackIndex(grid.column, columnCount);
    const int row = ClampTrackIndex(grid.row, rowCount);

    const int columnSpan = ClampTrackSpan(column, grid.columnSpan, columnCount);
    const int rowSpan = ClampTrackSpan(row, grid.rowSpan, rowCount);

    const float cellX = columnOffsets[column];
    const float cellY = rowOffsets[row];

    const float cellWidth =
      SumRange(columnWidths, column, columnSpan) +
      std::max(0, columnSpan - 1) * columnSpacing;

    const float cellHeight =
      SumRange(rowHeights, row, rowSpan) +
      std::max(0, rowSpan - 1) * rowSpacing;

    const DUIRect cellRect = {
      cellX,
      cellY,
      ClampNonNegative(cellWidth),
      ClampNonNegative(cellHeight)
    };

    const DUIRect contentRect = DeflateRectByChildInsets(cellRect, child);

    const DUISize measuredSize = GetChildMeasuredSize(
      child,
      {contentRect.width, contentRect.height});

    const DUIRect arrangedRect = AlignChildInRect(
      contentRect,
      measuredSize,
      child.options);

    child.control->SetLayoutFrame(
      {arrangedRect.x, arrangedRect.y},
      {arrangedRect.width, arrangedRect.height});
  }
}