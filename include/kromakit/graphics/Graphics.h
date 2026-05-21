#pragma once
#include <cstdio>
#include <cstdint>
#include <vector>
#include <cstdio>
#include <string>

#include <kromakit/DUIFont.h>

#include <nanovg.h>
#include <kromakit/graphics/svg/SvgDocument.h>
#include <kromakit/graphics/svg/SvgResourceCache.h>

#include <kromakit/graphics/types/DUIGeometry.h>
#include <kromakit/graphics/types/LineSegment.h>
#include <optional>

typedef NVGpaint ColorBrush;
typedef NVGcolor Color;

inline const Color Lighten(Color col, float value) {
	return {
		col.r + value,
		col.g + value,
		col.b + value,
		col.a
	};
}

inline const Color Alpha(Color col, float value) {
	return {
		col.r,
		col.g,
		col.b,
		value
	};
}

inline const Color Lighten(Color col, int value) {
	return Lighten(col, (float)value / 255.0f);
}

inline const Color Alpha(Color col, int value) {
	return Alpha(col, (float)value / 255.0f);
}

inline const Color ColorFromRGB(
	uint16_t r, uint16_t g,
	uint16_t b, uint16_t a = 255)
{
	return Color{
		r / 255.0f,
		g / 255.0f,
		b / 255.0f,
		a / 255.0f
	};
}

inline const Color ColorFromRGB(
	uint16_t grayscale)
{
	return ColorFromRGB(
		grayscale,
		grayscale,
		grayscale,
		255
	);
}

inline const Color ColorFromRGB(
	uint16_t grayscale, uint16_t alpha)
{
	return ColorFromRGB(
		grayscale,
		grayscale,
		grayscale,
		alpha
	);
}

struct RoundedClipState {
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;

	DUIInsets cornerRadius = DUIInsets(0);

	Color outsideFill = ColorFromRGB(0);
	bool rounded = false;
};


class Control;

class Graphics
{
private:
	void RenderFailedFontResolve(float x, float y) const;

public:
	float DeltaTime = 0.0f;
	DUISize viewportSize{0,0};
	DUIInsets viewportSafeArea{};
	float viewportPixelRatio = 1.0f;
	int backdropBlurImage = 0;
	DUISize backdropBlurTextureSize{0,0};

	std::vector<RoundedClipState> roundedClipStack;

	const float GetFPS() const { return 1.0f / DeltaTime; }

	void FillPolygon(const std::vector<DUIPoint> &points, float x, float y, Color color);


	Graphics(NVGcontext* rendTarget);

	NVGcontext* _RenderTarget;
	ColorBrush* Brush;

	void RenderShadow(
		Color col, float x, float y,
		float w, float h, float radius,
		float feather,
		float offsetX, float offsetY);

	void RenderBasicShadow(DUISize size);

	void SaveDrawingState();
	void RestoreDrawingState();

	void RotateTransformAt(
		float rotate,
		float x, float y
	);

	void FillRectangle(
		float x, float y,
		float width, float height,
		Color fill
	);

	void DrawRectangle(
		float x, float y,
		float width, float height,
		Color color
	);

	void RenderText(
		const char *text,
		DUIFont font, Color fill,
		float x, float y
	) const;

	void RenderTextBox(
		const char* text,
		DUIFont font, Color fill,
		float x, float y, float width
	) const;

	DUISize CalculateTextSize(
		const char* text,
		DUIFont font
	);

	float CalculateTextAdvance(const char *text, DUIFont font) const;

	DUISize CalculateTextBoxSize(
		const char *text, float width, DUIFont font);

	[[deprecated("This function causes a bottleneck, please don't use it...")]]
	void RenderTextCentered(
		const char* text,
		DUIFont font, Color fill,
		Control* control
	);

	void DrawRoundedRectangleBorder(
		float x, float y, float w, float h,
		Color color, DUIInsets radius, DUIInsets border);

	void FillControlBackground(
		Control* control
	);

	void DrawErrorX(
		Control* control,
		const char* str = nullptr
	);

	void FillRoundedRectangle(
		float x, float y,
		float width, float height,
		NVGpaint fill, float cornerRadius
	);

	void FillRoundedRectangle(
		float x, float y,
		float width, float height,
		NVGpaint fill, DUIInsets cornerRadius
	);

	void FillRoundedRectangleLinearGradient(
		float x, float y, float x_end, float y_end,
		float width, float height,
		Color col1, Color col2, float cornerRadius);
	
	void FillRoundedRectangle(
		float x, float y,
		float width, float height,
		Color fill, float cornerRadius
	);

	void FillRoundedRectangle(
		float x, float y,
		float width, float height,
		Color fill, DUIInsets cornerRadius
	);

	void FillEllipse(
		float center_x, float center_y,
		float radius, Color fill
	);

	void DrawEllipse(
		float center_x, float center_y,
		float radius, Color color
	);

	void DrawRoundedRectangle(
		float x, float y,
		float width, float height,
		Color color, float cornerRadius
	) const;

	void DrawRoundedRectangle(
		float x, float y,
		float width, float height,
		Color color, DUIInsets cornerRadius
	);

	void DrawLine(
		float x1, float y1,
		float x2, float y2,
		Color color, float thickness = 1.0f
	);

	void DrawLines(
		const std::vector<LineSegment>& lines,
		float x, float y,
		Color color, float width);

	void DrawLinesAsPath(
		const std::vector<DUIPoint>& points,
		float x, float y,
		Color color, bool complete_shape = true,
		float thickness = 1.0f
	);

	void FillOutlinedRoundedRectangle(
		float x, float y,
		float width, float height,
		Color fill, Color outline,
		float cornerRadius
	);

	void FillOutlinedRoundedRectangle(
		float x, float y,
		float width, float height,
		Color fill, Color outline,
		DUIInsets cornerRadius
	);

	void SetOpacity(float opacity);

	Color ColorBrightnessOffset(
		Color original,
		float factor
	);

	void PopRectangleClip();
	void PushRectangleClip(
		int width, int height
	);

	void DrawRoundedClipMask(const RoundedClipState &clip);

	void PushRoundedRectangleClip(
		float     x,
		float     y,
		float     width,
		float     height,
		DUIInsets cornerRadius,
		Color     outsideFill
	);

	void PopClip();

	void SetScale(float scale);

	void FillRadialGradient(
		float x, float y,
		float inner_radius, float outer_radius,
		Color inner, Color outer);

	void FillGradientEllipse(
		float x, float y,
		float x_start, float y_start,
		float x_end, float y_end,
		float radius,
		Color col1, Color col2);

	void SetBackdropBlurSource(
		int image,
		DUISize textureSize
	);

	int GetBackdropBlurImage() const;

	DUISize GetBackdropBlurTextureSize() const;


	void RenderSVG(const SvgDocument& svg, float x, float y, float width, float height, std::optional<Color> tint = std::nullopt);
	void RenderSVG(const SvgDocument& svg, DUIPoint location, DUISize size, std::optional<Color> tint = std::nullopt);
	void RenderSVG(const SvgDocument& svg, DUIRect rect, std::optional<Color> tint = std::nullopt);
	std::shared_ptr<SvgDocument> LoadSVG(const std::string& path);
};




