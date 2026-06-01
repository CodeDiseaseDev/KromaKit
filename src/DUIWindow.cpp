
#include <kromakit/DUIWindow.h>
#include <kromakit/Application.h>
#include <kromakit/Rendering/RenderPipeline.h>
#include <kromakit/context_menu/ContextMenu.h>

#include <algorithm>
#include <chrono>
#include <cmath>

#include <kromakit/Utils.h>

namespace {
constexpr float kContextMenuHoldCancelSlopPx = 8.0f;

bool IsControlInRemovedSubtree(Control* candidate, Control* removedRoot) {
  if (candidate == nullptr || removedRoot == nullptr)
    return false;

  for (Control* node = candidate; node != nullptr; node = node->parent) {
    if (node == removedRoot)
      return true;
  }

  return false;
}
}

inline void errorcb(int error, const char* desc)
{
  Logging::Log("GLFW error %d: %s\n", error, desc);
}

void DUIWindow::ConfigureOverlayBackgroundLayer(
  OverlayBackgroundLayer* backgroundLayer,
  IOverlayControl* overlayControl) {

  if (backgroundLayer == nullptr ||
      overlayControl == nullptr) {
    return;
  }

  auto& options = backgroundLayer->GetPresentationOptions();

  options.dismissOnBackgroundTap =
    overlayControl->ShouldDismissOnBackgroundTap();

  options.capturesBackdropClicks = true;
  options.dimBackground = true;
  options.blurBackground = false;
  options.backdropZoomOutEffect = true;
}

MessageBox* DUIWindow::ShowMessageBox(MessageBoxOptions options) {
  auto* backgroundLayer =
    overlayStack->CreateControl<OverlayBackgroundLayer>();

  auto* messageBox =
    backgroundLayer->CreateControl<MessageBox>();

  messageBox->SetOptions(std::move(options));

  ConfigureOverlayBackgroundLayer(backgroundLayer, messageBox);

  backgroundLayer->AddOverlayControl(messageBox);
  backgroundLayer->Present();

  MarkLayoutDirty();

  return messageBox;
}

MessageBox* DUIWindow::ShowInfo(
  std::string title,
  std::string message) {

  return ShowMessageBox(MessageBox::Info(
    std::move(title),
    std::move(message)
  ));
}

MessageBox* DUIWindow::ShowError(
  std::string title,
  std::string message,
  std::string details) {

  return ShowMessageBox(MessageBox::Error(
    std::move(title),
    std::move(message),
    std::move(details)
  ));
}

MessageBox* DUIWindow::ShowConfirm(
  std::string title,
  std::string message,
  std::function<void(bool)> onResult) {

  return ShowMessageBox(MessageBox::Confirm(
    std::move(title),
    std::move(message),
    std::move(onResult)
  ));
}

void DUIWindow::MarkRenderTreeDirty() {
  rendInvalidation.Modify([&](RenderInvalidationState& ris) {
    ++ris.renderTreeVersion;
    ++ris.visualVersion;
  });
}

void DUIWindow::MarkRenderOrderDirty() {
  rendInvalidation.Modify([&](RenderInvalidationState& ris) {
    ++ris.renderOrderVersion;
    ++ris.visualVersion;
  });
}

void DUIWindow::MarkVisualDirty() {
  rendInvalidation.Modify([&](RenderInvalidationState& ris) {
    ++ris.visualVersion;
  });
}

void DUIWindow::MarkLayoutVersionDirty() {
  if (rendInvalidation->layoutDirty)
    return;

  rendInvalidation.Modify([&](RenderInvalidationState& ris) {
    ris.layoutDirty = true;
    ris.renderTreeDirty = true;
    ris.visualDirty = true;

    ++ris.layoutVersion;
    ++ris.renderTreeVersion;
    ++ris.visualVersion;
  });
}

uint64_t DUIWindow::GetRenderTreeVersion() const {
  return rendInvalidation->renderTreeVersion;
}

uint64_t DUIWindow::GetRenderOrderVersion() const {
  return rendInvalidation->renderOrderVersion;
}

uint64_t DUIWindow::GetVisualVersion() const {
  return rendInvalidation->visualVersion;
}

uint64_t DUIWindow::GetLayoutVersion() const {
  return rendInvalidation->layoutVersion;
}

