//
// Created by code on 4/28/26.
//

#include <kromakit/overlay_background_layer/OverlayStack.h>

#include <kromakit/overlay_background_layer/OverlayBackgroundLayer.h>
#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

OverlayStack::~OverlayStack() {
}

OverlayStack::OverlayStack() {
  UseStyle(DUIDefaultStyles
    ::SurfaceBaseStyles
    ::NeutralSurfaces::Clear);
}

bool OverlayStack::HasActiveOverlay() const {
  for (auto* child : Children) {
    auto* layer = dynamic_cast<OverlayBackgroundLayer*>(child);
    if (layer != nullptr && layer->IsOpenOrClosing() && layer->GetPresentationOptions().backdropZoomOutEffect)
      return true;
  }

  return false;
}


void OverlayStack::DoLayout(Graphics *renderTarget) {
  (void)DoLayout2(renderTarget);
}

LayoutResult OverlayStack::DoLayout2(Graphics *renderTarget) {
  LayoutResult result;
  for (auto& child : Children) {
    if (child)
      result.geometryChanged |= child->SetLayoutFrame({0, 0}, size);
  }

  return result;
}

void OverlayStack::OnRender(Graphics *rendTarget) {
  // This will normally not do anything but call it anyway
  // to avoid UseStyle not applying a background if wanted
  // for whatever reason.
  rendTarget->FillControlBackground(this);
}


OverlayBackgroundLayer* OverlayStack::PushBorrowedLayer(
  OverlayBackgroundLayer* layer) {

  if (layer == nullptr)
    return nullptr;

  AddBorrowedControl(layer);

  layer->SetLayoutFrame({0, 0}, size);

  layer->UseStyle(DUIDefaultStyles
    ::SurfaceBaseStyles
    ::NeutralSurfaces::Clear);

  // This will repeat the message to DUIWindow and
  // invalidate the entire layout.
  MarkLayoutDirty();

  return layer;
}

bool OverlayStack::BringLayerToFront(OverlayBackgroundLayer *layer) {
  if (layer == nullptr) return false;

  auto it = std::find(Children.begin(), Children.end(), layer);
  if (it == Children.end()) return false;
  if (std::next(it) == Children.end()) return false;

  Control* ptr = *it;
  Children.erase(it);
  Children.push_back(ptr);

  MarkRenderTreeDirty();
  MarkVisualDirty();
  return true;
}
