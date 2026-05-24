#include <kromakit/Control.h>
// #include "Application.h"
// #include "Window.h"

#include <kromakit/DUIWindow.h>
#include <kromakit/Label.h>
#include <kromakit/Panel.h>
#include <kromakit/Rendering/render_state/RenderItem.h>
#include <kromakit/Utils.h>
#include <kromakit/context_menu/ContextMenu.h>
#include <kromakit/context_menu/ContextMenuAttachment.h>
#include <kromakit/HapticsBridge.h>
#include <kromakit/Logging.h>

#include <algorithm>
#include <cmath>

// #include "GaussianBlurProvider.h"

#ifndef DUI_DEBUG_VERIFY_LAYOUT_STABILITY
#define DUI_DEBUG_VERIFY_LAYOUT_STABILITY 0
#endif

namespace {
#if DUI_DEBUG_VERIFY_LAYOUT_STABILITY
struct LayoutSnapshot {
	Control* control = nullptr;
	DUIPoint location = {0, 0};
	DUISize size = {0, 0};
	DUISize contentLayoutSize = {0, 0};
};
#endif

bool NearlyEqual(const float a, const float b) {
	return std::fabs(a - b) < 0.01f;
}

bool PointNearlyEqual(const DUIPoint& a, const DUIPoint& b) {
	return NearlyEqual(a.x, b.x) && NearlyEqual(a.y, b.y);
}

bool SizeNearlyEqual(const DUISize& a, const DUISize& b) {
	return NearlyEqual(a.width, b.width) && NearlyEqual(a.height, b.height);
}

bool IsAncestorOrDescendantRange(
	const uint32_t aStart,
	const uint32_t aEnd,
	const uint32_t bStart,
	const uint32_t bEnd)
{
	const bool aContainsB = aStart <= bStart && bStart <= aEnd;
	const bool bContainsA = bStart <= aStart && aStart <= bEnd;
	return aContainsB || bContainsA;
}

bool HasArea(const DUIRect& rect) {
	return rect.width > 0.01f && rect.height > 0.01f;
}

DUIRect ScaleRectAroundPoint(
	const DUIRect& rect,
	const DUIPoint& origin,
	float scale)
{
	return {
		origin.x + (rect.x - origin.x) * scale,
		origin.y + (rect.y - origin.y) * scale,
		rect.width * scale,
		rect.height * scale
	};
}

#if DUI_DEBUG_VERIFY_LAYOUT_STABILITY
void BuildLayoutSnapshot(Control* control, std::vector<LayoutSnapshot>& snapshot) {
	if (control == nullptr)
		return;

	snapshot.push_back({
		control,
		control->location,
		control->size,
		control->contentLayoutSize
	});

	for (auto& child : control->Children) {
		BuildLayoutSnapshot(child, snapshot);
	}
}

bool LayoutChanged(
	const std::vector<LayoutSnapshot>& before,
	const std::vector<LayoutSnapshot>& after)
{
	if (before.size() != after.size())
		return true;

	for (size_t i = 0; i < before.size(); ++i)
	{
		const auto& lhs = before[i];
		const auto& rhs = after[i];

		if (lhs.control != rhs.control)
			return true;

		if (!NearlyEqual(lhs.location.x, rhs.location.x) ||
			!NearlyEqual(lhs.location.y, rhs.location.y) ||
			!NearlyEqual(lhs.size.width, rhs.size.width) ||
			!NearlyEqual(lhs.size.height, rhs.size.height) ||
			!NearlyEqual(lhs.contentLayoutSize.width, rhs.contentLayoutSize.width) ||
			!NearlyEqual(lhs.contentLayoutSize.height, rhs.contentLayoutSize.height))
		{
			return true;
		}
	}

	return false;
}
#endif

DUIWindow* ResolveRootWindowNoLog(Control* control) {
	for (Control* node = control; node != nullptr; node = node->parent) {
		if (auto* window = dynamic_cast<DUIWindow*>(node))
			return window;
	}

	return nullptr;
}

DUIRect IntersectRect(const DUIRect& a, const DUIRect& b) {
	const float left = std::max(a.x, b.x);
	const float top = std::max(a.y, b.y);
	const float right = std::min(a.Right(), b.Right());
	const float bottom = std::min(a.Bottom(), b.Bottom());

	const float width = right - left;
	const float height = bottom - top;
	if (width <= 0.01f || height <= 0.01f)
		return {0, 0, 0, 0};

	return {left, top, width, height};
}

void SubtractRect(
	const DUIRect& subject,
	const DUIRect& cutter,
	std::vector<DUIRect>& out)
{
	const DUIRect overlap = IntersectRect(subject, cutter);
	if (!HasArea(overlap)) {
		out.push_back(subject);
		return;
	}

	// Top segment.
	const DUIRect top {
		subject.x,
		subject.y,
		subject.width,
		overlap.y - subject.y
	};
	if (HasArea(top))
		out.push_back(top);

	// Bottom segment.
	const DUIRect bottom {
		subject.x,
		overlap.Bottom(),
		subject.width,
		subject.Bottom() - overlap.Bottom()
	};
	if (HasArea(bottom))
		out.push_back(bottom);

	// Left segment (within overlap vertical band).
	const DUIRect left {
		subject.x,
		overlap.y,
		overlap.x - subject.x,
		overlap.height
	};
	if (HasArea(left))
		out.push_back(left);

	// Right segment (within overlap vertical band).
	const DUIRect right {
		overlap.Right(),
		overlap.y,
		subject.Right() - overlap.Right(),
		overlap.height
	};
	if (HasArea(right))
		out.push_back(right);
}

}

