//
// Created by code on 4/29/26.
//

#include <kromakit/layout/layout_types/IStackLayout.h>
#include <algorithm>

namespace {
float ClampNonNegative(const float value)
{
  return std::max(0.0f, value);
}

bool IsVertical(const IStackLayout::Direction direction)
{
  return direction == IStackLayout::Direction::Vertical;
}

float GetMainSize(const DUISize& size, const bool vertical)
{
  return vertical ? size.height : size.width;
}

float GetCrossSize(const DUISize& size, const bool vertical)
{
  return vertical ? size.width : size.height;
}

float GetMarginMainStart(const LayoutItem& item, const bool vertical)
{
  return vertical ? item.margin.top : item.margin.left;
}

float GetMarginMainEnd(const LayoutItem& item, const bool vertical)
{
  return vertical ? item.margin.bottom : item.margin.right;
}

float GetMarginCrossStart(const LayoutItem& item, const bool vertical)
{
  return vertical ? item.margin.left : item.margin.top;
}

float GetMarginCrossEnd(const LayoutItem& item, const bool vertical)
{
  return vertical ? item.margin.right : item.margin.bottom;
}

float GetPaddingMainStart(const LayoutItem& item, const bool vertical)
{
  return vertical ? item.padding.top : item.padding.left;
}

float GetPaddingMainEnd(const LayoutItem& item, const bool vertical)
{
  return vertical ? item.padding.bottom : item.padding.right;
}

float GetPaddingCrossStart(const LayoutItem& item, const bool vertical)
{
  return vertical ? item.padding.left : item.padding.top;
}

float GetPaddingCrossEnd(const LayoutItem& item, const bool vertical)
{
  return vertical ? item.padding.right : item.padding.bottom;
}

float ClampMainSize(float size, const LayoutItem& item)
{
  size = ClampNonNegative(size);
  size = std::max(size, item.minSize);
  if (item.maxSize >= 0.0f) {
    size = std::min(size, item.maxSize);
  }

  return size;
}

float GetSpacing(const float spacing)
{
  return std::max(0.0f, spacing);
}

float GetLayoutPaddingMainStart(const DUIInsets& padding, const bool vertical)
{
  return ClampNonNegative(vertical ? padding.top : padding.left);
}

float GetLayoutPaddingMainEnd(const DUIInsets& padding, const bool vertical)
{
  return ClampNonNegative(vertical ? padding.bottom : padding.right);
}

float GetLayoutPaddingCrossStart(const DUIInsets& padding, const bool vertical)
{
  return ClampNonNegative(vertical ? padding.left : padding.top);
}

float GetLayoutPaddingCrossEnd(const DUIInsets& padding, const bool vertical)
{
  return ClampNonNegative(vertical ? padding.right : padding.bottom);
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

bool IsValidChild(const LayoutItem& child)
{
  return child.control != nullptr && child.control->isVisible;
}

struct MeasuredChild {
  LayoutItem* item = nullptr;
  DUISize measuredSize{0.0f, 0.0f};

  float baseMain = 0.0f;
  float mainInsets = 0.0f;

  float marginMainStart = 0.0f;
  float marginMainEnd = 0.0f;
  float paddingMainStart = 0.0f;
  float paddingMainEnd = 0.0f;

  float marginCrossStart = 0.0f;
  float marginCrossEnd = 0.0f;
  float paddingCrossStart = 0.0f;
  float paddingCrossEnd = 0.0f;
};
}

IStackLayout::IStackLayout(Direction direction):
  direction(direction) {

}

void IStackLayout::SetSpacing(float value) {
  spacing = std::max(0.0f, value);
}

void IStackLayout::SetPadding(const DUIInsets& value) {
  layoutPadding = value;
}

DUISize IStackLayout::Measure(
  const std::vector<LayoutItem> &children,
  const DUISize &available) {
  const bool vertical = IsVertical(direction);
  const float childSpacing = GetSpacing(spacing);
  const float layoutPaddingMainStart = GetLayoutPaddingMainStart(layoutPadding, vertical);
  const float layoutPaddingMainEnd = GetLayoutPaddingMainEnd(layoutPadding, vertical);
  const float layoutPaddingCrossStart = GetLayoutPaddingCrossStart(layoutPadding, vertical);
  const float layoutPaddingCrossEnd = GetLayoutPaddingCrossEnd(layoutPadding, vertical);

  float mainSum = 0.0f;
  float crossMax = 0.0f;
  int validCount = 0;

  for (const auto& child : children) {
    if (!IsValidChild(child)) {
      continue;
    }

    ++validCount;

    const DUISize measuredSize = GetChildMeasuredSize(child, available);
    const float measuredMain = GetMainSize(measuredSize, vertical);
    const float measuredCross = GetCrossSize(measuredSize, vertical);

    float controlMain = child.fixedSize >= 0.0f
      ? child.fixedSize
      : measuredMain;
    controlMain = ClampMainSize(controlMain, child);

    const float controlCross = ClampNonNegative(measuredCross);

    const float outerMain = controlMain
      + ClampNonNegative(GetMarginMainStart(child, vertical))
      + ClampNonNegative(GetMarginMainEnd(child, vertical))
      + ClampNonNegative(GetPaddingMainStart(child, vertical))
      + ClampNonNegative(GetPaddingMainEnd(child, vertical));

    const float outerCross = controlCross
      + ClampNonNegative(GetMarginCrossStart(child, vertical))
      + ClampNonNegative(GetMarginCrossEnd(child, vertical))
      + ClampNonNegative(GetPaddingCrossStart(child, vertical))
      + ClampNonNegative(GetPaddingCrossEnd(child, vertical));

    mainSum += outerMain;
    crossMax = std::max(crossMax, outerCross);
  }

  if (validCount > 1) {
    mainSum += childSpacing * static_cast<float>(validCount - 1);
  }

  mainSum += layoutPaddingMainStart + layoutPaddingMainEnd;
  crossMax += layoutPaddingCrossStart + layoutPaddingCrossEnd;

  if (vertical) {
    return {crossMax, mainSum};
  }

  return {mainSum, crossMax};
}

void IStackLayout::Arrange(
  std::vector<LayoutItem> &children,
  const DUIRect &rect) {
  const bool vertical = IsVertical(direction);
  const float childSpacing = GetSpacing(spacing);
  const float layoutPaddingMainStart = GetLayoutPaddingMainStart(layoutPadding, vertical);
  const float layoutPaddingMainEnd = GetLayoutPaddingMainEnd(layoutPadding, vertical);
  const float layoutPaddingCrossStart = GetLayoutPaddingCrossStart(layoutPadding, vertical);
  const float layoutPaddingCrossEnd = GetLayoutPaddingCrossEnd(layoutPadding, vertical);

  const DUISize availableSize = {
    ClampNonNegative(rect.width),
    ClampNonNegative(rect.height)
  };

  const float rectMainStart = vertical ? rect.y : rect.x;
  const float rectCrossStart = vertical ? rect.x : rect.y;
  const float availableMain = std::max(
    0.0f,
    (vertical ? availableSize.height : availableSize.width) -
      layoutPaddingMainStart -
      layoutPaddingMainEnd
  );
  const float availableCross = std::max(
    0.0f,
    (vertical ? availableSize.width : availableSize.height) -
      layoutPaddingCrossStart -
      layoutPaddingCrossEnd
  );
  const float innerMainStart = rectMainStart + layoutPaddingMainStart;
  const float innerCrossStart = rectCrossStart + layoutPaddingCrossStart;

  std::vector<MeasuredChild> measuredChildren;
  measuredChildren.reserve(children.size());

  float totalOuterMain = 0.0f;
  float totalGrow = 0.0f;

  for (auto& child : children) {
    if (!IsValidChild(child)) {
      continue;
    }

    MeasuredChild measuredChild;
    measuredChild.item = &child;
    measuredChild.measuredSize = GetChildMeasuredSize(child, availableSize);

    measuredChild.baseMain = child.fixedSize >= 0.0f
      ? child.fixedSize
      : GetMainSize(measuredChild.measuredSize, vertical);
    measuredChild.baseMain = ClampMainSize(measuredChild.baseMain, child);

    measuredChild.marginMainStart = ClampNonNegative(GetMarginMainStart(child, vertical));
    measuredChild.marginMainEnd = ClampNonNegative(GetMarginMainEnd(child, vertical));
    measuredChild.paddingMainStart = ClampNonNegative(GetPaddingMainStart(child, vertical));
    measuredChild.paddingMainEnd = ClampNonNegative(GetPaddingMainEnd(child, vertical));

    measuredChild.marginCrossStart = ClampNonNegative(GetMarginCrossStart(child, vertical));
    measuredChild.marginCrossEnd = ClampNonNegative(GetMarginCrossEnd(child, vertical));
    measuredChild.paddingCrossStart = ClampNonNegative(GetPaddingCrossStart(child, vertical));
    measuredChild.paddingCrossEnd = ClampNonNegative(GetPaddingCrossEnd(child, vertical));

    measuredChild.mainInsets =
      measuredChild.marginMainStart +
      measuredChild.marginMainEnd +
      measuredChild.paddingMainStart +
      measuredChild.paddingMainEnd;

    totalOuterMain += measuredChild.baseMain + measuredChild.mainInsets;

    if (child.grow > 0.0f) {
      totalGrow += child.grow;
    }

    measuredChildren.push_back(measuredChild);
  }

  if (measuredChildren.empty()) {
    return;
  }

  const float spacingTotal = childSpacing * static_cast<float>(measuredChildren.size() - 1);
  const float remainingMain = std::max(
    0.0f,
    availableMain - totalOuterMain - spacingTotal
  );

  float cursorMain = innerMainStart;

  for (auto& measuredChild : measuredChildren) {
    LayoutItem& child = *measuredChild.item;

    float outerMain = measuredChild.baseMain + measuredChild.mainInsets;
    if (child.grow > 0.0f && totalGrow > 0.0f) {
      outerMain += remainingMain * (child.grow / totalGrow);
    }

    const float controlMainAvailable = std::max(
      0.0f,
      outerMain - measuredChild.mainInsets
    );
    const float arrangedMain = ClampMainSize(controlMainAvailable, child);
    const float arrangedOuterMain = arrangedMain + measuredChild.mainInsets;

    const float crossInnerStart = innerCrossStart +
      measuredChild.marginCrossStart +
      measuredChild.paddingCrossStart;
    const float crossInnerAvailable = std::max(
      0.0f,
      availableCross -
        measuredChild.marginCrossStart -
        measuredChild.marginCrossEnd -
        measuredChild.paddingCrossStart -
        measuredChild.paddingCrossEnd
    );

    float preferredCross = ClampNonNegative(GetCrossSize(measuredChild.measuredSize, vertical));
    preferredCross = std::min(preferredCross, crossInnerAvailable);

    const LayoutAlignment crossAlignment = vertical
      ? child.options.horizontalAlignment
      : child.options.verticalAlignment;

    float arrangedCross = preferredCross;
    if (crossAlignment == LayoutAlignment::Stretch) {
      arrangedCross = crossInnerAvailable;
    }

    float crossOffset = 0.0f;
    if (crossAlignment == LayoutAlignment::Center) {
      crossOffset = (crossInnerAvailable - arrangedCross) * 0.5f;
    } else if (crossAlignment == LayoutAlignment::End) {
      crossOffset = crossInnerAvailable - arrangedCross;
    }

    arrangedCross = ClampNonNegative(arrangedCross);
    crossOffset = ClampNonNegative(crossOffset);

    const float arrangedMainStart = cursorMain +
      measuredChild.marginMainStart +
      measuredChild.paddingMainStart;
    const float arrangedCrossStart = crossInnerStart + crossOffset;

    DUIPoint location = {};
    DUISize size = {};

    if (vertical) {
      location = {arrangedCrossStart, arrangedMainStart};
      size = {arrangedCross, arrangedMain};
    } else {
      location = {arrangedMainStart, arrangedCrossStart};
      size = {arrangedMain, arrangedCross};
    }

    child.control->SetLayoutFrame(location, size);

    cursorMain += arrangedOuterMain + childSpacing;
  }


}
