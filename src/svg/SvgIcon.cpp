//
// Created by code on 5/8/26.
//

#include <kromakit/svg/SvgIcon.h>

#include <kromakit/styling/base_styles/SurfaceBaseStyles.h>

SvgIcon::SvgIcon() {
  svg_icon_ = nullptr;

  UseStyle(DUIDefaultStyles
    ::SurfaceBaseStyles
    ::NeutralSurfaces::Clear);
}

SvgIcon::SvgIcon(std::string_view svg_path) : SvgIcon() {
  svg_icon_ = SvgResourceCache::Load(svg_path.data());
}

void SvgIcon::SetTint(Color color) {
  tint_ = color;
  MarkVisualDirty();
}

void SvgIcon::ClearTint() {
  tint_ = std::nullopt;
  MarkVisualDirty();
}

void SvgIcon::OnRender(Graphics *rendTarget) {
  rendTarget->FillControlBackground(this);

  if (svg_icon_ == nullptr) {
    return;
  }

  DUIRect rect;
  rect.x = padding.left;
  rect.y = padding.top;
  rect.width = size.width - padding.right - rect.x;
  rect.height = size.height - padding.bottom - rect.y;

  rendTarget->RenderSVG(*svg_icon_, rect, tint_);
}

void SvgIcon::DoLayout(Graphics *rendTarget) {}

void SvgIcon::LoadSVG(std::string_view svg_path) {
  svg_icon_ = SvgResourceCache::Load(svg_path.data());
  MarkVisualDirty();
}