void Control::OnDebugOverlayRender(Graphics* rendTarget) {

}

Control::~Control() = default;

void Control::Update(float deltaTime) {
	(void)deltaTime;
}

void Control::UpdateTree(float deltaTime)
{
	if (!isVisible)
		return;

	Update(deltaTime);

	for (auto& child : Children)
	{
		if (!child || !child->isVisible)
			continue;

		child->UpdateTree(deltaTime);
	}
}

void Control::InternalDoLayout(Graphics* renderTarget)
{

}

LayoutResult Control::InternalDoLayout2(Graphics* renderTarget)
{
	const DUIPoint oldLocation = location;
	const DUISize oldSize = size;
	const DUISize oldContentLayoutSize = contentLayoutSize;

	InternalDoLayout(renderTarget);

	LayoutResult result;
	result.geometryChanged =
		!PointNearlyEqual(oldLocation, location) ||
		!SizeNearlyEqual(oldSize, size) ||
		!SizeNearlyEqual(oldContentLayoutSize, contentLayoutSize);

	if (result.geometryChanged)
		MarkRenderTreeDirty();

	return result;
}

LayoutResult Control::DoLayout2(Graphics* renderTarget)
{
	const DUIPoint oldLocation = location;
	const DUISize oldSize = size;
	const DUISize oldContentLayoutSize = contentLayoutSize;

	DoLayout(renderTarget);

	LayoutResult result;
	result.geometryChanged =
		!PointNearlyEqual(oldLocation, location) ||
		!SizeNearlyEqual(oldSize, size) ||
		!SizeNearlyEqual(oldContentLayoutSize, contentLayoutSize);

	if (result.geometryChanged)
		MarkRenderTreeDirty();

	return result;
}

void Control::OnPostRender(Graphics *rendTarget) {
}

Control::Control()
{
	size = { 100, 100 };
	location = { 0, 0 };

	foreground = { 1,1,1,1 };



	AssignOwnID();
}

void Control::OnPropertyChanged(IDUIProperty *property) {
	if (!property)
		return;

	switch (property->GetKind()) {
		case DUIPropertyKind::Layout:
			MarkLayoutDirty();
			break;

		case DUIPropertyKind::Visual:
			MarkVisualDirty();
			break;

		case DUIPropertyKind::Style:
			MarkLayoutDirty();
			break;

		case DUIPropertyKind::Transform:
			MarkRenderTreeDirty();
			break;

		case DUIPropertyKind::Internal:
			break;

		default:
			break;
	}
}

void Control::OnScroll(float deltaX, float deltaY) {

}

void Control::OnKeyDown(const DUIKeyEvent &keyEvent) {}
void Control::OnKeyUp(const DUIKeyEvent &keyEvent) {}
void Control::OnTextInput(const DUITextInputEvent &textInputEvent) {}

bool Control::HandleShortcut(const DUIKeyEvent &keyEvent) {
	return false;
}

bool Control::IsFullyCovered(
	const DUIRect& rect,
	const std::vector<DUIRect>& regions)
{
	return false;
	// if (!HasArea(rect))
	// 	return true;
	//
	// std::vector<DUIRect> remaining { rect };
	//
	// for (const DUIRect& cover : regions)
	// {
	// 	if (remaining.empty())
	// 		return true;
	//
	// 	std::vector<DUIRect> nextRemaining;
	// 	nextRemaining.reserve(remaining.size() * 2);
	//
	// 	for (const DUIRect& part : remaining)
	// 		SubtractRect(part, cover, nextRemaining);
	//
	// 	remaining.swap(nextRemaining);
	// }
	//
	// return remaining.empty();
}

