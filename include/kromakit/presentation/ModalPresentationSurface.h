#pragma once

#include <functional>
#include <memory>

#include <kromakit/presentation/IPresentationSurface.h>

class DUIWindow;
class OverlayStack;
class OverlayBackgroundLayer;
class Control;
class ModalPresentation;
class SurfaceModalPresentation;

class ModalPresentationSurface : public IPresentationSurface {
public:
  ModalPresentationSurface();
  ~ModalPresentationSurface() override;

  PresentationStyle GetStyle() const override;
  void Attach(DUIWindow& window, OverlayStack& overlayStack) override;
  void Detach() override;
  void SetContent(Control* content) override;
  Control* GetContent() const override;
  void ApplyState(const PresentationSurfaceState& state) override;
  void Present() override;
  void Dismiss(
    PresentationDismissReason reason =
      PresentationDismissReason::Programmatic) override;
  bool IsPresented() const override;
  void SetOnDismissed(
    std::function<void(PresentationDismissReason)> cb) override;

private:
  // Modal+overlay UI controls owned by this adapter. Presented content remains borrowed.
  std::unique_ptr<OverlayBackgroundLayer> overlayLayer_;
  std::unique_ptr<SurfaceModalPresentation> modal_;

  // Non-owning environment references provided by Attach().
  DUIWindow* window_ = nullptr;
  OverlayStack* overlayStack_ = nullptr;

  // Borrowed, non-owning presented content.
  Control* content_ = nullptr;

  PresentationSurfaceState state_;
  std::function<void(PresentationDismissReason)> onDismissed_ = nullptr;

  bool isPresented_ = false;
  bool hasNotifiedDismissForCurrentCycle_ = false;
  PresentationDismissReason pendingDismissReason_ =
    PresentationDismissReason::Programmatic;

  void EnsureInfrastructureCreated();
  void EnsureLayerAttached();
  void ClearBorrowedContentIfAttached();
  void HandleModalOverlayDismissed();
  void NotifyDismissedOnce(PresentationDismissReason reason);
};
