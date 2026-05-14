//
// Created by code on 4/30/26.
//

#ifndef TEST_IOS_APP_SCROLLABLEVIEW_H
#define TEST_IOS_APP_SCROLLABLEVIEW_H

#include <kromakit/gesture/scrolling/ScrollAxis.h>
#include <kromakit/gesture/scrolling/SwipeMomentum.h>
#include <kromakit/graphics/Graphics.h>
#include <kromakit/Control.h>
#include <kromakit/event_types/PointerEvent.h>

class ScrollableView : public Control {
public:
  void SetContent(Control* c);

  void SetHorizontalScrollEnabled(bool enabled);
  void SetVerticalScrollEnabled(bool enabled);

  void SetScrollOffset(DUIPoint offset);
  DUIPoint GetScrollOffset() const;

  void ScrollTo(DUIPoint offset, bool animated = false);
  void ScrollBy(DUIPoint delta);

  void SetContentSize(DUISize size);
  DUISize GetContentSize() const;

  void Update(float deltaTime) override;
  void DoLayout(Graphics *renderTarget) override;
  void OnRender(Graphics *rendTarget) override;

  void OnPointerMove(const PointerEvent& e) override;
  void OnPointerDown(const PointerEvent& e) override;
  void OnPointerUp(const PointerEvent& e) override;

  void EndScroll();

  void OnPointerCancel(const PointerEvent& e) override;
  void OnScroll(const PointerEvent& e) override;
  void OnWheelScroll(const WheelScrollEvent& e) override;

  // deprecated as of now.
  // void OnMouseDown() override;
  // void OnMouseMove(int x, int y) override;
  // void OnMouseUp() override;
  // void OnScroll(float deltaX, float deltaY) override;

protected:
  Control* content = nullptr;

  // DUIPoint scrollOffset {};
  DUISize contentSize {};

  bool horizontalEnabled = false;
  bool verticalEnabled = true;

  SwipeMomentum momentumX;
  SwipeMomentum momentumY;

  bool isDragging = false;
  bool gestureLocked = false;
  ScrollAxis lockedAxis = ScrollAxis::Auto;

  float deltaTimeCache = 1.0f / 60.0f;

  void BeginDragGesture();
  DUIPoint FilterFingerDeltaForAxisLock(const PointerEvent& e);
  void ApplyDragFromDelta(const DUIPoint &fingerDelta);

  void ClampScrollOffset();
  DUIPoint ClampOffset(DUIPoint offset);

  DUIRect GetViewportRect() const;
  DUIRect GetContentRect() const;

  void UpdateMomentum(float dt);

public:
  const std::vector<std::string> GetDebugOverlayItems() override { return {}; }
  std::optional<std::string> GetControlName() const override { return "ScrollableView"; }

  ScrollAxis GetGestureDirectionPreference() const override;
  bool WantsScrollCapture() const override;
  bool ReturnsSelfOnHitTestMiss() const override;
};



#endif //TEST_IOS_APP_SCROLLABLEVIEW_H