bool Control::IsInView(
	const DUIRect& rect,
	const DUISize viewportSize)
{
	return false;
	// DUIRect viewport {
	// 	0, 0,
	// 	viewportSize.width,
	// 	viewportSize.height
	// };
	//
	// return rect.Intersects(viewport);
}

void Control::RenderListCulling(
	std::vector<RenderItem>& list,
	const DUISize viewportSize)
{
	// std::stable_sort(list.begin(), list.end(), [](const auto& a, const auto& b) {
	// 	// Draw order is back-to-front: lower z first.
	// 	return a.z < b.z;
	// });
	//
	// struct CoveredRegion {
	// 	DUIRect rect;
	// 	uint32_t treeStart = 0;
	// 	uint32_t treeEnd = 0;
	// };
	// std::vector<CoveredRegion> covered;
	// covered.reserve(list.size());
	//
	// // Coverage must be evaluated front-to-back so top-most opaque controls
	// // can cull only controls behind them.
	// for (int i = static_cast<int>(list.size()) - 1; i >= 0; --i)
	// {
	// 	RenderItem& item = list[i];
	// 	const DUIRect cullRect = item.clipRect;
	//
	// 	if (!IsInView(cullRect, viewportSize))
	// 	{
	// 		item.visible = false;
	// 		continue;
	// 	}
	//
	// 	bool fullyCovered = false;
	// 	for (const CoveredRegion& region : covered) {
	// 		// Descendants/ancestors should not cull each other.
	// 		if (IsAncestorOrDescendantRange(
	// 				region.treeStart, region.treeEnd,
	// 				item.treeStart, item.treeEnd))
	// 			continue;
	//
	// 		// Cheap full-cover test: one opaque region fully contains this rect.
	// 		if (region.rect.ContainsRect(cullRect)) {
	// 			fullyCovered = true;
	// 			break;
	// 		}
	// 	}
	//
	// 	if (fullyCovered)
	// 	{
	// 		item.visible = false;
	// 		continue;
	// 	}
	//
	// 	item.visible = true;
	//
	// 	if (item.opaque)
	// 		covered.push_back({ cullRect, item.treeStart, item.treeEnd });
	// }
}

void Control::RenderFromList(
	Graphics* rendTarget,
	std::vector<RenderItem>& list,
	bool allow_translate,
	bool debug_overlay)
{
	// for (RenderItem& item : list)
	// {
	// 	if (!item.visible)
	// 		continue;
	//
	// 	if (!item.ctrl->isVisible)
	// 		continue;
	//
	// 	// If parent clipping already eliminated this region, avoid issuing
	// 	// draw calls for controls that cannot contribute any pixels.
	// 	if (item.clipRect.width <= 0.0f || item.clipRect.height <= 0.0f)
	// 		continue;
	//
	// 	Control* ctrl = item.ctrl;
	// 	ctrl->cachedScreenLocation = {
	// 		item.screenRect.x,
	// 		item.screenRect.y
	// 	};
	//
	// 	rendTarget->SaveDrawingState();
	// 	rendTarget->SetOpacity(item.opacity);
	//
	// 	// reset transform (important!)
	// 	nvgResetTransform(rendTarget->_RenderTarget);
	// 	nvgResetScissor(rendTarget->_RenderTarget);
	//
	// 	// clip set region
	// 	DUIRect clipRectForRender = item.clipRect;
	// 	const bool hasInheritedScale =
	// 		std::fabs(item.inheritedCentredScale - 1.0f) > 0.0001f;
	// 	const bool isLocalScaleRoot =
	// 		std::fabs(ctrl->GetCentredScale() - 1.0f) > 0.0001f;
	//
	// 	// Keep the scale-root item's clip in screen space so its own expanded
	// 	// background can remain visible; descendants still receive scaled clips.
	// 	if (hasInheritedScale && !isLocalScaleRoot) {
	// 		clipRectForRender = ScaleRectAroundPoint(
	// 			item.clipRect,
	// 			item.inheritedCentredScaleOrigin,
	// 			item.inheritedCentredScale);
	// 	}
	//
	// 	nvgScissor(
	// 		rendTarget->_RenderTarget,
	// 		clipRectForRender.x,
	// 		clipRectForRender.y,
	// 		clipRectForRender.width,
	// 		clipRectForRender.height);
	//
	// 	if (hasInheritedScale) {
	// 		nvgTranslate(
	// 			rendTarget->_RenderTarget,
	// 			item.inheritedCentredScaleOrigin.x,
	// 			item.inheritedCentredScaleOrigin.y);
	//
	// 		nvgScale(
	// 			rendTarget->_RenderTarget,
	// 			item.inheritedCentredScale,
	// 			item.inheritedCentredScale);
	//
	// 		nvgTranslate(
	// 			rendTarget->_RenderTarget,
	// 			-item.inheritedCentredScaleOrigin.x,
	// 			-item.inheritedCentredScaleOrigin.y);
	// 	}
	//
	// 	// move to absolute screen position
	// 	nvgTranslate(
	// 		rendTarget->_RenderTarget,
	// 		item.visualRect.x,
	// 		item.visualRect.y);
	//
	// 	ctrl->OnRender(rendTarget);
	//
	// 	// debug stuff
	// 	if (ctrl->__debug__devtools_highlighted)
	// 	{
	// 		rendTarget->FillRectangle(
	// 			0, 0,
	// 			ctrl->size.width,
	// 			ctrl->size.height,
	// 			ctrl->__debug__devtools_highlighted_color);
	//
	// 		rendTarget->DrawRectangle(
	// 			0, 0,
	// 			ctrl->size.width,
	// 			ctrl->size.height,
	// 			ctrl->__debug__devtools_highlighted_color);
	// 	}
	//
	// 	rendTarget->RestoreDrawingState();
	// }
}

