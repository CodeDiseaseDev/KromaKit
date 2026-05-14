#pragma once

#include <functional>
#include <memory>

#include <kromakit/presentation/IPresentationSurface.h>

class DUIWindow;
class OverlayStack;
class OverlayBackgroundLayer;
class Control;
class FloatingWindowControl;

class FloatingWindowSurface final : public IPresentationSurface {
public:
  FloatingWindowSurface();
  ~FloatingWindowSurface() override;

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
    std::function<void(PresentationDismissReason)> callback) override;

private:
  // Floating-window UI controls owned by this adapter. Presented content remains borrowed.
  std::unique_ptr<FloatingWindowControl> windowControl_;
  std::unique_ptr<OverlayBackgroundLayer> overlayLayer_;

  // Non-owning environment references provided by Attach().
  DUIWindow* window_ = nullptr;
  OverlayStack* overlayStack_ = nullptr;

  // Borrowed, non-owning presented content.
  Control* content_ = nullptr;

  PresentationSurfaceState state_;
  std::function<void(PresentationDismissReason)> onDismissed_ = nullptr;

  bool isPresented_ = false;
  bool hasNotifiedDismissForCurrentCycle_ = false;
  bool dismissInitiatedBySurface_ = false;
  PresentationDismissReason pendingDismissReason_ =
    PresentationDismissReason::Programmatic;

  void EnsureInfrastructureCreated();
  void EnsureLayerAttached();
  void ClearBorrowedContentIfAttached();
  void AttachBorrowedContentIfAvailable();
  void ApplyWindowState();
  void HandleWindowOverlayDismissed();
  void NotifyDismissedOnce(PresentationDismissReason reason);
};
