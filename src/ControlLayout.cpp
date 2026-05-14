#include <kromakit/Control.h>

#include <cmath>

namespace {

bool NearlyEqual(const float a, const float b) {
	return std::fabs(a - b) < 0.01f;
}

bool PointNearlyEqual(const DUIPoint& a, const DUIPoint& b) {
	return NearlyEqual(a.x, b.x) && NearlyEqual(a.y, b.y);
}

bool SizeNearlyEqual(const DUISize& a, const DUISize& b) {
	return NearlyEqual(a.width, b.width) && NearlyEqual(a.height, b.height);
}

}

DUIRect Control::GetRectF()
{
	return DUIRect{
		0, 0,
		size.width,
		size.height
	};
}

DUIRect Control::GetBounds() const {
	return DUIRect(
		location,
		size
	);
}

DUISize Control::GetSize() {
	return {size.width, size.height};
}

bool Control::SetLayoutLocation(DUIPoint nextLocation)
{
	if (PointNearlyEqual(location, nextLocation))
		return false;

	location = nextLocation;
	MarkRenderTreeDirty();
	return true;
}

bool Control::SetLayoutSize(DUISize nextSize)
{
	if (SizeNearlyEqual(size, nextSize))
		return false;

	size = nextSize;
	MarkRenderTreeDirty();

	if (NeedsLayoutWhenOwnSizeChanges())
		MarkLayoutDirty();

	return true;
}

bool Control::SetLayoutFrame(DUIPoint nextLocation, DUISize nextSize)
{
	bool changed = false;
	changed |= SetLayoutLocation(nextLocation);
	changed |= SetLayoutSize(nextSize);
	return changed;
}

bool Control::SetLayoutContentSize(DUISize nextContentLayoutSize)
{
	if (SizeNearlyEqual(contentLayoutSize, nextContentLayoutSize))
		return false;

	contentLayoutSize = nextContentLayoutSize;
	return true;
}

void Control::SetContent(std::string wStr)
{
	if (sContent == wStr)
		return;

	sContent = wStr;
	MarkLayoutDirty();
}

void Control::SetSize(float w, float h)
{
	if (NearlyEqual(size.width, w) &&
		NearlyEqual(size.height, h))
	{
		return;
	}

	size = { w,h };
	MarkLayoutDirty();
}

bool Control::IsOutOfBounds()
{
	if (parent == nullptr)
		return false;

	return location.x > parent->size.width ||
		location.y > parent->size.height;
}
