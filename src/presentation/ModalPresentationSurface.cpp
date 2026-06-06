#include <kromakit/presentation/ModalPresentationSurface.h>

#include <utility>

#include <kromakit/Control.h>
#include <kromakit/DUIWindow.h>
#include <kromakit/ModalPresentation.h>
#include <kromakit/overlay_background_layer/OverlayBackgroundLayer.h>
#include <kromakit/overlay_background_layer/OverlayStack.h>

class SurfaceModalPresentation final : public ModalPresentation {
public:
  std::function<void()> onOverlayDismissedHook = nullptr;

  void OnOverlayDismissed() override {
    ModalPresentation::OnOverlayDismissed();

    if (onOverlayDismissedHook != nullptr) {
      onOverlayDismissedHook();
    }
  }

  std::optional<std::string> GetControlName() const override {
    return "SurfaceModalPresentation";
  }
};

ModalPresentationSurface::ModalPresentationSurface() = default;
ModalPresentationSurface::~ModalPresentationSurface() {
  ClearBorrowedContentIfAttached();

  if (overlayLayer_ != nullptr &&
      overlayLayer_->parent != nullptr) {
    overlayLayer_->parent->RemoveControl(overlayLayer_.get());
  }

  window_ = nullptr;
  overlayStack_ = nullptr;

  if (modal_ != nullptr) {
    modal_->onOverlayDismissedHook = nullptr;
  }

  // Destroy modal content while its parent overlay layer is still alive.
  modal_.reset();
  overlayLayer_.reset();
}

PresentationStyle ModalPresentationSurface::GetStyle() const {
  return PresentationStyle::Modal;
}

void ModalPresentationSurface::Attach(
  DUIWindow& window,
  OverlayStack& overlayStack) {

  window_ = &window;
  overlayStack_ = &overlayStack;

  EnsureInfrastructureCreated();
  EnsureLayerAttached();

  // Re-apply host state on each attachment. Some fields are currently stored-only
  // because ModalPresentation/OverlayBackgroundLayer does not expose setters yet.
  ApplyState(state_);

  if (content_ != nullptr) {
    modal_->SetBorrowedContent(content_);
  }
}

void ModalPresentationSurface::Detach() {
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

void ModalPresentationSurface::SetContent(Control* content) {
  content_ = content;

  EnsureInfrastructureCreated();

  if (content_ != nullptr) {
    modal_->SetBorrowedContent(content_);
    return;
  }

  ClearBorrowedContentIfAttached();
}

Control* ModalPresentationSurface::GetContent() const {
  return content_;
}

void ModalPresentationSurface::ApplyState(
  const PresentationSurfaceState& state) {

  state_ = state;
  EnsureInfrastructureCreated();

  // Currently supported by ModalPresentation.
  modal_->SetTitle(state_.title);
  if (overlayLayer_ != nullptr) {
    // overlayLayer_->SetDismissOnBackgroundTap(state_.dismissOnBackgroundTap);
  }

  // Deferred for future work:
  // - preferredSize/minSize are stored only.
}

void ModalPresentationSurface::Present() {
  if (window_ == nullptr || overlayStack_ == nullptr) {
    return;
  }

  EnsureInfrastructureCreated();
  EnsureLayerAttached();
  ApplyState(state_);

  if (content_ != nullptr) {
    modal_->SetBorrowedContent(content_);
  }

  hasNotifiedDismissForCurrentCycle_ = false;
  pendingDismissReason_ = PresentationDismissReason::Programmatic;
  isPresented_ = true;

  overlayLayer_->Present();
}

void ModalPresentationSurface::Dismiss(
  PresentationDismissReason reason) {

  pendingDismissReason_ = reason;

  if (overlayLayer_ == nullptr ||
      !isPresented_) {
    return;
  }

  overlayLayer_->Dismiss();
}

bool ModalPresentationSurface::IsPresented() const {
  return isPresented_;
}

void ModalPresentationSurface::SetOnDismissed(
  std::function<void(PresentationDismissReason)> cb) {
  onDismissed_ = std::move(cb);
}

void ModalPresentationSurface::EnsureInfrastructureCreated() {
  if (modal_ == nullptr) {
    modal_ = std::make_unique<SurfaceModalPresentation>();
    modal_->onOverlayDismissedHook = [this] {
      HandleModalOverlayDismissed();
    };
  }

  if (overlayLayer_ == nullptr) {
    overlayLayer_ = std::make_unique<OverlayBackgroundLayer>(modal_.get());
  }
}

void ModalPresentationSurface::EnsureLayerAttached() {
  if (overlayLayer_ == nullptr || overlayStack_ == nullptr) {
    return;
  }

  overlayStack_->PushBorrowedLayer(overlayLayer_.get());
}

void ModalPresentationSurface::ClearBorrowedContentIfAttached() {
  if (modal_ == nullptr) {
    return;
  }

  if (modal_->content != nullptr &&
      modal_->content->parent == modal_.get()) {
    modal_->RemoveControl(modal_->content);
  }

  modal_->content = nullptr;
}

void ModalPresentationSurface::HandleModalOverlayDismissed() {
  isPresented_ = false;

  // OverlayBackgroundLayer can trigger dismissal hooks multiple times during
  // a single close cycle (Dismiss + DismissImmediately path). Guard to avoid
  // duplicate external callbacks.
  NotifyDismissedOnce(pendingDismissReason_);
}

void ModalPresentationSurface::NotifyDismissedOnce(
  PresentationDismissReason reason) {

  if (hasNotifiedDismissForCurrentCycle_) {
    return;
  }

  hasNotifiedDismissForCurrentCycle_ = true;

  if (onDismissed_ != nullptr) {
    onDismissed_(reason);
  }
}