bool Control::UsesRoundedCorners() const {
	return !cornerRadius.IsEmpty();
}

bool Control::IsFullyOpaque() const {
	return opacity >= 0.999f &&
		background.a >= 0.999f;
}

void Control::DoFullLayout(Graphics* g)
{
	DoFullLayout2(g);
}

LayoutResult Control::DoFullLayout2(Graphics* g)
{
	LayoutResult result;
	result.Merge(InternalDoLayout2(g));
	result.Merge(DoLayout2(g));

	layoutDirtySelf = false;

	for (auto& child : Children)
	{
		if (!child || !child->isVisible)
			continue;

		result.Merge(child->DoFullLayout2(g));
	}

	isLayoutDirty = layoutDirtySelf;
	for (const auto& child : Children) {
		if (child && child->isLayoutDirty) {
			isLayoutDirty = true;
			break;
		}
	}

	return result;
}

LayoutResult Control::LayoutDirtySubtree(Graphics* g, bool forceLayout)
{
	LayoutResult result;

	const bool shouldLayoutSelf = forceLayout || layoutDirtySelf;
	if (shouldLayoutSelf) {
		layoutDirtySelf = false;
		result.Merge(InternalDoLayout2(g));
		result.Merge(DoLayout2(g));
	}

	const bool forceChildLayout =
		forceLayout ||
		(shouldLayoutSelf && ShouldLayoutChildrenAfterOwnLayout());

	for (auto& child : Children)
	{
		if (!child || !child->isVisible)
			continue;

		if (!forceChildLayout &&
				!child->layoutDirtySelf &&
				!child->isLayoutDirty) {
			continue;
		}

		result.Merge(child->LayoutDirtySubtree(g, forceChildLayout));
	}

	isLayoutDirty = layoutDirtySelf;
	for (const auto& child : Children) {
		if (child && child->isLayoutDirty) {
			isLayoutDirty = true;
			break;
		}
	}

	return result;
}

float Control::GetFinalOpacity() const
{
	float result = opacity;

	const Control* p = parent;
	while (p != nullptr)
	{
		result *= p->opacity;
		p = p->parent;
	}

	return result;
}

