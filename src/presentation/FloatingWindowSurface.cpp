#include <kromakit/presentation/FloatingWindowSurface.h>

#include <utility>

#include <kromakit/Control.h>
#include <kromakit/DUIWindow.h>
#include <kromakit/overlay_background_layer/OverlayBackgroundLayer.h>
#include <kromakit/overlay_background_layer/OverlayStack.h>
#include <kromakit/presentation/FloatingWindowControl.h>

FloatingWindowSurface::FloatingWindowSurface() = default;
FloatingWindowSurface::~FloatingWindowSurface() = default;

PresentationStyle FloatingWindowSurface::GetStyle() const {
  return PresentationStyle::FloatingWindow;
}

void FloatingWindowSurface::Attach(
  DUIWindow& window,
  OverlayStack& overlayStack) {
  window_ = &window;
  overlayStack_ = &overlayStack;

  EnsureInfrastructureCreated();
  EnsureLayerAttached();
  ApplyState(state_);
  AttachBorrowedContentIfAvailable();
}

void FloatingWindowSurface::Detach() {
  if (isPresented_) {
    Dismiss(PresentationDismissReason::HostStyleSwitch);
  }

  ClearBorrowedContentIfAttached();

  if (overlayLayer_ != nullptr &&
      overlayLayer_->parent != nullptr) {
    overlayLayer_->parent->RemoveControl(overlayLayer_.get());
  }

  window_ = nullptr;
  overlayStack_ = nullptr;
}

void FloatingWindowSurface::SetContent(Control* content) {
  if (content_ == content) {
    return;
  }

  ClearBorrowedContentIfAttached();
  content_ = content;

  EnsureInfrastructureCreated();

  if (content_ != nullptr) {
    AttachBorrowedContentIfAvailable();
    return;
  }

  ClearBorrowedContentIfAttached();
}

Control* FloatingWindowSurface::GetContent() const {
  return content_;
}

void FloatingWindowSurface::ApplyState(
  const PresentationSurfaceState& state) {
  state_ = state;

  EnsureInfrastructureCreated();
  ApplyWindowState();
}

void FloatingWindowSurface::Present() {
  if (window_ == nullptr || overlayStack_ == nullptr) {
    return;
  }

  EnsureInfrastructureCreated();
  EnsureLayerAttached();
  ApplyWindowState();
  AttachBorrowedContentIfAvailable();

  hasNotifiedDismissForCurrentCycle_ = false;
  pendingDismissReason_ = PresentationDismissReason::Programmatic;
  dismissInitiatedBySurface_ = false;
  isPresented_ = true;

  overlayLayer_->Present();
  overlayStack_->BringLayerToFront(overlayLayer_.get());
}

void FloatingWindowSurface::Dismiss(
  PresentationDismissReason reason) {
  pendingDismissReason_ = reason;

  if (overlayLayer_ == nullptr ||
      !isPresented_) {
    return;
  }

  dismissInitiatedBySurface_ = true;
  overlayLayer_->Dismiss();
}

bool FloatingWindowSurface::IsPresented() const {
  return isPresented_;
}

void FloatingWindowSurface::SetOnDismissed(
  std::function<void(PresentationDismissReason)> callback) {
  onDismissed_ = std::move(callback);
}

void FloatingWindowSurface::EnsureInfrastructureCreated() {
  if (windowControl_ == nullptr) {
    windowControl_ = std::make_unique<FloatingWindowControl>();
    windowControl_->OnCloseRequested = [this] {
      Dismiss(PresentationDismissReason::CloseButton);
    };
    windowControl_->OnOverlayDismissedHook = [this] {
      HandleWindowOverlayDismissed();
    };
    windowControl_->OnRequestTopMost = [this] {
      if (overlayStack_ == nullptr || overlayLayer_ == nullptr) return;
      overlayStack_->BringLayerToFront(overlayLayer_.get());
    };
  }

  if (overlayLayer_ == nullptr) {
    overlayLayer_ = std::make_unique<OverlayBackgroundLayer>(windowControl_.get());
    overlayLayer_->GetPresentationOptions() =
      overlayLayer_->GetPresentationOptions().DesktopPreset();
  }
}

void FloatingWindowSurface::EnsureLayerAttached() {
  if (overlayLayer_ == nullptr || overlayStack_ == nullptr) {
    return;
  }

  overlayStack_->PushBorrowedLayer(overlayLayer_.get());
}

void FloatingWindowSurface::ClearBorrowedContentIfAttached() {
  if (windowControl_ == nullptr ||
      content_ == nullptr) {
    return;
  }

  Panel& contentPanel = windowControl_->GetContentPanel();
  if (content_->parent == &contentPanel) {
    contentPanel.RemoveControl(content_);
  }
}

void FloatingWindowSurface::AttachBorrowedContentIfAvailable() {
  if (windowControl_ == nullptr ||
      content_ == nullptr) {
    return;
  }

  Panel& contentPanel = windowControl_->GetContentPanel();
  if (content_->parent != nullptr &&
      content_->parent != &contentPanel) {
    content_->parent->RemoveControl(content_);
  }

  if (content_->parent != &contentPanel) {
    contentPanel.AddBorrowedControl(content_);
  }

  content_->isVisible = true;
  contentPanel.MarkLayoutDirty();
  contentPanel.MarkVisualDirty();
}

void FloatingWindowSurface::ApplyWindowState() {
  if (windowControl_ == nullptr) {
    return;
  }

  windowControl_->SetTitle(state_.title);

  if (state_.preferredSize.width > 0.0f &&
      state_.preferredSize.height > 0.0f) {
    windowControl_->SetLayoutSize(state_.preferredSize);
  } else if (windowControl_->size.width <= 0.0f ||
             windowControl_->size.height <= 0.0f) {
    windowControl_->SetLayoutSize({ 760.0f, 520.0f });
  }

  if (windowControl_->location.x == 0.0f &&
      windowControl_->location.y == 0.0f) {
    windowControl_->SetLayoutLocation({ 120.0f, 80.0f });
  }

  if (overlayLayer_ != nullptr) {
    // overlayLayer_->SetDismissOnBackgroundTap(state_.dismissOnBackgroundTap);

  }

  windowControl_->MarkLayoutDirty();
  windowControl_->MarkVisualDirty();
}

void FloatingWindowSurface::HandleWindowOverlayDismissed() {
  isPresented_ = false;

  const PresentationDismissReason reason =
    dismissInitiatedBySurface_
      ? pendingDismissReason_
      : PresentationDismissReason::BackgroundTap;

  dismissInitiatedBySurface_ = false;
  NotifyDismissedOnce(reason);
}

void FloatingWindowSurface::NotifyDismissedOnce(
  PresentationDismissReason reason) {
  if (hasNotifiedDismissForCurrentCycle_) {
    return;
  }

  hasNotifiedDismissForCurrentCycle_ = true;

  if (onDismissed_ != nullptr) {
    onDismissed_(reason);
  }
}