void DUIWindow::CloseWindow() {
  isClosed = true;
  window = nullptr;
}

void DUIWindow::ResetContextMenuHoldTracking() {
  activeContextMenuHoldTarget = nullptr;
  contextMenuHoldFired = false;
  contextMenuHoldStartPoint = {0, 0};
}

void DUIWindow::BeginContextMenuHoldIfAvailable(Control* target) {
  ResetContextMenuHoldTracking();

  if (target == nullptr)
    return;

  if (!target->BeginContextMenuHoldTracking())
    return;

  activeContextMenuHoldTarget = target;
  contextMenuHoldStartPoint = cursorLocation;
}

void DUIWindow::CancelContextMenuHoldTracking() {
  if (activeContextMenuHoldTarget != nullptr) {
    activeContextMenuHoldTarget->CancelContextMenuHoldTracking();
  }

  ResetContextMenuHoldTracking();
}

bool DUIWindow::TryPresentContextMenuForTarget(Control* target) {
  if (target == nullptr)
    return false;

  auto menu = target->CreateContextMenuFromProvider();
  if (!menu)
    return false;

  if (overlayStack == nullptr)
    return false;

  TeardownActiveHoldContextMenuPresentation();

  const float maxX = std::max(0.0f, size.width - menu->size.width);
  const float maxY = std::max(0.0f, size.height - menu->size.height);

  menu->location = {
    std::clamp(cursorLocation.x, 0.0f, maxX),
    std::clamp(cursorLocation.y, 0.0f, maxY)
  };

  activeHoldContextMenu = std::shared_ptr<ContextMenu>(std::move(menu));
  activeHoldContextMenuLayer =
    std::make_unique<OverlayBackgroundLayer>(activeHoldContextMenu.get());

  overlayStack->PushBorrowedLayer(activeHoldContextMenuLayer.get());
  activeHoldContextMenuLayer->Present();

  Control::ContextMenuHapticFeedback();

  return true;
}

void DUIWindow::TeardownActiveHoldContextMenuPresentation() {
  if (activeHoldContextMenuLayer != nullptr) {
    if (activeHoldContextMenuLayer->parent != nullptr) {
      activeHoldContextMenuLayer->parent->RemoveControl(activeHoldContextMenuLayer.get());
    }

    activeHoldContextMenuLayer->ClearControls();
    activeHoldContextMenuLayer.reset();
  }

  activeHoldContextMenu.reset();
}

void DUIWindow::UpdateContextMenuHoldState() {
  if (activeContextMenuHoldTarget == nullptr || contextMenuHoldFired) {
    return;
  }

  if (!activeContextMenuHoldTarget->HasContextMenuProvider()) {
    CancelContextMenuHoldTracking();
    return;
  }

  if (activeGestureOwner != nullptr) {
    CancelContextMenuHoldTracking();
    return;
  }

  const float deltaX = std::fabs(cursorLocation.x - contextMenuHoldStartPoint.x);
  const float deltaY = std::fabs(cursorLocation.y - contextMenuHoldStartPoint.y);
  if (deltaX > kContextMenuHoldCancelSlopPx ||
      deltaY > kContextMenuHoldCancelSlopPx) {
    CancelContextMenuHoldTracking();
    return;
  }

  if (!activeContextMenuHoldTarget->TryConsumeContextMenuHoldIfReady()) {
    return;
  }

  contextMenuHoldFired = true;
  if (mouseHeldControl == activeContextMenuHoldTarget) {
    mouseHeldControl->isMouseDown = false;
  }

  (void)TryPresentContextMenuForTarget(activeContextMenuHoldTarget);
}

void DUIWindow::create_render_target(NVGcontext* renderTarget)
{
    if (graphics != nullptr) {
        delete graphics;
        graphics = nullptr;
    }

    if (renderTarget == nullptr) {
        Logging::Log("Could not init nanovg.\n");
        return;
    }

    graphics = new Graphics(renderTarget);
	if (!renderPipeline) {
		renderPipeline = std::make_unique<RenderPipeline>();
	}

}

std::optional<std::string> DUIWindow::GetControlName() const {
  return "Window";
}

const std::vector<std::string> DUIWindow::GetDebugOverlayItems() {
  return {};
}

