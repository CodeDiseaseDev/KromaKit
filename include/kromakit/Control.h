#pragma once

#include <kromakit/Alignment.h>
#include <kromakit/Logging.h>
#include <kromakit/Rendering/RenderLayer.h>
#include <kromakit/Utils.h>
#include <kromakit/core/properties/DUIVisualProperty.h>
#include <kromakit/core/properties/control_property_registry/PropertyRegistry.h>
#include <kromakit/event_types/PointerEvent.h>
#include <kromakit/event_types/WheelScrollEvent.h>
#include <kromakit/gesture/scrolling/ScrollAxis.h>
#include <kromakit/graphics/Graphics.h>
#include <kromakit/layout/LayoutResult.h>
#include <kromakit/math/MathConstants.h>
#include <kromakit/styling/DUIStyleName.h>
#include <kromakit/styling/DUIStyleRegistry.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include <kromakit/animation/FloatEaseAnimation.h>
#include <kromakit/event_types/keyboard/DUIKeyEvent.h>
#include <kromakit/event_types/keyboard/DUITextInputEvent.h>


#define DUI_DEBUG_IF_FORCE(cond)  ((cond) || true)
#define DUI_DEBUG_IF_BYPASS(cond) ((cond) && false)

#define PtrClassName(ptrClass) typeid(*ptrClass).name()


inline constexpr double radians(double x) {
  return x * (MathConstants::Pi / 180.0);
}

inline constexpr double map(
  double x,
  double fl,
  double fh,
  double tl,
  double th
) {
  return (x - fl) * (th - tl) / (fh - fl) + tl;
}

inline constexpr double clamp(
  double x,
  double min,
  double max
) {
  if (x > max)
    return max;

  if (x < min)
    return min;

  return x;
}

inline constexpr double mapc(
  double x,
  double fl,
  double fh,
  double tl,
  double th
) {
  return map(
    clamp(x, fl, fh),
    fl,
    fh,
    tl,
    th);
}


class ContextMenu;
class ContextMenuAttachment;
class DUIWindow;
class GaussianBlurProvider;

struct RenderItem;

template <class T>
void Wrap(T* ctrl, std::function<void(T*)> func) {
  if (func != nullptr) {
    func(ctrl);
  }
}

class Control : public IPropertyListener {
public:
  struct HitTestParameters {
    bool skip_hit_test_invisible = true;
    std::function<bool(Control*)> should_skip = nullptr;
  };

  using ContextMenuProvider = std::function<std::unique_ptr<ContextMenu>()>;

public:
  Control();
  virtual ~Control();

  // ---------------------------------------------------------------------------
  // Rendering / update / layout
  // ---------------------------------------------------------------------------

  virtual void OnRender(Graphics* rendTarget) = 0;
  virtual void OnPostRender(Graphics* rendTarget);
  virtual void OnDebugOverlayRender(Graphics* rendTarget);

  virtual void Update(float deltaTime);
  void UpdateTree(float deltaTime);

  virtual void DoLayout(Graphics* renderTarget) = 0;
  virtual LayoutResult DoLayout2(Graphics* renderTarget);

  void InternalDoLayout(Graphics* renderTarget);
  LayoutResult InternalDoLayout2(Graphics* renderTarget);

  void DoFullLayout(Graphics* g);
  LayoutResult DoFullLayout2(Graphics* g);

  LayoutResult LayoutDirtySubtree(
    Graphics* g,
    bool forceLayout = false
  );

  bool StabilizeLayout(Graphics* rendTarget);
  void RenderRoot(Graphics* rendTarget);

  void BuildRenderList(
    std::vector<RenderItem>& list,
    DUIPoint                 parentScreenPos,
    DUIRect                  parentClip,
    float                    parentOpacity = 1.0f,
    DUIPoint                 parentVisualOffset = {0, 0},
    RenderLayer              inheritedLayer = RenderLayer::Content,
    uint32_t*                traversalCounter = nullptr,
    float                    inheritedCentredScale = 1.0f,
    DUIPoint                 inheritedCentredScaleOrigin = {0, 0}
  );

  void RenderListCulling(
    std::vector<RenderItem>& list,
    DUISize                  viewportSize
  );

  void RenderFromList(
    Graphics*                rendTarget,
    std::vector<RenderItem>& list,
    bool                     allow_translate,
    bool                     debug_overlay
  );

  // ---------------------------------------------------------------------------
  // Geometry / bounds / layout state
  // ---------------------------------------------------------------------------

  bool UsesRoundedCorners() const;

  DUIRect GetRectF();
  DUIRect GetBounds() const;
  DUISize GetSize();

