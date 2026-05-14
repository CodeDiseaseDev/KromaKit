//
// Created by code on 4/30/26.
//

#include <kromakit/gesture/scrolling/ScrollableView.h>
#include <kromakit/Control.h>

#include <algorithm>
#include <cmath>

void ScrollableView::Update(float deltaTime) {
  Control::Update(deltaTime);

  if (deltaTime > 0.0001f)
    deltaTimeCache = deltaTime;

  UpdateMomentum(deltaTime);
}

void ScrollableView::DoLayout(Graphics* g) {
  if (!content)
    return;

  DUISize viewport = GetSize();

  const DUISize desiredContentSize {
    std::max(viewport.width, contentSize.width),
    std::max(viewport.height, contentSize.height)
  };

  content->SetLayoutSize(desiredContentSize);

  ClampScrollOffset();

  content->SetLayoutLocation({
    -scrollOffset.x,
    -scrollOffset.y
  });
}

void ScrollableView::OnRender(Graphics* g) {
  // Content should render through the normal child-control pipeline.
  // Later: draw scrollbars here.
}

void ScrollableView::BeginDragGesture() {
  isDragging = true;
  gestureLocked = false;
  lockedAxis = ScrollAxis::Auto;

  if (horizontalEnabled) {
    momentumX.Begin();
  } else {
    momentumX.Stop();
  }

  if (verticalEnabled) {
    momentumY.Begin();
  } else {
    momentumY.Stop();
  }
}

DUIPoint ScrollableView::FilterFingerDeltaForAxisLock(const PointerEvent& e) {
  DUIPoint fingerDelta = e.delta;

  if (!horizontalEnabled) {
    fingerDelta.x = 0.0f;
  }

  if (!verticalEnabled) {
    fingerDelta.y = 0.0f;
  }

  if (!horizontalEnabled || !verticalEnabled) {
    return fingerDelta;
  }

  constexpr float axisLockThreshold = 8.0f;

  if (!gestureLocked) {
    const float totalAbsX = std::fabs(e.totalDelta.x);
    const float totalAbsY = std::fabs(e.totalDelta.y);

    if (totalAbsX > axisLockThreshold || totalAbsY > axisLockThreshold) {
      lockedAxis =
        (totalAbsX >= totalAbsY)
          ? ScrollAxis::Horizontal
          : ScrollAxis::Vertical;
      gestureLocked = true;
    }
  }

  if (!gestureLocked) {
    return fingerDelta;
  }

  if (lockedAxis == ScrollAxis::Horizontal) {
    fingerDelta.y = 0.0f;
  } else if (lockedAxis == ScrollAxis::Vertical) {
    fingerDelta.x = 0.0f;
  }

  return fingerDelta;
}

void ScrollableView::ApplyDragFromDelta(const DUIPoint& fingerDelta) {
  // const DUIPoint fingerDelta = FilterFingerDeltaForAxisLock(e);

  // if (std::fabs(fingerDelta.x) < 0.0001f &&
  //     std::fabs(fingerDelta.y) < 0.0001f) {
  //   return;
  // }

  constexpr float dragScale = 1.0f; // lower to slow content
  const DUIPoint scrollDelta {
    -fingerDelta.x * dragScale,
    -fingerDelta.y * dragScale
  };

  ScrollBy(scrollDelta);

  if (horizontalEnabled) {// && std::fabs(scrollDelta.x) > 0.0001f) {
    momentumX.Update(scrollDelta.x, deltaTimeCache);
  }

  if (verticalEnabled) {// && std::fabs(scrollDelta.y) > 0.0001f) {
    momentumY.Update(scrollDelta.y, deltaTimeCache);
  }
}

void ScrollableView::OnPointerDown(const PointerEvent& e) {
  Control::OnPointerDown(e);

  if (e.mouseButton != MouseButton::Left) return;

  BeginDragGesture();
}

void ScrollableView::OnPointerMove(const PointerEvent& e) {
  Control::OnPointerMove(e);

  if (e.mouseButton != MouseButton::Left) return;

  if (!isDragging)
    return;

  const auto delta = FilterFingerDeltaForAxisLock(e);
  ApplyDragFromDelta(delta);
}

void ScrollableView::OnPointerUp(const PointerEvent& e) {
  Control::OnPointerUp(e);

  if (e.mouseButton != MouseButton::Left) return;

  if (!isDragging) {
    return;
  }

  isDragging = false;
  gestureLocked = false;
  lockedAxis = ScrollAxis::Auto;

  EndScroll();
}

void ScrollableView::EndScroll() {
  if (horizontalEnabled) {
    momentumX.End();
  } else {
    momentumX.Stop();
  }

  if (verticalEnabled) {
    momentumY.End();
  } else {
    momentumY.Stop();
  }
}

void ScrollableView::OnPointerCancel(const PointerEvent& e) {
  Control::OnPointerCancel(e);

  isDragging = false;
  gestureLocked = false;
  lockedAxis = ScrollAxis::Auto;

  momentumX.Stop();
  momentumY.Stop();
}

void ScrollableView::OnScroll(const PointerEvent& e) {
  Control::OnScroll(e);

  // Avoid double-applying movement when both pointer-move and scroll callbacks
  // are emitted for the same gesture frame.
  if (isDragging) {
    return;
  }

  // Current DUIWindow gesture dispatch drives scrolling through this callback.
  // Treat the first event as drag-start so release can call End() once.

  BeginDragGesture();

  const auto delta = FilterFingerDeltaForAxisLock(e);
  ApplyDragFromDelta(delta);
}