DUIWindow::~DUIWindow() {
  if (graphics != nullptr) {
    delete graphics;
    graphics = nullptr;
  }
  // nvgDeleteGLES2();
}

DUIWindow::DUIWindow() {
  //
  // IMPORTANT:
  // DUIWindow::AddControl redirects user controls into contentStack.
  // Use Control::AddControl here to add the window's internal root layers
  // directly to DUIWindow itself.
  //
  // Control::AddControl(&GetContentStack());
  // Control::AddControl(&GetOverlayStack());
  contentStack = Control::CreateControl<ContentStack>();
  overlayStack = Control::CreateControl<OverlayStack>();
  /*   ^   important*/

  rootStackCoordinator.Attach(contentStack, overlayStack);
}

void DUIWindow::SetSafeArea(
  float left, float top,
  float right, float bottom) {

  safeArea = {
    left, top, // x, y

    size.width - left - right,
    size.height - top - bottom
  };

  MarkLayoutDirty();
}


void DUIWindow::GenerateWindow(
  const char* title)
{
}

void DUIWindow::Show()
{
  this->TriggerOnWindowLoaded();

  throw std::exception();

}

void DUIWindow::SetSize(
  float width, float height)
{
  Control::SetSize(width, height);

  GetContentStack()->SetLayoutSize(size);
  GetOverlayStack()->SetLayoutSize(size);

  // don't double layout.
  // MarkLayoutDirty();
}

void DUIWindow::SetLocation(int x, int y)
{
  throw std::exception();
}

void DUIWindow::Rerender()
{
  // WindowRender();
}

DUIWindow *DUIWindow::GetRootWindow()
{
  return this;
}

void DUIWindow::SetTitle(const char* title)
{
  throw std::exception();
}

DeveloperTools* DUIWindow::StartDeveloperToolsInstance()
{
  throw std::exception();
}

void DUIWindow::CloseCurrentDeveloperToolsInstance()
{
}

Control* DUIWindow::SolveScrollTarget(
  Control* ctrl) {

  if (ctrl == nullptr)
    return nullptr;


  if (ctrl->WantsScrollCapture()) {
    return ctrl;
  }

  return SolveScrollTarget(ctrl->parent);
}

Control* DUIWindow::SolveClickTarget(Control *ctrl) {
  if (ctrl == nullptr)
    return nullptr;


  if (ctrl->WantsClickCapture()) {
    return ctrl;
  }

  return SolveClickTarget(ctrl->parent);
}

Control* DUIWindow::SolveContextMenuTarget(Control* ctrl) const {
  for (Control* node = ctrl; node != nullptr; node = node->parent) {
    if (node == (Control*)this) {
      break;
    }

    if (node->HasContextMenuProvider()) {
      return node;
    }
  }

  return nullptr;
}

void DUIWindow::OnControlRemoved(Control* removedRoot) {
  if (removedRoot == nullptr)
    return;

  if (IsControlInRemovedSubtree(hoverControl, removedRoot)) {
    hoverControl = nullptr;
  }

  if (IsControlInRemovedSubtree(selectedControl, removedRoot)) {
    selectedControl = nullptr;
  }

  if (IsControlInRemovedSubtree(mouseHeldControl, removedRoot)) {
    mouseHeldControl = nullptr;
  }

  if (IsControlInRemovedSubtree(activeGestureOwner, removedRoot)) {
    activeGestureOwner = nullptr;
    activeGestureIsDrag = false;
    gestureLock = GestureDirection::None;
    totalDelta = {0, 0};
  }

  if (IsControlInRemovedSubtree(activeContextMenuHoldTarget, removedRoot)) {
    CancelContextMenuHoldTracking();
  }

  if (mouseHeldControl == nullptr) {
    activeGestureOwner = nullptr;
    activeGestureIsDrag = false;
    gestureLock = GestureDirection::None;
    totalDelta = {0, 0};
  }
}

void DUIWindow::UpdateCursor(int x, int y)
{
  cursorLocation = { (float)x, (float)y };
  gestureCurrentPoint = cursorLocation;
}