  virtual DUISize Measure(DUISize available) const {
    return size;
  }

  bool SetLayoutLocation(DUIPoint nextLocation);
  bool SetLayoutSize(DUISize nextSize);

  bool SetLayoutFrame(
    DUIPoint nextLocation,
    DUISize  nextSize
  );

  bool SetLayoutContentSize(DUISize nextContentLayoutSize);

  void SetCentredScale(float scale);
  float GetCentredScale() const;

  virtual void SetSize(
    float w,
    float h
  );

  void SetContent(std::string wStr);

  bool IsOutOfBounds();

  bool Contains(
    float x,
    float y
  );

  bool IsFullyCovered(
    const DUIRect&            rect,
    const std::vector<DUIRect>& regions
  );

  bool IsInView(
    const DUIRect& rect,
    DUISize        viewportSize
  );

  bool IsFullyOpaque() const;
  float GetFinalOpacity() const;

  // ---------------------------------------------------------------------------
  // Dirty flags / invalidation
  // ---------------------------------------------------------------------------

  void MarkRenderTreeDirty();
  void MarkRenderOrderDirty();
  void MarkVisualDirty();
  void MarkLayoutDirty();

  uint64_t GetRenderTreeVersion() const;
  uint64_t GetRenderOrderVersion() const;
  uint64_t GetVisualVersion() const;
  uint64_t GetLayoutVersion() const;

  // ---------------------------------------------------------------------------
  // Styling / visual state
  // ---------------------------------------------------------------------------

  bool UseStyle(const DUIStyleName& name);
  const DUIStyleName& GetStyleName() const;

  void SetFont(DUIFont font);

  void SetRenderLayer(RenderLayer layer);
  void SetVisible(bool visible);
  void SetOpacity(float nextOpacity);
  void SetZIndex(int nextZIndex);

  // ---------------------------------------------------------------------------
  // Hit testing / input
  // ---------------------------------------------------------------------------

  Control* HitTestAt(
    int               x,
    int               y,
    DUIPoint&         relativePos,
    HitTestParameters parameters
  );

  virtual void OnPointerMove(const PointerEvent& e);
  virtual void OnPointerDown(const PointerEvent& e);
  virtual void OnPointerUp(const PointerEvent& e);
  virtual void OnPointerCancel(const PointerEvent& e);

  virtual void OnScroll(const PointerEvent& e);
  virtual void OnWheelScroll(const WheelScrollEvent& e);

  virtual void OnMouseIn();
  virtual void OnMouseOut();

  [[deprecated("Use pointer events instead")]]
  virtual void OnMouseDrag(
    float deltaX,
    float deltaY
  );

  [[deprecated("Use pointer events instead")]]
  virtual void OnMouseMove(
    int x,
    int y
  );

  [[deprecated("Use pointer events instead")]]
  virtual void OnMouseMoveGlobal(
    int gx,
    int gy
  );

  [[deprecated("Use pointer events instead")]]
  virtual void OnMouseDown();

  [[deprecated("Use pointer events instead")]]
  virtual void OnMouseUp();

  [[deprecated("Use pointer or wheel scroll events instead")]]
  virtual void OnScroll(
    float deltaX,
    float deltaY
  );

  // ---------------------------------------------------------------------------
  // Keyboard input
  // ---------------------------------------------------------------------------
  virtual void OnKeyDown(const DUIKeyEvent &keyEvent);
  virtual void OnKeyUp(const DUIKeyEvent &keyEvent);
  virtual void OnTextInput(const DUITextInputEvent &textInputEvent);

  virtual bool HandleShortcut(const DUIKeyEvent &keyEvent);

  [[deprecated]]
  virtual void OnKeyDown(int key);

  [[deprecated]]
  virtual void OnKeyUp(int key);

  // ---------------------------------------------------------------------------
  // Context menus
  // ---------------------------------------------------------------------------

  void SetContextMenuProvider(ContextMenuProvider provider);
  void ClearContextMenuProvider();

  bool HasContextMenuProvider() const;

  void SetContextMenuHoldThresholdMs(int milliseconds);
  int GetContextMenuHoldThresholdMs() const;

  bool BeginContextMenuHoldTracking();
  bool TryConsumeContextMenuHoldIfReady();

  void EndContextMenuHoldTracking();
  void CancelContextMenuHoldTracking();

  std::unique_ptr<ContextMenu> CreateContextMenuFromProvider() const;

  bool TryToPresentContextMenu();

  static void ContextMenuHapticFeedback();

