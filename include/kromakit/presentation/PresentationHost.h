#pragma once

#include <functional>
#include <memory>
#include <string>

#include <kromakit/presentation/IPresentationSurface.h>
#include <kromakit/presentation/PresentationStyle.h>

class Control;
class DUIWindow;
class OverlayStack;

class PresentationHost {
public:
  PresentationHost();
  ~PresentationHost();

  void Attach(DUIWindow& window, OverlayStack& overlayStack);
  void Detach();

  void SetContent(Control* content);
  Control* GetContent() const;

  void SetPreferredStyle(PresentationStyle style);
  PresentationStyle GetPreferredStyle() const;
  PresentationStyle GetResolvedStyle() const;

  void ApplyState(const PresentationSurfaceState& state);
  const PresentationSurfaceState& GetState() const;

  void SetTitle(std::string title);
  void SetPreferredSize(DUISize size);
  void SetMinSize(DUISize size);
  void SetDismissOnBackgroundTap(bool enabled);

  void Present();
  void Dismiss(
    PresentationDismissReason reason =
      PresentationDismissReason::Programmatic);
  bool IsPresented() const;

  void RefreshPresentationStyle();

  void SetOnDismissed(
    std::function<void(PresentationDismissReason)> cb);

private:
  PresentationStyle ResolveStyle() const;
  std::unique_ptr<IPresentationSurface> CreateSurface(PresentationStyle style);
  void EnsureSurface();
  void SwitchSurfaceIfNeeded();
  void BindSurfaceDismissCallback(IPresentationSurface& surface);

  // Non-owning host attachment points.
  DUIWindow* window_ = nullptr;
  OverlayStack* overlayStack_ = nullptr;

  std::unique_ptr<IPresentationSurface> activeSurface_;
  PresentationStyle preferredStyle_ = PresentationStyle::Auto;
  PresentationStyle resolvedStyle_ = PresentationStyle::Auto;

  // Borrowed, non-owning content pointer.
  Control* content_ = nullptr;
  PresentationSurfaceState state_;

  // Persists caller intent across surface switches and temporary detach/attach.
  bool shouldBePresented_ = false;
  std::function<void(PresentationDismissReason)> onDismissed_ = nullptr;
};