void DUIWindow::UpdateHover(Control* ctrl, DUIPoint relativeLocation)
{
  if (ctrl != hoverControl)
  {
    if (hoverControl)
    {
      hoverControl->cursorHoverLocation = { -1, -1 };
      hoverControl->isMouseHovering = false;
      hoverControl->OnMouseOut();
    }

    hoverControl = ctrl;

    if (ctrl)
    {
      ctrl->isMouseHovering = true;
      ctrl->cursorHoverLocation = relativeLocation;
      ctrl->OnMouseIn();
    }
  }
  else if (ctrl)
  {
    ctrl->cursorHoverLocation = relativeLocation;
  }
}

PointerEvent DUIWindow::BuildPointerEventForTarget(
    Control* target,
    int x,
    int y,
    float dx,
    float dy,
    MouseButton mouseButton) const
{
  PointerEvent e {};

  e.globalPosition = {
    static_cast<float>(x),
    static_cast<float>(y)
  };

  e.previousGlobalPosition = {
    static_cast<float>(x) - dx,
    static_cast<float>(y) - dy
  };

  e.startGlobalPosition = gestureStartPoint;

  e.position = e.globalPosition;

  if (target && target->cachedScreenLocation.x != -1) {
    e.position = {
      e.globalPosition.x - target->cachedScreenLocation.x,
      e.globalPosition.y - target->cachedScreenLocation.y
    };
  }

  e.previousPosition = {
    e.position.x - dx,
    e.position.y - dy
  };

  const DUIPoint globalDeltaFromStart {
    e.globalPosition.x - e.startGlobalPosition.x,
    e.globalPosition.y - e.startGlobalPosition.y
  };

  e.startPosition = {
    e.position.x - globalDeltaFromStart.x,
    e.position.y - globalDeltaFromStart.y
  };

  e.delta = { dx, dy };

  e.totalDelta = {
    e.position.x - e.startPosition.x,
    e.position.y - e.startPosition.y
  };

  e.pointerId = 0;
  e.isPrimary = true;
  e.mouseButton = mouseButton;

  return e;
}

WheelScrollEvent DUIWindow::BuildWheelScrollEventForTarget(
    Control* target,
    float deltaX,
    float deltaY,
    bool isPrecise) const
{
  WheelScrollEvent e {};

  e.globalPosition = cursorLocation;
  e.position = e.globalPosition;

  if (target && target->cachedScreenLocation.x != -1) {
    e.position = {
      e.globalPosition.x - target->cachedScreenLocation.x,
      e.globalPosition.y - target->cachedScreenLocation.y
    };
  }

  e.delta = { deltaX, deltaY };
  e.isPrecise = isPrecise;

  return e;
}

