#include <kromakit/presentation/PresentationHost.h>

#include <utility>

#include <kromakit/presentation/FloatingWindowSurface.h>
#include <kromakit/Control.h>
#include <kromakit/DUIWindow.h>
#include <kromakit/overlay_background_layer/OverlayStack.h>
#include <kromakit/presentation/ModalPresentationSurface.h>
#include <kromakit/platform/Platform.h>

PresentationHost::PresentationHost() = default;
PresentationHost::~PresentationHost() = default;

void PresentationHost::Attach(
  DUIWindow& window,
  OverlayStack& overlayStack) {

  window_ = &window;
  overlayStack_ = &overlayStack;

  EnsureSurface();

  if (activeSurface_ != nullptr &&
      shouldBePresented_) {
    activeSurface_->Present();
  }
}

void PresentationHost::Detach() {
  if (activeSurface_ != nullptr) {
    if (activeSurface_->IsPresented()) {
      // Detach is an infrastructure transition. Keep caller intent
      // (shouldBePresented_) so future Attach() can restore presentation.
      activeSurface_->Dismiss(
        PresentationDismissReason::HostStyleSwitch);
    }

    activeSurface_->Detach();
  }

  window_ = nullptr;
  overlayStack_ = nullptr;
}

void PresentationHost::SetContent(Control* content) {
  content_ = content;

  if (activeSurface_ != nullptr) {
    activeSurface_->SetContent(content_);
  }
}

Control* PresentationHost::GetContent() const {
  return content_;
}

void PresentationHost::SetPreferredStyle(
  PresentationStyle style) {
  preferredStyle_ = style;
  RefreshPresentationStyle();
}

PresentationStyle PresentationHost::GetPreferredStyle() const {
  return preferredStyle_;
}

PresentationStyle PresentationHost::GetResolvedStyle() const {
  return resolvedStyle_;
}

void PresentationHost::ApplyState(
  const PresentationSurfaceState& state) {

  state_ = state;

  if (activeSurface_ != nullptr) {
    activeSurface_->ApplyState(state_);
  }
}

const PresentationSurfaceState& PresentationHost::GetState() const {
  return state_;
}

void PresentationHost::SetTitle(std::string title) {
  state_.title = std::move(title);

  if (activeSurface_ != nullptr) {
    activeSurface_->ApplyState(state_);
  }
}

void PresentationHost::SetPreferredSize(DUISize size) {
  state_.preferredSize = size;

  if (activeSurface_ != nullptr) {
    activeSurface_->ApplyState(state_);
  }
}

void PresentationHost::SetMinSize(DUISize size) {
  state_.minSize = size;

  if (activeSurface_ != nullptr) {
    activeSurface_->ApplyState(state_);
  }
}

void PresentationHost::SetDismissOnBackgroundTap(bool enabled) {
  // state_.dismissOnBackgroundTap = enabled;

  if (activeSurface_ != nullptr) {
    activeSurface_->ApplyState(state_);
  }
}

void PresentationHost::Present() {
  shouldBePresented_ = true;

  EnsureSurface();

  if (activeSurface_ != nullptr) {
    activeSurface_->Present();
  }
}

void PresentationHost::Dismiss(
  PresentationDismissReason reason) {

  shouldBePresented_ = false;

  if (activeSurface_ != nullptr) {
    activeSurface_->Dismiss(reason);
  }
}

bool PresentationHost::IsPresented() const {
  return activeSurface_ != nullptr &&
         activeSurface_->IsPresented();
}

void PresentationHost::RefreshPresentationStyle() {
  SwitchSurfaceIfNeeded();
}

void PresentationHost::SetOnDismissed(
  std::function<void(PresentationDismissReason)> cb) {
  onDismissed_ = std::move(cb);
}

PresentationStyle PresentationHost::ResolveStyle() const {
  switch (preferredStyle_) {
    case PresentationStyle::Auto:
      return Platform::Instance.IsDesktop() ?
        PresentationStyle::FloatingWindow :
        PresentationStyle::Modal;

    case PresentationStyle::Modal:
      return PresentationStyle::Modal;

    case PresentationStyle::FloatingWindow:
      return PresentationStyle::FloatingWindow;
  }

  return PresentationStyle::Modal;
}

std::unique_ptr<IPresentationSurface>
PresentationHost::CreateSurface(PresentationStyle style) {
  switch (style) {
    case PresentationStyle::Modal:
      return std::make_unique<ModalPresentationSurface>();

    case PresentationStyle::FloatingWindow:
      return std::make_unique<FloatingWindowSurface>();

    case PresentationStyle::Auto:
      return CreateSurface(ResolveStyle());
  }

  return std::make_unique<ModalPresentationSurface>();
}


void PresentationHost::EnsureSurface() {
  SwitchSurfaceIfNeeded();

  if (activeSurface_ == nullptr ||
      window_ == nullptr ||
      overlayStack_ == nullptr) {
    return;
  }

  activeSurface_->Attach(*window_, *overlayStack_);
  activeSurface_->ApplyState(state_);
  activeSurface_->SetContent(content_);
}

void PresentationHost::SwitchSurfaceIfNeeded() {
  const PresentationStyle target = ResolveStyle();
  const bool needsNewSurface =
    activeSurface_ == nullptr ||
    resolvedStyle_ != target;

  if (!needsNewSurface) {
    return;
  }

  if (activeSurface_ != nullptr) {
    if (activeSurface_->IsPresented()) {
      activeSurface_->Dismiss(
        PresentationDismissReason::HostStyleSwitch);
    }

    activeSurface_->Detach();
    activeSurface_.reset();
  }

  resolvedStyle_ = target;
  activeSurface_ = CreateSurface(target);
  if (activeSurface_ == nullptr) {
    return;
  }

  BindSurfaceDismissCallback(*activeSurface_);

  if (window_ != nullptr &&
      overlayStack_ != nullptr) {
    activeSurface_->Attach(*window_, *overlayStack_);
  }

  activeSurface_->ApplyState(state_);
  activeSurface_->SetContent(content_);

  if (shouldBePresented_ &&
      window_ != nullptr &&
      overlayStack_ != nullptr) {
    activeSurface_->Present();
  }
}

void PresentationHost::BindSurfaceDismissCallback(
  IPresentationSurface& surface) {

  surface.SetOnDismissed([this](PresentationDismissReason reason) {
    // Surface switching should not be surfaced as a user-level dismissal,
    // and should not clear caller "should be presented" intent.
    if (reason != PresentationDismissReason::HostStyleSwitch) {
      shouldBePresented_ = false;

      if (onDismissed_ != nullptr) {
        onDismissed_(reason);
      }
    }
  });
}