// usually only desktop
void ScrollableView::OnWheelScroll(const WheelScrollEvent& e) {
  Control::OnWheelScroll(e);

  constexpr float wheelStepScale = 10.0f;
  const DUIPoint scrollDelta {
    -e.delta.x * wheelStepScale,
    -e.delta.y * wheelStepScale
  };

  if (std::fabs(scrollDelta.x) < 0.0001f &&
      std::fabs(scrollDelta.y) < 0.0001f) {
    return;
  }

  // Wheel input is not a pointer drag gesture.
  // Apply movement immediately, then seed momentum without touching
  // ScrollableView's drag-state machine.
  ScrollBy(scrollDelta);

  if (horizontalEnabled) {
    momentumX.Update(scrollDelta.x, deltaTimeCache);
    momentumX.End();
  } else {
    momentumX.Stop();
  }

  if (verticalEnabled) {
    momentumY.Update(scrollDelta.y, deltaTimeCache);
    momentumY.End();
  } else {
    momentumY.Stop();
  }
}

void ScrollableView::SetContent(Control* c) {
  if (content == c)
    return;

	if (content) {
		if (content->parent != nullptr) {
			content->parent->RemoveControl(content);
		}
	}

  content = c;

	if (content) {
		// Important: attach it to the actual DirectUI tree.
		AddBorrowedControl(content);
	}

  MarkLayoutDirty();
  MarkRenderTreeDirty();
}

void ScrollableView::SetHorizontalScrollEnabled(bool enabled) {
  if (horizontalEnabled == enabled)
    return;

  horizontalEnabled = enabled;
  ClampScrollOffset();
  MarkVisualDirty();
}

void ScrollableView::SetVerticalScrollEnabled(bool enabled) {
  if (verticalEnabled == enabled)
    return;

  verticalEnabled = enabled;
  ClampScrollOffset();
  MarkVisualDirty();
}

void ScrollableView::SetScrollOffset(DUIPoint offset) {
  const DUIPoint clamped = ClampOffset(offset);

  if (std::fabs(scrollOffset.x - clamped.x) < 0.001f &&
      std::fabs(scrollOffset.y - clamped.y) < 0.001f) {
    return;
  }

  scrollOffset = clamped;

  if (content) {
    content->SetLayoutLocation({
      -scrollOffset.x,
      -scrollOffset.y
    });
  }

  // Scrolling is visual/geometry movement only, not a full layout request.
  MarkVisualDirty();
}

DUIPoint ScrollableView::GetScrollOffset() const {
  return scrollOffset;
}

void ScrollableView::ScrollTo(DUIPoint offset, bool animated) {
  if (animated) {
    // v1: instant scroll. Keep the API for later animation support.
  }

  SetScrollOffset(offset);
}

void ScrollableView::ScrollBy(DUIPoint delta) {
  SetScrollOffset({
    scrollOffset.x + delta.x,
    scrollOffset.y + delta.y
  });
}

void ScrollableView::SetContentSize(DUISize size) {
  if (std::fabs(contentSize.width - size.width) < 0.001f &&
      std::fabs(contentSize.height - size.height) < 0.001f) {
    return;
  }

  contentSize = size;

  ClampScrollOffset();

  // Content size affects layout bounds/max scroll range.
  MarkLayoutDirty();
}

DUISize ScrollableView::GetContentSize() const {
  return contentSize;
}

void ScrollableView::ClampScrollOffset() {
  SetScrollOffset(scrollOffset);
}

DUIPoint ScrollableView::ClampOffset(DUIPoint offset) {
  DUISize viewport = GetSize();

  const float maxX = horizontalEnabled
    ? std::max(0.0f, contentSize.width - viewport.width)
    : 0.0f;

  const float maxY = verticalEnabled
    ? std::max(0.0f, contentSize.height - viewport.height)
    : 0.0f;

  return {
    std::clamp(offset.x, 0.0f, maxX),
    std::clamp(offset.y, 0.0f, maxY)
  };
}

DUIRect ScrollableView::GetViewportRect() const {
  return {
    location.x,
    location.y,
    size.width,
    size.height
  };
}

DUIRect ScrollableView::GetContentRect() const {
  return {
    location.x - scrollOffset.x,
    location.y - scrollOffset.y,
    contentSize.width,
    contentSize.height
  };
}

void ScrollableView::UpdateMomentum(float dt) {
  if (isDragging)
    return;

  const DUIPoint previousOffset = scrollOffset;
  DUIPoint extra {};

  if (horizontalEnabled) {
    extra.x = momentumX.Tick(dt);
  }

  if (verticalEnabled) {
    extra.y = momentumY.Tick(dt);
  }

  if (std::fabs(extra.x) < 0.0001f &&
      std::fabs(extra.y) < 0.0001f) {
    return;
  }

  ScrollBy(extra);

  if (horizontalEnabled &&
      std::fabs(scrollOffset.x - previousOffset.x) < 0.0001f &&
      std::fabs(extra.x) >= 0.0001f) {
    momentumX.Stop();
  }

  if (verticalEnabled &&
      std::fabs(scrollOffset.y - previousOffset.y) < 0.0001f &&
      std::fabs(extra.y) >= 0.0001f) {
    momentumY.Stop();
  }
}

bool ScrollableView::WantsScrollCapture() const {
  const DUISize viewport = {size.width, size.height};

  return (verticalEnabled && contentSize.height > viewport.height) ||
         (horizontalEnabled && contentSize.width > viewport.width);
}

bool ScrollableView::ReturnsSelfOnHitTestMiss() const {
  return WantsScrollCapture();
}

ScrollAxis ScrollableView::GetGestureDirectionPreference() const {
  if (horizontalEnabled && verticalEnabled) {
    return ScrollAxis::Free;
  }

  if (horizontalEnabled) {
    return ScrollAxis::Horizontal;
  }

  if (verticalEnabled) {
    return ScrollAxis::Vertical;
  }

  return ScrollAxis::Auto;
}