void DUIWindow::UpdateGestureState(float deltaX, float deltaY)
{
    totalDelta.x += deltaX;
    totalDelta.y += deltaY;

    if (!gestureStartControl)
        return;

    const float threshold = 8.0f;

    // 🔒 ALWAYS use gestureStartControl
    Control* dragCandidate = nullptr;
    if (gestureStartControl->IsDraggable() &&
        gestureStartControl->WantsDragCapture())
    {
        dragCandidate = gestureStartControl;
    }
    else
    {
        Control* hit = SolveClickTarget(gestureStartControl);
        if (hit && hit->IsDraggable() && hit->WantsDragCapture())
            dragCandidate = hit;
    }

    Control* scrollCandidate = SolveScrollTarget(gestureStartControl);



    // 🧠 Lock direction
    if (gestureLock == GestureDirection::None)
    {
        DUIPoint lockDelta = {
            gestureCurrentPoint.x - gestureStartPoint.x,
            gestureCurrentPoint.y - gestureStartPoint.y
        };

        if (std::abs(lockDelta.x) > threshold ||
            std::abs(lockDelta.y) > threshold)
        {
            float absX = std::abs(lockDelta.x);
            float absY = std::abs(lockDelta.y);

            GestureDirection dominant =
                (absX > absY)
                ? GestureDirection::Horizontal
                : GestureDirection::Vertical;

            Control* preferenceControl =
                dragCandidate ? dragCandidate : gestureStartControl;

            auto pref = preferenceControl
                ? preferenceControl->GetGestureDirectionPreference()
                : ScrollAxis::Auto;

            if (pref == ScrollAxis::Free)
                gestureLock = GestureDirection::Free;
            else if (pref == ScrollAxis::Horizontal)
                gestureLock = (dominant == GestureDirection::Horizontal)
                              ? GestureDirection::Horizontal
                              : GestureDirection::Vertical;
            else if (pref == ScrollAxis::Vertical)
                gestureLock = (dominant == GestureDirection::Vertical)
                              ? GestureDirection::Vertical
                              : GestureDirection::Horizontal;
            else
                gestureLock = dominant;

            // const char* lockName =
            //     gestureLock == GestureDirection::Horizontal ? "Horizontal" :
            //     gestureLock == GestureDirection::Vertical ? "Vertical" :
            //     gestureLock == GestureDirection::Free ? "Free" :
            //     "None";
            // Logging::Log(
            //   "[Gesture] lock=%s abs=(%.2f, %.2f) start=%s drag=%s scroll=%s\n",
            //   lockName,
            //   absX, absY,
            //   gestureStartControl ? gestureStartControl->GetControlName().c_str() : "(null)",
            //   dragCandidate ? dragCandidate->GetControlName().c_str() : "(null)",
            //   scrollCandidate ? scrollCandidate->GetControlName().c_str() : "(null)"
            // );
        }
    }

    if (scrollCandidate != nullptr)
    {
        const ScrollAxis scrollPref =
            scrollCandidate->GetGestureDirectionPreference();

        if (gestureLock == GestureDirection::Horizontal &&
            scrollPref == ScrollAxis::Vertical)
        {
            scrollCandidate = nullptr;
        }
        else if (gestureLock == GestureDirection::Vertical &&
                 scrollPref == ScrollAxis::Horizontal)
        {
            scrollCandidate = nullptr;
        }
    }

    // 🏁 Assign owner ONCE
    if (!activeGestureOwner)
    {
        if (gestureLock == GestureDirection::Horizontal)
        {
            activeGestureOwner = dragCandidate ? dragCandidate : scrollCandidate;
            activeGestureIsDrag = (dragCandidate != nullptr);
        }
        else if (gestureLock == GestureDirection::Vertical)
        {
            activeGestureOwner = scrollCandidate ? scrollCandidate : dragCandidate;
            activeGestureIsDrag = (dragCandidate != nullptr && !scrollCandidate);
        }
        else if (gestureLock == GestureDirection::Free)
        {
            activeGestureOwner = dragCandidate ? dragCandidate : scrollCandidate;
            activeGestureIsDrag = (dragCandidate != nullptr);
        }

        // if (activeGestureOwner != nullptr)
        // {
        //     const char* lockName =
        //         gestureLock == GestureDirection::Horizontal ? "Horizontal" :
        //         gestureLock == GestureDirection::Vertical ? "Vertical" :
        //         gestureLock == GestureDirection::Free ? "Free" :
        //         "None";
        //     Logging::Log(
        //       "[Gesture] owner=%s mode=%s lock=%s\n",
        //       activeGestureOwner->GetControlName().c_str(),
        //       activeGestureIsDrag ? "drag" : "scroll",
        //       lockName
        //     );
        // }
    }
}