void Control::BuildRenderList(
	std::vector<RenderItem> &list,
	DUIPoint parentScreenPos,
	DUIRect parentClip,
	float parentOpacity,
	DUIPoint parentVisualOffset,
	RenderLayer inheritedLayer,
	uint32_t *traversalCounter,
	float inheritedCentredScale,
	DUIPoint inheritedCentredScaleOrigin)
{
	if (!isVisible)
		return;

	uint32_t localTraversalCounter = 0;
	if (traversalCounter == nullptr)
		traversalCounter = &localTraversalCounter;

	const uint32_t nodeTreeStart = (*traversalCounter)++;

	const RenderLayer effectiveLayer =
		(inheritedLayer == RenderLayer::Overlay || GetRenderLayer() == RenderLayer::Overlay)
		? RenderLayer::Overlay
		: RenderLayer::Content;

	// Base layout position in screen space (no renderOffset).
	DUIPoint screenPos {
		parentScreenPos.x + location.x,
		parentScreenPos.y + location.y
	};

	/// this is incorrect code now because `ScrollView` exists now
	// if (parent != nullptr) {
	// 	screenPos.x += parent->scrollOffset.x;
	// 	screenPos.y += parent->scrollOffset.y;
	// }

	// Accumulate visual-only offset separately so screenPos remains stable.
	DUIPoint visualOffset {
		parentVisualOffset.x + renderOffset.x,
		parentVisualOffset.y + renderOffset.y
	};

	DUIPoint visualScreenPos {
		screenPos.x + visualOffset.x,
		screenPos.y + visualOffset.y
	};

	// Base rect (no renderOffset).
	DUIRect rect {
		screenPos.x,
		screenPos.y,
		size.width,
		size.height
	};

	// Visual rect used for clipping/culling/rasterization.
	DUIRect visualRect {
		visualScreenPos.x,
		visualScreenPos.y,
		size.width,
		size.height
	};

	const bool shouldClipToBounds = ShouldClipToBounds();

	// Controls are always constrained by ancestor clip.
	DUIRect clip = parentClip;

	// If this control clips to bounds, intersect with its visual rect.
	if (parent == nullptr || shouldClipToBounds) {
		clip = parentClip.IntersectRects(visualRect);
	}

	// Entire subtree is clipped out by viewport/ancestor clip.
	if (clip.width <= 0.0f || clip.height <= 0.0f)
		return;

	// float thisOpacity = opacity;
	// if (parent != nullptr) {
	// 	thisOpacity *= parent->opacity;
	// }
	float thisOpacity = parentOpacity * opacity;
	const bool isOpaqueForOcclusion =
		shouldClipToBounds &&
		thisOpacity >= 0.999f &&
		background.a >= 0.999f;

	float currentCentredScale = inheritedCentredScale;
	DUIPoint currentCentredScaleOrigin = inheritedCentredScaleOrigin;

	const float localCentredScale = GetCentredScale();
	if (std::fabs(localCentredScale - 1.0f) > 0.0001f) {
		currentCentredScale = inheritedCentredScale * localCentredScale;
		currentCentredScaleOrigin = {
			visualRect.x + visualRect.width * 0.5f,
			visualRect.y + visualRect.height * 0.5f
		};
	}

	// push *this* control into the list.
	const size_t thisRenderItemIndex = list.size();
	list.push_back({
		this,
		rect,
		visualRect,
		isOpaqueForOcclusion,
		zIndex, // however you store this
		true,
		renderOffset,
		clip,
		thisOpacity,
		effectiveLayer,
		nodeTreeStart,
		nodeTreeStart,
		currentCentredScale,
		currentCentredScaleOrigin
	});

	// recurse into children
	for (auto& child : Children)
	{
		if (!child)
			continue;

		if (!child->isVisible)
			continue;

		child->BuildRenderList(
			list,
			screenPos,
			clip,
			thisOpacity,
			visualOffset,
			effectiveLayer,
			traversalCounter,
			currentCentredScale,
			currentCentredScaleOrigin);
	}

	list[thisRenderItemIndex].treeEnd = (*traversalCounter) - 1;



}

bool Control::StabilizeLayout(Graphics* rendTarget) {
	constexpr int kMaxLayoutPasses = 6;
	bool forceFullPass = layoutDirtySelf;

#if DUI_DEBUG_VERIFY_LAYOUT_STABILITY
	std::vector<LayoutSnapshot> debugBefore;
	std::vector<LayoutSnapshot> debugAfter;
#endif

	for (int pass = 0; pass < kMaxLayoutPasses; ++pass)
	{
		if (pass > 0)
			rendTarget->DeltaTime = 0.0f;

#if DUI_DEBUG_VERIFY_LAYOUT_STABILITY
		debugBefore.clear();
		BuildLayoutSnapshot(this, debugBefore);
#endif

		const uint64_t layoutVersionBefore = GetLayoutVersion();
		const LayoutResult result = LayoutDirtySubtree(rendTarget, forceFullPass);
		const bool layoutRequestedDuringPass =
			GetLayoutVersion() != layoutVersionBefore ||
			isLayoutDirty ||
			layoutDirtySelf;

#if DUI_DEBUG_VERIFY_LAYOUT_STABILITY
		debugAfter.clear();
		BuildLayoutSnapshot(this, debugAfter);

		const bool snapshotChanged = LayoutChanged(debugBefore, debugAfter);
		if (snapshotChanged && !result.geometryChanged) {
			Logging::Log(
				"Layout verification: geometry changed without LayoutResult at pass %d for %s\n",
				pass,
				GetControlName().value_or("<name unset>").c_str());
		}
#endif

		if (!result.geometryChanged &&
				!result.needsAnotherPass &&
				!layoutRequestedDuringPass) {
			return true;
		}

		forceFullPass = result.geometryChanged || result.needsAnotherPass;
	}

	// if (background.r == 255 &&
	// 				background.g == 0 &&
	// 				background.b == 255) {
	//
	// 	Logging::Log("%s is magenta\n",
	// 		GetControlName().c_str());
	// }

	return false;
}