  // ---------------------------------------------------------------------------
  // Behaviour flags / virtual policy hooks
  // ---------------------------------------------------------------------------

  virtual bool IsDraggable() const {
    return false;
  }

  virtual bool WantsDragCapture() const {
    return false;
  }

  virtual bool WantsScrollCapture() const {
    return false;
  }

  virtual bool WantsClickCapture() const {
    return false;
  }

  virtual bool ReturnsSelfOnHitTestMiss() const {
    return true;
  }

  virtual bool ShouldClipToBounds() {
    return true;
  }

  virtual bool RequiresBackdropBlur() const {
    return false;
  }

  virtual RenderLayer GetRenderLayer() const {
    return renderLayer;
  }

  virtual bool NeedsLayoutWhenOwnSizeChanges() const {
    return false;
  }

  virtual bool ShouldLayoutChildrenAfterOwnLayout() const {
    return true;
  }

  virtual ScrollAxis GetGestureDirectionPreference() const {
    return gestureDirectionPreference;
  }

  // ---------------------------------------------------------------------------
  // Alignment helpers
  // ---------------------------------------------------------------------------

  void AlignToX(
    Control*   target,
    XAlignment alignment,
    float      spacing = 0.0f,
    bool       useParentLocation = true
  );

  void AlignToY(
    Control*   target,
    YAlignment alignment,
    float      spacing = 0.0f,
    bool       useParentLocation = true
  );

  void AlignTo(
    Control*  target,
    Alignment alignment,
    float     spacing = 0.0f,
    bool      useParentLocation = true
  );

  void StretchInParent(
    StretchMode stretch,
    float       spacing = 0.0f
  );

  // ---------------------------------------------------------------------------
  // Control hierarchy
  // ---------------------------------------------------------------------------

  template<typename T, typename... Args>
  requires std::is_base_of_v<Control, T>
  T* CreateOwnedControl(Args&&... args) {
    auto control = std::make_shared<T>(
      std::forward<Args>(args)...);

    T* raw = control.get();
    raw->owner = this;

    ownedChildren.push_back(control);

    return raw;
  }

  template<typename T, typename... Args>
  requires std::is_base_of_v<Control, T>
  T* CreateControl(Args&&... args) {
    T* raw = CreateOwnedControl<T>(
      std::forward<Args>(args)...);

    AttachControl(raw);

    return raw;
  }

  template<typename T>
  requires std::is_base_of_v<Control, T>
  T* AddControl(std::unique_ptr<T> control) {
    if (control == nullptr) {
      Logging::Log("AddControl received a nullptr control parameter.\n");
      return nullptr;
    }

    T* raw = control.get();

    if (raw->owner != nullptr && raw->owner != this) {
      Logging::Log(
        "AddControl refused to take ownership of '%s' because it is already owned by '%s'.\n",
        raw->GetControlName().value_or("<name unset>").c_str(),
        raw->owner->GetControlName().value_or("<name unset>").c_str());

      return nullptr;
    }

    raw->owner = this;

    ownedChildren.emplace_back(std::move(control));
    AttachControl(raw);

    return raw;
  }

  virtual void AddBorrowedControl(Control* control) {
    if (control == nullptr) {
      Logging::Log("AddBorrowedControl received a nullptr control parameter.\n");
      return;
    }

    // Borrowed controls must not be owned by a different parent that would
    // destroy them on RemoveControl().
    if (
      control->parent != nullptr &&
      control->parent != this &&
      control->owner == control->parent
    ) {
      Logging::Log(
        "AddBorrowedControl refused to reparent '%s' because it is owned by its current parent '%s'.\n",
        control->GetControlName().value_or("<name unset>").c_str(),
        control->parent->GetControlName().value_or("<name unset>").c_str());

      return;
    }

    if (
      control->parent != nullptr &&
      control->parent != this
    ) {
      control->parent->RemoveControl(control);
    }

    const bool exists = std::any_of(
      Children.begin(),
      Children.end(),
      [control](const Control* child) {
        return child == control;
      });

    if (exists)
      return;

    control->parent = this;
    control->OnUsed();

    Children.push_back(control);
    MarkLayoutDirty();

    if (hasLoaded) {
      control->InternalOnLoad();
    }
  }

  /// LEGACY, NOT TO BE USED.
  void AddControl(Control* control) = delete;