void DUIWindow::DispatchMovement(
    Control* ctrl,
    DUIPoint relativeLocation,
    int x, int y,
    float dx, float dy)
{
    Control* target = nullptr;
    DUIPoint rel = relativeLocation;

    if (activeGestureOwner)
        target = activeGestureOwner;
    else if (gestureStartControl)
        target = gestureStartControl;
    else
        target = ctrl;

    if (target && target != ctrl)
    {
        if (target->cachedScreenLocation.x != -1)
        {
            rel = {
                x - target->cachedScreenLocation.x,
                y - target->cachedScreenLocation.y
            };
        }
    }

    if (target && target != (Control*)this && !activeGestureOwner)
    {
        PointerEvent e = BuildPointerEventForTarget(
          target,
          x,
          y,
          dx,
          dy,
          activeMouseButton);

        target->cursorHoverLocation = rel;

        // New pointer API
        target->OnPointerMove(e);

        // Legacy mouse API while migrating
        target->OnMouseMove(rel.x, rel.y);
        target->OnMouseMoveGlobal(x, y);
    }

    if (activeGestureOwner)
    {
        PointerEvent gestureEvent = BuildPointerEventForTarget(
            activeGestureOwner, x, y, dx, dy, activeMouseButton);

        activeGestureOwner->cursorHoverLocation = gestureEvent.position;

        activeGestureOwner->OnPointerMove(gestureEvent);
        activeGestureOwner->OnMouseMove(
            static_cast<int>(gestureEvent.position.x),
            static_cast<int>(gestureEvent.position.y));
        activeGestureOwner->OnMouseMoveGlobal(x, y);

        if (gestureLock == GestureDirection::Free)
        {
            // if (std::fabs(dx) > 0.01f || std::fabs(dy) > 0.01f) {
            //     Logging::Log(
            //       "[Gesture] dispatch free->scroll owner=%s delta=(%.2f, %.2f)\n",
            //       activeGestureOwner->GetControlName().c_str(),
            //       dx, dy
            //     );
            // }
            activeGestureOwner->OnMouseDrag(dx, dy);
            activeGestureOwner->OnScroll(dx, dy);
            activeGestureOwner->OnScroll(gestureEvent);
        }
        else if (activeGestureIsDrag)
        {
            activeGestureOwner->OnMouseDrag(dx, dy);
        }
        else
        {
            // if (std::fabs(dx) > 0.01f || std::fabs(dy) > 0.01f) {
            //     Logging::Log(
            //       "[Gesture] dispatch scroll owner=%s delta=(%.2f, %.2f)\n",
            //       activeGestureOwner->GetControlName().c_str(),
            //       dx, dy
            //     );
            // }
            activeGestureOwner->OnScroll(dx, dy);
            activeGestureOwner->OnScroll(gestureEvent);
        }
    }
}

void DUIWindow::SetSelectedControl(Control* control) {
  if (selectedControl == control)
    return;

  Control* previous = selectedControl;
  selectedControl = control;

  if (previous != nullptr) {
    previous->isSelected = false;
    previous->OnIsSelectedChanged();
    previous->MarkVisualDirty();
  }

  if (selectedControl != nullptr) {
    selectedControl->isSelected = true;
    selectedControl->OnIsSelectedChanged();
    selectedControl->MarkVisualDirty();
  }
}

void DUIWindow::ClearSelectedControl() {
  SetSelectedControl(nullptr);
}

Control* DUIWindow::GetSelectedControl() const {
  return selectedControl;
}

bool DUIWindow::IsControlSelected(const Control* control) const {
  return control != nullptr && control == selectedControl;
}

void DUIWindow::SelectControlFromPointerEvent(Control* control) {
  if (control == nullptr) {
    ClearSelectedControl();
    return;
  }

  SetSelectedControl(control);
}

void DUIWindow::OnMouseMove(int x, int y)
{
  UpdateCursor(x, y);

  DUIPoint relativeLocation{0,0};
  const HitTestParameters params;

  Control* ctrl = HitTestAt(
    x, y, relativeLocation, params);

  static DUIPoint lastMouse{};

  float deltaX = x - lastMouse.x;
  float deltaY = y - lastMouse.y;

  UpdateHover(ctrl, relativeLocation);
  UpdateGestureState(deltaX, deltaY);
  UpdateContextMenuHoldState();

  DispatchMovement(
    ctrl, relativeLocation,
    x, y, deltaX, deltaY);

  lastMouse = {
    static_cast<float>(x),
    static_cast<float>(y)
  };
}

void DUIWindow::OnMouseDown(MouseButton mouseButton)
{
  activeMouseButton = mouseButton;

  ResetContextMenuHoldTracking();
  activeGestureOwner = nullptr;
  activeGestureIsDrag = false;
  gestureLock = GestureDirection::None;
  totalDelta = {0, 0};

  if (mouseButton == MouseButton::Left) {
    gestureStartPoint = cursorLocation;
    gestureCurrentPoint = cursorLocation;
  } else {
    gestureStartPoint = {0, 0};
    gestureCurrentPoint = cursorLocation;
  }

  gestureStartControl = nullptr;

  if (hoverControl != nullptr)
  {
    if (mouseButton == MouseButton::Left &&
        selectedControl != nullptr &&
        selectedControl != hoverControl)
    {
      if (hoverControl->WantsClickCapture()) {
        selectedControl->isSelected = false;
        selectedControl->OnIsSelectedChanged();
      }
    }

    if (hoverControl != (Control*)this)
    {
      Control* clickTarget = SolveClickTarget(hoverControl);

      mouseHeldControl = clickTarget;

      mouseHeldControl->isMouseDown = true;

      if (mouseButton == MouseButton::Left) {
        gestureStartControl = mouseHeldControl;
        BeginContextMenuHoldIfAvailable(clickTarget);
      }

      if (clickTarget != nullptr &&
          clickTarget->WantsClickCapture())
      {
        if (mouseButton == MouseButton::Left)
          SelectControlFromPointerEvent(clickTarget);

        PointerEvent e = BuildPointerEventForTarget(
            clickTarget,
            static_cast<int>(cursorLocation.x),
            static_cast<int>(cursorLocation.y),
            0.0f,
            0.0f,
            mouseButton
        );

        clickTarget->OnPointerDown(e);

        if (mouseButton == MouseButton::Left) {
          // Legacy callback during migration.
          clickTarget->OnMouseDown();
        }
      }
    }
  }

  if (mouseButton == MouseButton::Right) {
    Control* contextMenuTarget = SolveContextMenuTarget(hoverControl);
    (void)TryPresentContextMenuForTarget(contextMenuTarget);
  }
}

