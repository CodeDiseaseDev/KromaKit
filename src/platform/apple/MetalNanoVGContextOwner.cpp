#include <kromakit/platform/apple/MetalNanoVGContextOwner.h>

#include <nanovg_mtl.h>

MetalNanoVGContextOwner::~MetalNanoVGContextOwner() {
  Reset();
}

bool MetalNanoVGContextOwner::Create(void* metalLayer, int nvgFlags) {
  Reset();

  if (metalLayer == nullptr) {
    return false;
  }

  context_ = nvgCreateMTL(metalLayer, nvgFlags);
  return context_ != nullptr;
}

void MetalNanoVGContextOwner::Reset() {
  if (context_ == nullptr) {
    return;
  }

  nvgDeleteMTL(context_);
  context_ = nullptr;
}