  void AttachControl(Control* raw) {
    if (raw == nullptr) {
      Logging::Log("AttachControl received a nullptr control parameter.\n");
      return;
    }

    if (raw->owner != this) {
      Logging::Log(
        "AttachControl refused to attach control '%s': owner=%s parent=%s this=%s.\n",
        raw->GetControlName().value_or("<name unset>").c_str(),
        raw->owner ? raw->owner->GetControlName().value_or("<name unset>").c_str() : "(null)",
        raw->parent ? raw->parent->GetControlName().value_or("<name unset>").c_str() : "(null)",
        this->GetControlName().value_or("<name unset>").c_str());

      return;
    }

    if (
      raw->parent != nullptr &&
      raw->parent != this
    ) {
      raw->parent->RemoveControl(raw);
    }

    const bool exists = std::any_of(
      Children.begin(),
      Children.end(),
      [raw](const Control* child) {
        return child == raw;
      });

    if (exists)
      return;

    raw->parent = this;
    raw->OnUsed();

    Children.push_back(raw);
    MarkLayoutDirty();

    if (hasLoaded) {
      raw->InternalOnLoad();
    }
  }

  void RemoveControl(Control* control);
  void ClearControls();

  virtual void OnUsed();
  virtual DUIWindow* GetRootWindow();

  // ---------------------------------------------------------------------------
  // Lifecycle / selection / properties
  // ---------------------------------------------------------------------------

  void TriggerOnWindowLoaded();

  void OnPropertyChanged(IDUIProperty* property) override;

  virtual void OnIsSelectedChanged();

  // ---------------------------------------------------------------------------
  // Debug
  // ---------------------------------------------------------------------------

  virtual const std::vector<std::string> GetDebugOverlayItems() { return {}; };
  virtual std::optional<std::string> GetControlName() const;

  bool SetDebugOverlayVisible(bool visible);

  void PrintDebugTree();

public:
  // ---------------------------------------------------------------------------
  // Public state
  // ---------------------------------------------------------------------------

  int ControlID = 0;

  bool hasLoaded = false;
  bool debug_overlay_enabled = false;

  bool useHoverBackground = false;

  bool __debug__devtools_highlighted = false;
  Color __debug__devtools_highlighted_color = ColorFromRGB(0);

  std::vector<std::shared_ptr<Control>> ownedChildren;
  std::vector<Control*> Children;

  PropertyRegistry propRegistry = PropertyRegistry(this);

  DUISize size;
  DUIPoint location = {0, 0};

  DUIInsets cornerRadius = {0, 0, 0, 0};
  DUIInsets outlineThickness = DUIInsets(0);

  [[deprecated("use `Control::padding` instead")]]
  DUIInsets backgroundPadding = {0, 0, 0, 0};

  [[deprecated("use `Control::margin` instead")]]
  DUIInsets layoutMargin = {5, 5, 5, 5};

  DUIInsets margin = DUIInsets(0);
  DUIInsets padding = DUIInsets(0);



  Color hoverBackground = {.5,.5,.5,1};
  Color background = {0, 0, 0, 0};
  Color foreground = {1, 1, 1, 1};
  Color outline    = {.2, .2, .2};

  DUIVisualProperty<float, "render_scale"> render_scale =
    propRegistry.NewVisualProperty<float, "render_scale">(1.0f);

  DUIVisualProperty<bool, "debug_x_render_"> debug_x_render_ =
    propRegistry.NewVisualProperty<bool, "debug_x_render_">(false);

  DUIPoint cursorHoverLocation = {0, 0};
  DUIPoint cachedScreenLocation = {-1, -1};

  DUIPoint scrollOffset = {0, 0};
  DUIPoint renderOffset = {0, 0};

  DUISize contentLayoutSize = {0, 0};

  bool isLayoutDirty = true;
  bool layoutDirtySelf = true;

  float opacity = 1.0f;
  int zIndex = 0;

  RenderLayer renderLayer = RenderLayer::Content;

  Control* parent = nullptr;
  Control* owner = nullptr;

  bool isMouseDown = false;
  bool isMouseHovering = false;
  bool isSelected = false;

  bool isVisible = true;
  bool isHitTestVisible = true;

  DUIFont font = DUIFont("default");

  ScrollAxis gestureDirectionPreference = ScrollAxis::Auto;

  std::string sContent = "";

private:
  DUIStyleName styleName;
  float centredScale = 1.0f;

  void PrintDebugTreeRecursive(
    const std::string& prefix,
    bool               isLast,
    bool               isRoot
  );

  void AssignOwnID();

  bool IsControlInView_(
    Control* ctrl,
    Graphics* graphics,
    DUIPoint location
  );

protected:
  // To be used only by the Control.cpp class. Use OnRender instead.
  std::unique_ptr<ContextMenuAttachment> contextMenuAttachment_;

  virtual void OnLoad();
  void InternalOnLoad();
};