void DUIWindow::OnMouseDown() {
  OnMouseDown(MouseButton::Left);
}

void DUIWindow::OnMouseUp(MouseButton mouseButton)
{
  if (activeMouseButton == MouseButton::None) {
    activeMouseButton = mouseButton;
  }

  const bool shouldInvokeLegacyMouseCallbacks =
    (activeMouseButton == MouseButton::Left);

  Control* holdTarget = activeContextMenuHoldTarget;
  const bool suppressClickLikeMouseUp = contextMenuHoldFired;

  if (holdTarget != nullptr) {
    holdTarget->EndContextMenuHoldTracking();
  }

  Control* gestureOwner = activeGestureOwner;
  const bool gestureWasActive = (gestureOwner != nullptr);
  Control* heldControl = mouseHeldControl;

  Control* pointerUpTarget = nullptr;

  if (gestureOwner != nullptr && gestureOwner != (Control*)this)
    pointerUpTarget = gestureOwner;
  else if (heldControl != nullptr && heldControl != (Control*)this)
    pointerUpTarget = heldControl;

  const bool suppressPointerUp =
    contextMenuHoldFired &&
    holdTarget != nullptr &&
    pointerUpTarget == holdTarget;

  if (pointerUpTarget != nullptr && !suppressPointerUp)
  {
    PointerEvent e = BuildPointerEventForTarget(
      pointerUpTarget,
      static_cast<int>(cursorLocation.x),
      static_cast<int>(cursorLocation.y),
      0.0f,
      0.0f,
      activeMouseButton
    );

    pointerUpTarget->OnPointerUp(e);
  }

  // Always end an active gesture owner, even when suppressing click-style mouse up.
  if (gestureOwner != nullptr &&
      gestureOwner != (Control*)this &&
      !suppressPointerUp &&
      shouldInvokeLegacyMouseCallbacks) {
    gestureOwner->OnMouseUp();
      }

  if (heldControl != nullptr &&
      heldControl != (Control*)this)
  {
    if (heldControl->WantsClickCapture()) {
      if (shouldInvokeLegacyMouseCallbacks &&
          !gestureWasActive &&
          !suppressClickLikeMouseUp) {
        heldControl->OnMouseUp();
      }

      if (mouseHeldControl == heldControl)
        heldControl->isMouseDown = false;
    }

    if (mouseHeldControl == heldControl)
      mouseHeldControl = nullptr;
  }

  gestureStartControl = nullptr;
  activeGestureOwner = nullptr;
  activeGestureIsDrag = false;
  gestureLock = GestureDirection::None;
  totalDelta = {0, 0};
  ResetContextMenuHoldTracking();
  activeMouseButton = MouseButton::None;
}

void DUIWindow::OnMouseUp() {
  OnMouseUp(MouseButton::Left);
}

void DUIWindow::OnMouseWheel(float deltaX, float deltaY, bool isPrecise) {
  DUIPoint relativeLocation {0, 0};
  const HitTestParameters params;

  Control* hoverTarget = HitTestAt(
    static_cast<int>(cursorLocation.x),
    static_cast<int>(cursorLocation.y),
    relativeLocation,
    params);

  UpdateHover(hoverTarget, relativeLocation);

  Control* wheelTarget = SolveScrollTarget(hoverTarget);
  if (wheelTarget == nullptr || wheelTarget == (Control*)this) {
    return;
  }

  if (keyboard_modifier_state.shift) {
    std::swap(deltaX, deltaY);
  }

  WheelScrollEvent event = BuildWheelScrollEventForTarget(
    wheelTarget,
    deltaX,
    deltaY,
    isPrecise);

  wheelTarget->OnWheelScroll(event);
}