void Control::RenderRoot(Graphics* rendTarget)
{
	UpdateTree(rendTarget->DeltaTime);

	if (isLayoutDirty) {
		const float originalDeltaTime = rendTarget->DeltaTime;

		if (!StabilizeLayout(rendTarget))
			isLayoutDirty = true;

		rendTarget->DeltaTime = originalDeltaTime;
	}

	std::vector<RenderItem> list;

	BuildRenderList(list, {0, 0}, GetRectF());
	RenderListCulling(list, rendTarget->viewportSize);

	RenderFromList(rendTarget, list, true, false);


}

// void Control::MarkLayoutDirty()
// {
// 	layoutDirtySelf = true;
//
// 	for (Control* control = this; control != nullptr; control = control->parent) {
// 		control->isLayoutDirty = true;
// 	}
//
// 	if (DUIWindow* window = ResolveRootWindowNoLog(this))
// 		window->MarkLayoutVersionDirty();
// }
void Control::MarkLayoutDirty()
{
	if (layoutDirtySelf && isLayoutDirty)
		return;

	layoutDirtySelf = true;

	bool changedAnyAncestor = false;

	for (Control* control = this; control != nullptr; control = control->parent) {
		if (!control->isLayoutDirty) {
			control->isLayoutDirty = true;
			changedAnyAncestor = true;
		}
	}

	if (!changedAnyAncestor)
		return;

	if (DUIWindow* window = ResolveRootWindowNoLog(this))
		window->MarkLayoutVersionDirty();
}

void Control::MarkRenderTreeDirty()
{
	if (DUIWindow* window = ResolveRootWindowNoLog(this))
		window->MarkRenderTreeDirty();
}

void Control::MarkRenderOrderDirty()
{
	if (DUIWindow* window = ResolveRootWindowNoLog(this))
		window->MarkRenderOrderDirty();
}

void Control::MarkVisualDirty()
{
	if (DUIWindow* window = ResolveRootWindowNoLog(this))
		window->MarkVisualDirty();
}

uint64_t Control::GetRenderTreeVersion() const
{
	if (DUIWindow* window = ResolveRootWindowNoLog(const_cast<Control*>(this)))
		return window->GetRenderTreeVersion();
	return 0;
}

uint64_t Control::GetRenderOrderVersion() const
{
	if (DUIWindow* window = ResolveRootWindowNoLog(const_cast<Control*>(this)))
		return window->GetRenderOrderVersion();
	return 0;
}

uint64_t Control::GetVisualVersion() const
{
	if (DUIWindow* window = ResolveRootWindowNoLog(const_cast<Control*>(this)))
		return window->GetVisualVersion();
	return 0;
}

uint64_t Control::GetLayoutVersion() const
{
	if (DUIWindow* window = ResolveRootWindowNoLog(const_cast<Control*>(this)))
		return window->GetLayoutVersion();
	return 0;
}

bool Control::IsControlInView_(Control* ctrl, Graphics* graphics, DUIPoint location) {
	if (ctrl == nullptr || graphics == nullptr)
		return false;

	if (ctrl->size.width <= 0 || ctrl->size.height <= 0)
		return false;

	DUIRect controlRect = {
		location.x,
		location.y,
		ctrl->size.width,
		ctrl->size.height,
	};

	DUIRect viewportRect = {
		0, 0,
		graphics->viewportSize.width,
		graphics->viewportSize.height
	};

	return controlRect.Intersects(viewportRect);
}

void Control::PrintDebugTree() {
	PrintDebugTreeRecursive("", true, true);
}

void Control::PrintDebugTreeRecursive(
	const std::string &prefix, bool isLast, bool isRoot) {

	std::string line = prefix;

	if (!isRoot) {
		line += isLast ? "└─ " : "├─ ";
	}

	line += GetControlName().value_or("<name unset>");

	Logging::Log("%s\n", line.c_str());

	std::string childPrefix = prefix;

	if (!isRoot) {
		childPrefix += isLast ? "   " : "│  ";
	}

	for (size_t i = 0; i < Children.size(); i++) {
		auto child = Children[i];

		if (child == nullptr) {
			std::string nullLine = childPrefix;
			nullLine += (i == Children.size() - 1) ? "└─ " : "├─ ";
			nullLine += "<null>";
			Logging::Log("%s\n", nullLine.c_str());
			continue;
		}

		child->PrintDebugTreeRecursive(
				childPrefix,
				i == Children.size() - 1,
				false
		);
	}
}

