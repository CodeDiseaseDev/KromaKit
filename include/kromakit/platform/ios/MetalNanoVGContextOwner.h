#pragma once

#include <nanovg.h>

class MetalNanoVGContextOwner {
public:
  MetalNanoVGContextOwner() = default;
  ~MetalNanoVGContextOwner();

  MetalNanoVGContextOwner(const MetalNanoVGContextOwner&) = delete;
  MetalNanoVGContextOwner& operator=(const MetalNanoVGContextOwner&) = delete;

  bool Create(void* metalLayer, int nvgFlags);
  void Reset();

  NVGcontext* Get() const {
    return context_;
  }

private:
  NVGcontext* context_ = nullptr;
};
