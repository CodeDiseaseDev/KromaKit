#pragma once

#include <functional>
#include <string>

#include <kromakit/graphics/types/DUISize.h>
#include <kromakit/presentation/PresentationStyle.h>

class Control;
class DUIWindow;
class OverlayStack;

struct PresentationSurfaceState {
  std::string title;
  DUISize preferredSize {0, 0};
  DUISize minSize {0, 0};
  // bool dismissOnBackgroundTap = true;
};

class IPresentationSurface {
public:
  virtual ~IPresentationSurface() = default;

  virtual PresentationStyle GetStyle() const = 0;

  // Attaches this surface to window/overlay infrastructure.
  // Used by PresentationHost when activating or switching styles.
  virtual void Attach(DUIWindow& window, OverlayStack& overlayStack) = 0;
  virtual void Detach() = 0;

  // Content is borrowed and non-owning.
  // Implementations must not delete or assume lifetime ownership.
  // PresentationHost can move the same content instance across surfaces.
  virtual void SetContent(Control* content) = 0;
  virtual Control* GetContent() const = 0;

  // Applies host-managed state that should survive style switching.
  virtual void ApplyState(const PresentationSurfaceState& state) = 0;

  virtual void Present() = 0;
  virtual void Dismiss(
    PresentationDismissReason reason =
      PresentationDismissReason::Programmatic) = 0;

  virtual bool IsPresented() const = 0;

  // Called when the surface has dismissed itself (or completed dismissal).
  // Used by PresentationHost to coordinate lifecycle and style transitions.
  virtual void SetOnDismissed(
    std::function<void(PresentationDismissReason)> cb) = 0;
};