std::optional<std::string> Control::GetControlName() const {
	return std::nullopt;
}

bool Control::UseStyle(const DUIStyleName &name) {
	if (name.Empty()) {
		return false;
	}

	DUIStyleRegistry::Apply("*", *this);

	if (!DUIStyleRegistry::Apply(
		name.ToString(), *this)) {

		Logging::Log("Style not found or invalid "
				"for control: %s\n",
				name.c_str());

		return false;
	}

	styleName = name;
	MarkLayoutDirty();
	return true;
}

const DUIStyleName & Control::GetStyleName() const {
	return styleName;
}

void Control::SetRenderLayer(RenderLayer layer) {
	if (renderLayer == layer)
		return;

	renderLayer = layer;
	MarkRenderTreeDirty();
}

void Control::SetVisible(bool visible) {
	if (isVisible == visible)
		return;

	isVisible = visible;
	MarkRenderTreeDirty();
}

void Control::SetOpacity(float nextOpacity) {
	if (NearlyEqual(opacity, nextOpacity))
		return;

	opacity = nextOpacity;
	MarkRenderTreeDirty();
}

void Control::SetCentredScale(float scale) {
	// const float clamped = std::clamp(scale, 0.1f, 4.0f);
	// constexpr float kScaleEpsilon = 0.00001f;
	//
	// if (std::fabs(centredScale - clamped) < kScaleEpsilon)
	// 	return;

	centredScale = scale;//clamped;

	MarkVisualDirty();
	MarkRenderTreeDirty();
}

float Control::GetCentredScale() const {
	return centredScale;
}

void Control::SetZIndex(int nextZIndex) {
	if (zIndex == nextZIndex)
		return;

	zIndex = nextZIndex;
	MarkRenderOrderDirty();
}

void Control::SetFont(DUIFont newFont) {
	if (newFont != font) {
		font = newFont;
		MarkLayoutDirty();
	}
}

bool Control::Contains(float x, float y) {
	return (x >= this->location.x &&
		x <= this->location.x + this->size.width &&
		y >= this->location.y &&
		y <= this->location.y + this->size.height);
}

void Control::OnPointerMove(const PointerEvent &e) {}
void Control::OnPointerDown(const PointerEvent &e) {}
void Control::OnPointerUp(const PointerEvent &e) {}
void Control::OnPointerCancel(const PointerEvent &e) {}
void Control::OnScroll(const PointerEvent &e) {}
void Control::OnWheelScroll(const WheelScrollEvent& e) { (void)e; }

void Control::SetContextMenuProvider(ContextMenuProvider provider) {
	if (!provider) {
		ClearContextMenuProvider();
		return;
	}

	if (!contextMenuAttachment_) {
		contextMenuAttachment_ = std::make_unique<ContextMenuAttachment>();
	}

	contextMenuAttachment_->SetProvider(std::move(provider));
}

void Control::ClearContextMenuProvider() {
	if (!contextMenuAttachment_) {
		return;
	}

	contextMenuAttachment_->ClearProvider();
}

bool Control::HasContextMenuProvider() const {
	return contextMenuAttachment_ &&
		contextMenuAttachment_->HasProvider();
}

void Control::SetContextMenuHoldThresholdMs(int milliseconds) {
	if (!contextMenuAttachment_) {
		contextMenuAttachment_ = std::make_unique<ContextMenuAttachment>();
	}

	contextMenuAttachment_->SetHoldThresholdMs(milliseconds);
}

int Control::GetContextMenuHoldThresholdMs() const {
	if (!contextMenuAttachment_) {
		return ContextMenuAttachment::kDefaultHoldThresholdMs;
	}

	return contextMenuAttachment_->GetHoldThresholdMs();
}

bool Control::BeginContextMenuHoldTracking() {
	if (!HasContextMenuProvider()) {
		return false;
	}

	contextMenuAttachment_->HoldInteractionState().PointerDown();
	return true;
}

bool Control::TryConsumeContextMenuHoldIfReady() {
	if (!contextMenuAttachment_) {
		return false;
	}

	return contextMenuAttachment_->HoldInteractionState().ConsumeHoldIfReady();
}

void Control::EndContextMenuHoldTracking() {
	if (!contextMenuAttachment_) {
		return;
	}

	(void)contextMenuAttachment_->HoldInteractionState().PointerUp();
}

void Control::CancelContextMenuHoldTracking() {
	if (!contextMenuAttachment_) {
		return;
	}

	contextMenuAttachment_->HoldInteractionState().Cancel();
}

