#include <kromakit/graphics/Graphics.h>

#include <algorithm>
#include <cmath>
#include <float.h>

#include <kromakit/Control.h>

Graphics::Graphics(
	NVGcontext* vg)
{
	this->_RenderTarget = vg;
}

void Graphics::RenderShadow(
	Color col,
	float x, float y,
	float w, float h,
	float radius,
	float feather,
	float offsetX,
	float offsetY) {

	NVGpaint shadow = nvgBoxGradient(
		_RenderTarget,
		x + offsetX,
		y + offsetY,
		w, h,
		radius,
		feather,
		col,
		nvgRGBA(0, 0, 0, 0)
	);

	const float spread =
		feather +
		std::max(std::abs(offsetX), std::abs(offsetY)) +
		4.0f;

	nvgBeginPath(_RenderTarget);
	nvgRect(
		_RenderTarget,
		x + offsetX - spread,
		y + offsetY - spread,
		w + spread * 2.0f,
		h + spread * 2.0f
	);

	nvgRoundedRect(_RenderTarget, x, y, w, h, radius);
	nvgPathWinding(_RenderTarget, NVG_HOLE);

	nvgFillPaint(_RenderTarget, shadow);
	nvgFill(_RenderTarget);
}

void Graphics::RenderBasicShadow(DUISize size) {
	RenderShadow(
		nvgRGBA(0, 0, 0, 80),
		0, 0, size.width, size.height,
		5, 30,
		0.0f, 0.0f);
}

void Graphics::SaveDrawingState()
{
	nvgSave(_RenderTarget);
}

void Graphics::RestoreDrawingState()
{
	nvgRestore(_RenderTarget);
}

void Graphics::RotateTransformAt(
	float rotate, float x, float y)
{
	nvgTranslate(_RenderTarget, x, y);
	nvgRotate(_RenderTarget, rotate);
	nvgTranslate(_RenderTarget, -x, -y);
}

void Graphics::FillRectangle(
	float x, float y,
	float width, float height,
	Color fill)
{
	nvgBeginPath(_RenderTarget);
	nvgRect(_RenderTarget, x, y, width, height);
	nvgFillColor(_RenderTarget, fill);
	nvgFill(_RenderTarget);
}

void Graphics::DrawRectangle(
	float x, float y,
	float width, float height,
	Color color)
{
	nvgBeginPath(_RenderTarget);
	nvgRect(_RenderTarget, x, y, width, height);
	nvgStrokeColor(_RenderTarget, color);
	nvgStroke(_RenderTarget);
}

void Graphics::RenderFailedFontResolve(float x, float y) const {
	DrawRoundedRectangle(x, y, 200, 10, ColorFromRGB(255,255,0), 5);

}

