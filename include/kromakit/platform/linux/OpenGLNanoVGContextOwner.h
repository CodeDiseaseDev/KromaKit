#pragma once

#include <nanovg.h>

class OpenGLNanoVGContextOwner {
public:
  OpenGLNanoVGContextOwner() = default;
  ~OpenGLNanoVGContextOwner();

  OpenGLNanoVGContextOwner(const OpenGLNanoVGContextOwner&) = delete;
  OpenGLNanoVGContextOwner& operator=(const OpenGLNanoVGContextOwner&) = delete;

  bool Create(int nvgFlags);
  void Reset();

  NVGcontext* Get() const {
    return context_;
  }

private:
  NVGcontext* context_ = nullptr;
};