void DUIWindow::OnMouseIn()
{
}

void DUIWindow::OnMouseOut()
{
  CancelContextMenuHoldTracking();
  activeMouseButton = MouseButton::None;
}

void DUIWindow::UpdateKeyboardState(const DUIKeyEvent& keyEvent) {
  if (keyEvent.key == DUIKey::ShiftLeft ||
      keyEvent.key == DUIKey::ShiftRight) {
    keyboard_modifier_state.shift = keyEvent.pressed;
  }

  if (keyEvent.key == DUIKey::ControlLeft ||
      keyEvent.key == DUIKey::ControlRight) {
    keyboard_modifier_state.ctrl = keyEvent.pressed;
  }

  if (keyEvent.key == DUIKey::AltLeft ||
      keyEvent.key == DUIKey::AltRight) {
    keyboard_modifier_state.alt = keyEvent.pressed;
  }

  if (keyEvent.key == DUIKey::SuperLeft ||
      keyEvent.key == DUIKey::SuperRight) {
    keyboard_modifier_state.meta = keyEvent.pressed;
  }
}

void DUIWindow::OnKeyDown(const DUIKeyEvent& keyEvent) {
  Control::OnKeyDown(keyEvent);

  UpdateKeyboardState(keyEvent);

  if (keyEvent.ctrl || keyEvent.super) {
    if (RouteShortcut(keyEvent))
      return;
  }

  if (selectedControl != nullptr)
    selectedControl->OnKeyDown(keyEvent);
}

void DUIWindow::OnKeyUp(const DUIKeyEvent& keyEvent) {
  Control::OnKeyUp(keyEvent);

  UpdateKeyboardState(keyEvent);

  if (selectedControl != nullptr)
    selectedControl->OnKeyUp(keyEvent);
}

void DUIWindow::OnTextInput(const DUITextInputEvent& textInputEvent) {
  Control::OnTextInput(textInputEvent);

  if (selectedControl != nullptr)
    selectedControl->OnTextInput(textInputEvent);
}

bool DUIWindow::RouteShortcut(const DUIKeyEvent &keyEvent) {
  Control* current = selectedControl; // later focusedControl / firstResponder

  while (current != nullptr) {
    if (current->HandleShortcut(keyEvent))
      return true;

    current = current->parent;
  }

  return HandleShortcut(keyEvent); // window/global fallback
}


// legacy
void DUIWindow::OnKeyDown(int vk) {
  if (keyDownCallback != nullptr)
    keyDownCallback(vk);

  if (selectedControl != nullptr)
    selectedControl->OnKeyDown(vk);
}

void DUIWindow::OnKeyUp(int vk) {
  if (keyUpCallback != nullptr)
    keyUpCallback(vk);

  if (selectedControl != nullptr)
    selectedControl->OnKeyUp(vk);
}

void DUIWindow::WindowRender(float deltaTime)
{


  if (!graphics) return;

  // static auto lastTime = std::chrono::high_resolution_clock::now();

  // auto now = std::chrono::high_resolution_clock::now();
  // float dt = std::chrono::duration<float>(now - lastTime).count();
  // lastTime = now;

  rootStackCoordinator.Update(deltaTime);

  // if (dt > 0.05f) dt = 0.05f;

  float width = size.width;
  float height = size.height;

  if (width <= 0 || height <= 0 || render_scale <= 0)
    return;

  UpdateContextMenuHoldState();

  if (!renderPipeline) {
    renderPipeline = std::make_unique<RenderPipeline>();
  }

  renderPipeline->RenderWindow(this, deltaTime);
}

void DUIWindow::IsolatedWindowRender(
  Graphics* graphics, bool cache_screen_loc)
{
}

void DUIWindow::OnResize()
{
}

void DUIWindow::OnRender(Graphics* rendTarget)
{

}

void DUIWindow::DoLayout(Graphics* renderTarget)
{

}