void Graphics::RenderText(
	const char* text,
	DUIFont font,
	Color fill,
	float x,
	float y) const
{
	if (text == nullptr || text[0] == '\0') {
		return;
	}

	auto resolvedFont = FontRegistry::Resolve(font);

	if (!resolvedFont.success) {
		RenderFailedFontResolve(x,y);
		return;
	}

	std::string& resolvedFontName = resolvedFont.fullFontName;

	nvgFillColor(_RenderTarget, fill);
	nvgFontFace(_RenderTarget, resolvedFontName.c_str());
	nvgFontSize(_RenderTarget, font.FontSize);

	nvgTextAlign(_RenderTarget, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

	nvgText(_RenderTarget, x, y, text, nullptr);

}


void Graphics::RenderTextBox(
	const char* text,
	DUIFont font, Color fill,
	float x, float y, float width) const {

	auto resolvedFont = FontRegistry::Resolve(font);

	if (!resolvedFont.success) {
		RenderFailedFontResolve(x,y);
		return;
	}

	std::string& resolvedFontName = resolvedFont.fullFontName;

	// float bounds[4];

	nvgFillColor(_RenderTarget, fill);
	nvgFontFace(_RenderTarget, resolvedFontName.c_str());
	nvgFontSize(_RenderTarget, font.FontSize);
	nvgTextAlign(_RenderTarget, NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
	nvgTextBox(_RenderTarget, x, y, width, text, nullptr);

}

DUISize Graphics::CalculateTextSize(
	const char* text, DUIFont font)
{
	auto resolvedFont = FontRegistry::Resolve(font);

	if (!resolvedFont.success) {
		return {0,0};
	}

	std::string& resolvedFontName = resolvedFont.fullFontName;

	nvgFontFace(_RenderTarget, resolvedFontName.c_str());
	nvgFontSize(_RenderTarget, font.FontSize);

	float bounds[4];
	nvgTextBounds(
		_RenderTarget, 0, 0,
		text, nullptr, bounds
	);

	return {
		bounds[2] - bounds[0],
		bounds[3] - bounds[1]
	};
}

float Graphics::CalculateTextAdvance(
	const char* text, DUIFont font) const {

	if (text == nullptr || text[0] == '\0')
		return 0.0f;

	auto resolvedFont = FontRegistry::Resolve(font);

	if (!resolvedFont.success) {
		return 0;
	}

	std::string& resolvedFontName = resolvedFont.fullFontName;
	nvgFontFace(_RenderTarget, resolvedFontName.c_str());
	nvgFontSize(_RenderTarget, font.FontSize);

	constexpr int kMaxGlyphs = 1024;
	NVGglyphPosition positions[kMaxGlyphs];

	int count = nvgTextGlyphPositions(
		_RenderTarget,
		0.0f,
		0.0f,
		text,
		nullptr,
		positions,
		kMaxGlyphs
	);

	if (count <= 0)
		return 0.0f;

	return positions[count - 1].maxx;
}

DUISize Graphics::CalculateTextBoxSize(
	const char* text, float width, DUIFont font)
{
	auto resolvedFont = FontRegistry::Resolve(font);

	if (!resolvedFont.success) {
		return {0,0};
	}

	std::string& resolvedFontName = resolvedFont.fullFontName;

	nvgFontFace(_RenderTarget, resolvedFontName.c_str());
	nvgFontSize(_RenderTarget, font.FontSize);

	float bounds[4];
	nvgTextBoxBounds(
		_RenderTarget, 0, 0,
		width, text, nullptr, bounds
	);

	return {
		bounds[2] - bounds[0],
		bounds[3] - bounds[1]
	};
}

void Graphics::RenderTextCentered(
	const char* text,
	DUIFont font, Color fill,
	Control* control)
{
	auto textMetrics = CalculateTextSize(text, font);

	float x = control->size.width / 2 - textMetrics.width / 2;
	float y = control->size.height / 2 - textMetrics.height / 2;

	RenderText(text, font, fill, x, y);
}

namespace {

struct CornerRadiusXY {
	float rx = 0.0f;
	float ry = 0.0f;
};

struct CornerRadiiXY {
	CornerRadiusXY tl;
	CornerRadiusXY tr;
	CornerRadiusXY br;
	CornerRadiusXY bl;
};

float ClampZero(float value)
{
	return std::max(value, 0.0f);
}

CornerRadiiXY NormalizeCornerRadii(
	float width,
	float height,
	CornerRadiiXY radii)
{
	radii.tl.rx = ClampZero(radii.tl.rx);
	radii.tl.ry = ClampZero(radii.tl.ry);
	radii.tr.rx = ClampZero(radii.tr.rx);
	radii.tr.ry = ClampZero(radii.tr.ry);
	radii.br.rx = ClampZero(radii.br.rx);
	radii.br.ry = ClampZero(radii.br.ry);
	radii.bl.rx = ClampZero(radii.bl.rx);
	radii.bl.ry = ClampZero(radii.bl.ry);

	float scale = 1.0f;
	const float top = radii.tl.rx + radii.tr.rx;
	const float right = radii.tr.ry + radii.br.ry;
	const float bottom = radii.bl.rx + radii.br.rx;
	const float left = radii.tl.ry + radii.bl.ry;

	if (top > width && top > 0.0f)
		scale = std::min(scale, width / top);
	if (right > height && right > 0.0f)
		scale = std::min(scale, height / right);
	if (bottom > width && bottom > 0.0f)
		scale = std::min(scale, width / bottom);
	if (left > height && left > 0.0f)
		scale = std::min(scale, height / left);

	radii.tl.rx *= scale;
	radii.tl.ry *= scale;
	radii.tr.rx *= scale;
	radii.tr.ry *= scale;
	radii.br.rx *= scale;
	radii.br.ry *= scale;
	radii.bl.rx *= scale;
	radii.bl.ry *= scale;

	return radii;
}

void AddRoundedRectPath(
	NVGcontext* vg,
	float x,
	float y,
	float width,
	float height,
	CornerRadiiXY radii)
{
	if (width <= 0.0f || height <= 0.0f)
		return;

	radii = NormalizeCornerRadii(width, height, radii);
	constexpr float k = 0.552284749831f;

	const float right = x + width;
	const float bottom = y + height;

	nvgMoveTo(vg, x + radii.tl.rx, y);
	nvgLineTo(vg, right - radii.tr.rx, y);

	if (radii.tr.rx > 0.0f || radii.tr.ry > 0.0f) {
		nvgBezierTo(
			vg,
			right - radii.tr.rx * (1.0f - k), y,
			right, y + radii.tr.ry * (1.0f - k),
			right, y + radii.tr.ry);
	}

	nvgLineTo(vg, right, bottom - radii.br.ry);

	if (radii.br.rx > 0.0f || radii.br.ry > 0.0f) {
		nvgBezierTo(
			vg,
			right, bottom - radii.br.ry * (1.0f - k),
			right - radii.br.rx * (1.0f - k), bottom,
			right - radii.br.rx, bottom);
	}

	nvgLineTo(vg, x + radii.bl.rx, bottom);

	if (radii.bl.rx > 0.0f || radii.bl.ry > 0.0f) {
		nvgBezierTo(
			vg,
			x + radii.bl.rx * (1.0f - k), bottom,
			x, bottom - radii.bl.ry * (1.0f - k),
			x, bottom - radii.bl.ry);
	}

	nvgLineTo(vg, x, y + radii.tl.ry);

	if (radii.tl.rx > 0.0f || radii.tl.ry > 0.0f) {
		nvgBezierTo(
			vg,
			x, y + radii.tl.ry * (1.0f - k),
			x + radii.tl.rx * (1.0f - k), y,
			x + radii.tl.rx, y);
	}

	nvgClosePath(vg);
}

}

void Graphics::DrawRoundedRectangleBorder(
		float x, float y,
		float w, float h,
		Color color,
		DUIInsets radius,
		DUIInsets border
) {
	if (color.a <= 0.0f)
		return;

	if (w <= 0.0f || h <= 0.0f)
		return;

	const float bl = std::max(border.left, 0.0f);
	const float bt = std::max(border.top, 0.0f);
	const float br = std::max(border.right, 0.0f);
	const float bb = std::max(border.bottom, 0.0f);

	if (bl <= 0.0f && bt <= 0.0f && br <= 0.0f && bb <= 0.0f)
		return;

	const float ix = x + bl;
	const float iy = y + bt;
	const float iw = w - bl - br;
	const float ih = h - bt - bb;

	if (iw <= 0.0f || ih <= 0.0f) {
		FillRoundedRectangle(x, y, w, h, color, radius);
		return;
	}

	CornerRadiiXY outerRadii = NormalizeCornerRadii(w, h, {
		{ radius.left, radius.left },
		{ radius.top, radius.top },
		{ radius.bottom, radius.bottom },
		{ radius.right, radius.right }
	});

	CornerRadiiXY innerRadii = NormalizeCornerRadii(iw, ih, {
		{ std::max(0.0f, outerRadii.tl.rx - bl), std::max(0.0f, outerRadii.tl.ry - bt) },
		{ std::max(0.0f, outerRadii.tr.rx - br), std::max(0.0f, outerRadii.tr.ry - bt) },
		{ std::max(0.0f, outerRadii.br.rx - br), std::max(0.0f, outerRadii.br.ry - bb) },
		{ std::max(0.0f, outerRadii.bl.rx - bl), std::max(0.0f, outerRadii.bl.ry - bb) }
	});

	nvgBeginPath(_RenderTarget);
	AddRoundedRectPath(_RenderTarget, x, y, w, h, outerRadii);
	AddRoundedRectPath(_RenderTarget, ix, iy, iw, ih, innerRadii);
	nvgPathWinding(_RenderTarget, NVG_HOLE);

	nvgFillColor(_RenderTarget, color);
	nvgFill(_RenderTarget);
}

void Graphics::FillControlBackground(Control* control)
{
	if (control == nullptr) {
		return;
	}

	const bool hoverBackgorund =
		control->useHoverBackground &&
		control->isMouseHovering;

	const Color& bg = hoverBackgorund ?
		control->hoverBackground :
		control->background;

	const DUISize& size = control->size;
	const DUIInsets& cr = control->cornerRadius;
	const DUIInsets& pad = control->backgroundPadding;

	const DUIInsets& outlineSize = control->outlineThickness;
	const Color& outline = control->outline;

	const DUIRect rect = {
		-pad.left, -pad.top,
		size.width +  pad.left + pad.right,
		size.height + pad.top + pad.bottom,
	};

	// ✨optimisation✨
	if (bg.a > 0.0f) {
		FillRoundedRectangle(
				rect.x, rect.y,
				rect.width, rect.height,
				bg, cr);
	}

	// ✨more optimisation✨
	if (!outlineSize.IsEmpty()) {
		DrawRoundedRectangleBorder(
			rect.x, rect.y,
			rect.width, rect.height,
			outline, cr, outlineSize);
	}
}

void Graphics::DrawErrorX(
	Control* control, const char* str)
{
	constexpr Color red = { 1,0,0,1 };
	constexpr Color red_transparent = { 1,0,0,0.2 };
	constexpr float space = 5;

	FillRoundedRectangle(
		space, space, control->size.width - space * 2,
									control->size.height - space * 2,
		red_transparent, 6
	);

	DrawLine(
		0, 0, control->size.width,
					control->size.height,
		red);

	DrawLine(
		0, control->size.height,
		control->size.width, 0,
		red);

	char buf[1024];
	sprintf(
		buf, "Error (DrawErrorX() called from control '%x'),\n    \"%s\"",
		control->ControlID,
		str
	);

	RenderText(
		buf,
		control->font, red,
		control->size.width / 2,
		control->size.height / 2
	);
}

void Graphics::FillRoundedRectangle(
	float x, float y,
	float width, float height,
	NVGpaint fill, float cornerRadius) {



	nvgBeginPath(_RenderTarget);

	nvgRoundedRectVarying(
		_RenderTarget, x, y,
		width, height,
		cornerRadius,
		cornerRadius,
		cornerRadius,
		cornerRadius
	);

	nvgFillPaint(_RenderTarget, fill);
	nvgFill(_RenderTarget);
}

void Graphics::FillRoundedRectangle(
	float x, float y,
	float width, float height,
	NVGpaint fill, DUIInsets cornerRadius)
{
	nvgBeginPath(_RenderTarget);
	nvgRoundedRectVarying(
		_RenderTarget, x, y,
		width, height,
		cornerRadius.left,
		cornerRadius.top,
		cornerRadius.right,
		cornerRadius.bottom
	);
	nvgFillPaint(_RenderTarget, fill);
	nvgFill(_RenderTarget);
}

void Graphics::FillRoundedRectangleLinearGradient(
	float x, float y, float x_end, float y_end,
	float width, float height,
	Color col1, Color col2, float cornerRadius) {

	NVGpaint paint = nvgLinearGradient(
    _RenderTarget,
    x, y,
    x + x_end, y + y_end,
		col1, col2
	);

	FillRoundedRectangle(x, y, width, height, paint, cornerRadius);
}

void Graphics::FillRoundedRectangle(
	float x, float y,
	float width, float height,
	Color fill, float cr)
{
	DUIInsets crr = {
		cr, cr, cr, cr
	};
	FillRoundedRectangle(
		x, y, width, height,
		fill, crr);
}

void Graphics::FillRoundedRectangle(
	float x, float y,
	float width, float height, Color fill,
	DUIInsets cornerRadius) {

	nvgBeginPath(_RenderTarget);
	nvgRoundedRectVarying(
		_RenderTarget, x, y,
		width, height,
		cornerRadius.left,
		cornerRadius.top,
		cornerRadius.bottom,
		cornerRadius.right
	);
	nvgFillColor(_RenderTarget, fill);
	nvgFill(_RenderTarget);
}

void Graphics::FillEllipse(
	float center_x, float center_y,
	float radius, Color fill)
{
	FillRoundedRectangle(
		center_x - radius,
		center_y - radius,
		radius * 2, radius * 2,
		fill, radius
	);
}

void Graphics::DrawEllipse(
	float center_x, float center_y,
	float radius, Color color)
{


	// D2D1_ELLIPSE ellipse = {
	// 	{ center_x, center_y },
	// 	radius, radius
	// };

	nvgBeginPath(_RenderTarget);
	nvgFillColor(_RenderTarget, color);

	nvgEllipse(
		_RenderTarget,
		center_x, center_y,
		radius, radius
	);
	nvgStroke(_RenderTarget);
}

void Graphics::DrawRoundedRectangle(
	float x, float y,
	float width, float height,
	Color color, DUIInsets rc)
{
	nvgBeginPath(_RenderTarget);

	nvgBeginPath(_RenderTarget);
	nvgStrokeWidth(_RenderTarget, 1.0f);
	nvgRoundedRectVarying(
		_RenderTarget, x, y, width, height,
		rc.left, rc.top,
		rc.bottom, rc.right);
	nvgStrokeColor(_RenderTarget, color);
	nvgStroke(_RenderTarget);

}

void Graphics::DrawRoundedRectangle(
	float x, float y,
	float width, float height,
	Color color, float cornerRadius) const
{
	nvgBeginPath(_RenderTarget);

	nvgBeginPath(_RenderTarget);
	nvgStrokeWidth(_RenderTarget, 1.0f);
	nvgRoundedRect(_RenderTarget, x, y, width, height, cornerRadius);
	nvgStrokeColor(_RenderTarget, color);
	nvgStroke(_RenderTarget);
}

void Graphics::DrawLine(
	float x1, float y1,
	float x2, float y2,
	Color color, float thickness)
{
	nvgShapeAntiAlias(_RenderTarget, 1);

	nvgBeginPath(_RenderTarget);
	nvgMoveTo(_RenderTarget, x1, y1);
	nvgLineTo(_RenderTarget, x2, y2);
	nvgStrokeWidth(_RenderTarget, thickness);
	nvgStrokeColor(_RenderTarget, color);
	nvgStroke(_RenderTarget);
}

void Graphics::DrawLines(
	const std::vector<LineSegment> &lines,
	float x, float y, Color color, float width) {

	if (lines.empty())
		return;

	nvgBeginPath(_RenderTarget);

	for (const LineSegment& line : lines) {
		nvgMoveTo(_RenderTarget, line.x1, line.y1);
		nvgLineTo(_RenderTarget, line.x2, line.y2);
	}

	nvgStrokeColor(_RenderTarget, color);
	nvgStrokeWidth(_RenderTarget, width);
	nvgStroke(_RenderTarget);
}

void Graphics::DrawLinesAsPath(
	const std::vector<DUIPoint>& points,
	float x, float y,
	Color color, bool complete_shape, float thickness)
{
	if (points.size() < 2)
	{
		return;
	}

	nvgShapeAntiAlias(_RenderTarget, 1);
	nvgBeginPath(_RenderTarget);

	nvgMoveTo(_RenderTarget,
		points[0].x + x,
		points[0].y + y);

	for (size_t i = 1; i < points.size(); i++)
	{
		const DUIPoint& thisPoint = points[i];
		// const DUIPoint& nextPoint = points[i + 1];

		nvgLineTo(_RenderTarget, thisPoint.x + x, thisPoint.y + y);
		// nvgLineTo(_RenderTarget, nextPoint.x + x, nextPoint.y + y);

		// DrawLine(
		// 	thisPoint.x + x, thisPoint.y + y,
		// 	nextPoint.x + x, nextPoint.y + y,
		// 	color, thickness);
	}

	if (complete_shape)
	{
		// nvgMoveTo(_RenderTarget, points.front().x + x, points.front().y + y);
		// nvgLineTo(_RenderTarget, points.back().x + x, points.back().y + y);
		nvgClosePath(_RenderTarget);
	}

	nvgStrokeWidth(_RenderTarget, thickness);
	nvgStrokeColor(_RenderTarget, color);
	nvgStroke(_RenderTarget);
}

void Graphics::FillOutlinedRoundedRectangle(
	float x, float y,
	float width, float height,
	Color fill, Color outline,
	float cornerRadius)
{
	FillRoundedRectangle(x, y, width, height, fill, cornerRadius);
	DrawRoundedRectangle(x, y, width, height, outline, cornerRadius);
}

void Graphics::FillOutlinedRoundedRectangle(
	float x, float y,
	float width, float height,
	Color fill, Color outline,
	DUIInsets cornerRadius)
{
	FillRoundedRectangle(x, y, width, height, fill, cornerRadius);
	DrawRoundedRectangle(x, y, width, height, outline, cornerRadius);
}

void Graphics::SetOpacity(float opacity) {
	nvgGlobalAlpha(_RenderTarget, opacity);
}

Color Graphics::ColorBrightnessOffset(
	Color original,
	float factor)
{
	return {
		original.r + factor,
		original.g + factor,
		original.b + factor,
		original.a
	};
}

void Graphics::PushRectangleClip(
	int width, int height)
{
	// D2D1_RECT_F clip_rect = { -1, -1, width, height + 1 };
	//
	// const D2D1_ANTIALIAS_MODE aa_mode =
	// 	D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
	//
	// _RenderTarget->PushAxisAlignedClip(
	// 	clip_rect, aa_mode
	// );

	// nvgSave(_RenderTarget);
	// nvgScissor(_RenderTarget, 0, 0, width, height);

	nvgIntersectScissor(_RenderTarget, 0, 0, width, height);
}

void Graphics::DrawRoundedClipMask(const RoundedClipState& clip) {
	// If your DUIInsets field names differ, just map them here.
	// For example:
	// left   = top-left
	// top    = top-right
	// right  = bottom-right
	// bottom = bottom-left

	const float topLeftRadius = clip.cornerRadius.left;
	const float topRightRadius = clip.cornerRadius.top;
	const float bottomRightRadius = clip.cornerRadius.right;
	const float bottomLeftRadius = clip.cornerRadius.bottom;

	nvgBeginPath(_RenderTarget);

	// Outer rectangle: the whole clipped box.
	nvgRect(
		_RenderTarget,
		clip.x,
		clip.y,
		clip.width,
		clip.height);

	// Inner rounded rectangle: the area we want to KEEP.
	nvgRoundedRectVarying(
		_RenderTarget,
		clip.x,
		clip.y,
		clip.width,
		clip.height,
		topLeftRadius,
		topRightRadius,
		bottomRightRadius,
		bottomLeftRadius);

	// Make the rounded rectangle a hole.
	nvgPathWinding(_RenderTarget, NVG_HOLE);

	nvgFillColor(
		_RenderTarget,
		nvgRGBAf(
			clip.outsideFill.r,
			clip.outsideFill.g,
			clip.outsideFill.b,
			clip.outsideFill.a));

	nvgFill(_RenderTarget);
}

void Graphics::PushRoundedRectangleClip(
	float     x,
	float     y,
	float     width,
	float     height,
	DUIInsets cornerRadius,
	Color     outsideFill
) {
	nvgSave(_RenderTarget);

	// Cheap broad clip. NanoVG scissor is still rectangular.
	nvgIntersectScissor(
		_RenderTarget,
		x,
		y,
		width,
		height);

	roundedClipStack.push_back({
		x,
		y,
		width,
		height,
		cornerRadius,
		outsideFill,
		true
	});
}

void Graphics::PopClip() {
	if (!roundedClipStack.empty()) {
		RoundedClipState clip = roundedClipStack.back();
		roundedClipStack.pop_back();

		if (clip.rounded) {
			DrawRoundedClipMask(clip);
		}
	}

	nvgRestore(_RenderTarget);
}

void Graphics::PopRectangleClip()
{
	nvgIntersectScissor(_RenderTarget, 0, 0, FLT_MAX, FLT_MAX);
	// nvgRestore(_RenderTarget);
}

// void Graphics::PushRoundedRectangleClip(
// 	float x,
// 	float y,
// 	float width,
// 	float height,
// 	DUIInsets cornerRadius)
// {
// 	// NanoVG exposes rectangular scissor clipping only.
// 	// Keep rounded radii in the signature so callers can opt into a dedicated
// 	// rounded-clip API now, while preserving behavior until backend mask/stencil
// 	// clipping is wired.
// 	(void)cornerRadius;
//
// 	nvgSave(_RenderTarget);
// 	nvgIntersectScissor(_RenderTarget, x, y, width, height);
// }
//
// void Graphics::PopRoundedRectangleClip()
// {
// 	nvgRestore(_RenderTarget);
// }

void Graphics::SetScale(float scale) {
	// nvgTranslate(_RenderTarget, cx, cy);
	nvgScale(_RenderTarget, scale, scale);
	// nvgTranslate(_RenderTarget, -cx, -cy);
}

void Graphics::FillRadialGradient(
	float x, float y,
	float inner_radius, float outer_radius,
	Color inner, Color outer) {

	NVGpaint paint = nvgRadialGradient(
		_RenderTarget,
	    x, y,
	    inner_radius,
	    outer_radius,
	    inner, outer
	);

	nvgBeginPath(_RenderTarget);
	nvgCircle(_RenderTarget, x, y, outer_radius);
	nvgFillPaint(_RenderTarget, paint);
	nvgFill(_RenderTarget);

}

void Graphics::FillGradientEllipse(
	float x, float y,
	float x_start, float y_start,
	float x_end, float y_end,
	float radius,
	Color col1, Color col2) {

	NVGpaint paint = nvgLinearGradient(
		_RenderTarget,
		x + x_start - radius, y + y_start - radius,
		x + x_end, y + y_end,
		col1, col2
	);

	nvgBeginPath(_RenderTarget);
	nvgCircle(_RenderTarget, x, y, radius);
	nvgFillPaint(_RenderTarget, paint);
	nvgFill(_RenderTarget);
}

void Graphics::SetBackdropBlurSource(
	int image,
	DUISize textureSize)
{
	backdropBlurImage = image;
	backdropBlurTextureSize = textureSize;
}

int Graphics::GetBackdropBlurImage() const
{
	return backdropBlurImage;
}

DUISize Graphics::GetBackdropBlurTextureSize() const
{
	return backdropBlurTextureSize;
}
