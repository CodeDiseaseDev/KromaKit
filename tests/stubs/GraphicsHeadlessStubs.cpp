#include <kromakit/graphics/Graphics.h>

extern "C" void nvgResetTransform(NVGcontext*) {}
extern "C" void nvgResetScissor(NVGcontext*) {}
extern "C" void nvgScissor(NVGcontext*, float, float, float, float) {}
extern "C" void nvgTranslate(NVGcontext*, float, float) {}
extern "C" void nvgScale(NVGcontext*, float, float) {}

void Graphics::SaveDrawingState() {}
void Graphics::RestoreDrawingState() {}
void Graphics::SetOpacity(float) {}

void Graphics::FillRectangle(float, float, float, float, Color) {}
void Graphics::DrawRectangle(float, float, float, float, Color) {}
void Graphics::DrawLine(float, float, float, float, Color, float) {}
void Graphics::DrawLines(const std::vector<LineSegment>&, float, float, Color, float) {}
void Graphics::FillControlBackground(Control*) {}

void Graphics::RenderText(
  const char*,
  DUIFont,
  Color,
  float,
  float
) const {}

DUISize Graphics::CalculateTextSize(const char*, DUIFont) {
  return {0, 0};
}
