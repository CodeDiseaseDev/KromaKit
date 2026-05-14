//
// Created by code on 5/8/26.
//

#include <kromakit/graphics/Graphics.h>

namespace {

static int ToNvgLineCap(SvgLineCap cap) {
  switch (cap) {
    case SvgLineCap::Round:
      return NVG_ROUND;

    case SvgLineCap::Square:
      return NVG_SQUARE;

    case SvgLineCap::Butt:
    default:
      return NVG_BUTT;
  }
}

static int ToNvgLineJoin(SvgLineJoin join) {
  switch (join) {
    case SvgLineJoin::Round:
      return NVG_ROUND;

    case SvgLineJoin::Bevel:
      return NVG_BEVEL;

    case SvgLineJoin::Miter:
    default:
      return NVG_MITER;
  }
}

} // namespace

void Graphics::RenderSVG(
  const SvgDocument &svg,
  float x, float y,
  float width, float height,
  std::optional<Color> tint) {

  if (svg.width <= 0.0f || svg.height <= 0.0f)
    return;

  if (width <= 0.0f || height <= 0.0f)
    return;

  const float scale = std::min(
    width / svg.width,
    height / svg.height
  );
  if (scale <= 0.0f)
    return;

  const float renderWidth = svg.width * scale;
  const float renderHeight = svg.height * scale;

  const float offsetX = (width - renderWidth) * 0.5f;
  const float offsetY = (height - renderHeight) * 0.5f;

  nvgSave(_RenderTarget);
  nvgTranslate(_RenderTarget, x + offsetX, y + offsetY);
  nvgScale(_RenderTarget, scale, scale);

  for (const auto& shape : svg.shapes) {
    if (!shape.visible)
      continue;
    if (shape.opacity <= 0.0f)
      continue;
    if (!shape.hasFill && !shape.hasStroke)
      continue;

    nvgSave(_RenderTarget);
    nvgGlobalAlpha(_RenderTarget, shape.opacity);

    auto BuildShapePath = [&](bool forFill) -> bool {
      nvgBeginPath(_RenderTarget);

      bool drewAnyPath = false;
      for (const auto& path : shape.paths) {
        if (path.points.size() < 8)
          continue;

        const int pointCount = static_cast<int>(path.points.size() / 2);
        if (pointCount < 4)
          continue;

        const float* pts = path.points.data();
        drewAnyPath = true;

        nvgMoveTo(_RenderTarget, pts[0], pts[1]);
        for (int i = 0; i + 3 < pointCount; i += 3) {
          const float* p = &pts[i * 2];
          nvgBezierTo(
            _RenderTarget,
            p[2], p[3],
            p[4], p[5],
            p[6], p[7]
          );
        }

        if (forFill || path.closed)
          nvgClosePath(_RenderTarget);

        if (forFill && shape.fillRule == SvgFillRule::EvenOdd) {
          nvgPathWinding(_RenderTarget, path.isHole ? NVG_HOLE : NVG_SOLID);
        }
      }

      return drewAnyPath;
    };

    const Color fillColor = tint.has_value()
      ? tint.value()
      : shape.fillColor;

    const Color strokeColor = tint.has_value()
      ? tint.value()
      : shape.strokeColor;

    bool drewAnyPath = false;
    if (shape.hasFill) {
      const bool hasFillPath = BuildShapePath(true);
      if (hasFillPath) {
        nvgFillColor(_RenderTarget, fillColor);
        nvgFill(_RenderTarget);
      }
      drewAnyPath = drewAnyPath || hasFillPath;
    }

    if (shape.hasStroke) {
      const bool hasStrokePath = BuildShapePath(false);
      if (hasStrokePath) {
        nvgStrokeColor(_RenderTarget, strokeColor);
        nvgStrokeWidth(_RenderTarget, shape.strokeWidth);
        nvgLineCap(_RenderTarget, ToNvgLineCap(shape.strokeLineCap));
        nvgLineJoin(_RenderTarget, ToNvgLineJoin(shape.strokeLineJoin));
        nvgMiterLimit(_RenderTarget, shape.miterLimit);
        nvgStroke(_RenderTarget);
      }
      drewAnyPath = drewAnyPath || hasStrokePath;
    }

    if (!drewAnyPath) {
      nvgRestore(_RenderTarget);
      continue;
    }

    nvgRestore(_RenderTarget);
  }

  nvgRestore(_RenderTarget);
}

void Graphics::RenderSVG(
  const SvgDocument &svg,
  DUIPoint location, DUISize size,
  std::optional<Color> tint) {

  RenderSVG(
    svg, location.x, location.y,
    size.width, size.height,
    tint);
}

void Graphics::RenderSVG(
  const SvgDocument &svg, DUIRect rect,
  std::optional<Color> tint) {
  RenderSVG(
    svg, rect.x, rect.y,
    rect.width, rect.height,
    tint);
}

std::shared_ptr<SvgDocument> Graphics::LoadSVG(
  const std::string &path) {

  return SvgResourceCache::Load(path);
}