void Control::ContextMenuHapticFeedback() {
	DirectUIHaptics::Impact(2);
}

std::unique_ptr<ContextMenu> Control::CreateContextMenuFromProvider() const {
	if (!HasContextMenuProvider()) {
		return nullptr;
	}

	const auto& provider = contextMenuAttachment_->GetProvider();
	if (!provider) {
		return nullptr;
	}

	return provider();
}

bool Control::TryToPresentContextMenu() {
	if (DUIWindow* wndControl = GetRootWindow()) {
		return wndControl->TryPresentContextMenuForTarget(this);
	}

	Logging::Log("TryToPresentContextMenu failed at GetRootWindow\n");
	return false;
}

void Control::OnMouseDrag(float deltaX, float deltaY) {}
void Control::OnMouseMove(int x, int y) {}
void Control::OnMouseMoveGlobal(int gx, int gy) {}
void Control::OnMouseDown() {}
void Control::OnMouseUp() {}
void Control::OnMouseIn() {}
void Control::OnMouseOut() {}
void Control::OnKeyDown(int key) {}
void Control::OnKeyUp(int key) {}
void Control::OnIsSelectedChanged() {}

bool Control::SetDebugOverlayVisible(bool visible) {
	bool original = debug_overlay_enabled;
	debug_overlay_enabled = visible;

	return original;
}

void Control::AlignToX(
	Control* target,
	XAlignment alignment,
	float spacing,
	bool useParentLocation)
{
	float offsetX = target->location.x;

	if (!useParentLocation)
		offsetX = 0;


	switch (alignment)
	{
	case BeforeLeft:
		location.x = offsetX + target->location.y - size.width - spacing;
		break;

	default:
	case Left:
		location.x = offsetX + spacing;
		break;

	case XCenter:
		location.x = offsetX + (target->size.width / 2 - size.width / 2);
		break;

	case Right:
		location.x = offsetX + target->size.width - size.width - spacing;
		break;

	case AfterRight:
		location.x = offsetX + target->size.width + spacing;
		break;
	}
}

void Control::AlignToY(
	Control* target,
	YAlignment alignment,
	float spacing,
	bool useParentLocation)
{
	float offsetY = target->location.y;

	if (!useParentLocation)
		offsetY = 0;

	switch (alignment)
	{
	case BeforeTop:
		location.y = offsetY + target->location.x - size.height - spacing;
		break;

	default:
	case Top:
		location.y = offsetY + spacing;
		break;

	case YCenter:
		location.y = offsetY + (target->size.height / 2 - size.height / 2);
		break;

	case Bottom:
		location.y = offsetY + target->size.height - size.height - spacing;
		break;

	case AfterBottom:
		location.y = offsetY + target->size.height + spacing;
		break;
	}
}

void Control::AlignTo(
	Control* target,
	Alignment alignment,
	float spacing,
	bool useParentLocation)
{
	AlignToX(target, alignment.x, spacing, useParentLocation);
	AlignToY(target, alignment.y, spacing, useParentLocation);
}

void Control::StretchInParent(
	StretchMode stretch,
	float spacing)
{
	switch (stretch.x)
	{
	case NoStretch:
		break;

	case Fill:
		AlignToX(parent, Left, spacing);
		SetSize(
			parent->size.width - spacing * 2,
			size.height
		);
	}

	switch (stretch.y)
	{
	case NoStretch:
		break;

	case Fill:
		AlignToY(parent, Top, spacing);
		SetSize(
			size.width,
			parent->size.height - spacing * 2
		);
	}
}

Control* Control::HitTestAt(
	int x, int y,
	DUIPoint& relativePos,
	HitTestParameters parameters)
{
	if (!Contains(x, y))
		return nullptr;

	if (!isVisible)
		return nullptr;

	if (parameters.skip_hit_test_invisible && !isHitTestVisible)
		return nullptr;

	if (parameters.should_skip != nullptr &&
			parameters.should_skip(this)) {
		return nullptr;
	}

	for (auto i = Children.rbegin();
		i != Children.rend(); i++)
	{
		float xOffset = 0;//scrollOffset.x;// + renderOffset.x;
		float yOffset = 0;//scrollOffset.y;// + renderOffset.y;

			Control* ctrl = (*i)->HitTestAt(
				x - location.x - xOffset,
				y - location.y - yOffset,
				relativePos, parameters
			);

		if (ctrl != nullptr)
			return ctrl;
	}

	relativePos = {
		(float)(x - location.x),
		(float)(y - location.y)
	};

	if (!ReturnsSelfOnHitTestMiss())
		return nullptr;

	return this;
}
