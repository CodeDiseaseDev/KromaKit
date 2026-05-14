#include <kromakit/Rendering/RenderPipeline.h>
#include <kromakit/Rendering/RenderBackendHooks.h>

#include <kromakit/DUIWindow.h>
#include <kromakit/graphics/Graphics.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

namespace {
bool HasArea(const DUIRect& rect) {
	return rect.width > 0.01f && rect.height > 0.01f;
}

bool IsInView(const DUIRect& rect, const DUISize viewportSize)
{
	const DUIRect viewport {
		0,
		0,
		viewportSize.width,
		viewportSize.height
	};
	return rect.Intersects(viewport);
}

bool IsAncestorOrDescendantRange(const RenderItem& a, const RenderItem& b) {
	const bool aContainsB = a.treeStart <= b.treeStart && b.treeStart <= a.treeEnd;
	const bool bContainsA = b.treeStart <= a.treeStart && a.treeStart <= b.treeEnd;
	return aContainsB || bContainsA;
}

bool IsAncestorOrDescendantRange(
	const RenderOcclusionRegion& a,
	const RenderItem& b)
{
	const bool aContainsB = a.treeStart <= b.treeStart && b.treeStart <= a.treeEnd;
	const bool bContainsA = b.treeStart <= a.treeStart && a.treeStart <= b.treeEnd;
	return aContainsB || bContainsA;
}

bool SameViewport(const DUISize& lhs, const DUISize& rhs)
{
	return lhs == rhs;
}

DUIRect ScaleRectAroundPoint(
	const DUIRect& rect,
	const DUIPoint& origin,
	float scale)
{
	return {
		origin.x + (rect.x - origin.x) * scale,
		origin.y + (rect.y - origin.y) * scale,
		rect.width * scale,
		rect.height * scale
	};
}
}

void RenderPipeline::RenderWindow(DUIWindow* window, float deltaTime)
{
	if (window == nullptr || window->graphics == nullptr)
		return;

	Graphics* graphics = window->graphics;
	ConfigureGraphicsState(window, graphics, deltaTime);
	BuildRenderLists(window, graphics);

	const bool blurSupported = DirectUIRenderBackend::SupportsBackdropBlur();
	const bool needsBackdropBlur = blurSupported && OverlayNeedsBackdropBlur();
	if (needsBackdropBlur) {
		ExecuteContentCapturePass(window, graphics);
		backdropBlurManager_.ExecuteBlurPass();
	}

	ExecuteOnscreenPass(window, graphics, needsBackdropBlur);
}

void RenderPipeline::ConfigureGraphicsState(
	DUIWindow* window,
	Graphics* graphics,
	float deltaTime) const
{
	graphics->DeltaTime = deltaTime;
	graphics->viewportSize = {
		window->size.width,
		window->size.height
	};
	graphics->viewportSafeArea = window->safeArea;
	graphics->viewportPixelRatio = window->render_scale;
}

void RenderPipeline::BuildRenderLists(DUIWindow* window, Graphics* graphics)
{
	window->UpdateTree(graphics->DeltaTime);

	bool layoutWasDirty = false;
	if (window->isLayoutDirty) {
		layoutWasDirty = true;
		const float originalDeltaTime = graphics->DeltaTime;
		if (!window->StabilizeLayout(graphics)) {
			window->isLayoutDirty = true;
		}
		graphics->DeltaTime = originalDeltaTime;
	}

	const uint64_t treeVersion = window->GetRenderTreeVersion();
	const uint64_t layoutVersion = window->GetLayoutVersion();
	const uint64_t orderVersion = window->GetRenderOrderVersion();
	const uint64_t visualVersion = window->GetVisualVersion();
	const bool viewportChanged =
		!SameViewport(cache_.viewportSize, graphics->viewportSize) ||
		cache_.viewportPixelRatio != graphics->viewportPixelRatio;

	const bool rebuildRequired =
		cache_.items.empty() ||
		layoutWasDirty ||
		viewportChanged ||
		cache_.treeVersion != treeVersion ||
		cache_.layoutVersion != layoutVersion;

	if (rebuildRequired) {
		RebuildRenderItems(window);
		cache_.treeVersion = treeVersion;
		cache_.layoutVersion = layoutVersion;
		cache_.orderVersion = orderVersion;
		cache_.visualVersion = visualVersion;
		cache_.viewportSize = graphics->viewportSize;
		cache_.viewportPixelRatio = graphics->viewportPixelRatio;
	} else if (cache_.orderVersion != orderVersion) {
		RefreshLayerOrderState();
		cache_.content.orderDirty = true;
		cache_.content.cullDirty = true;
		cache_.overlay.orderDirty = true;
		cache_.overlay.cullDirty = true;
		cache_.orderVersion = orderVersion;
		cache_.visualVersion = visualVersion;
	} else if (cache_.visualVersion != visualVersion) {
		cache_.visualVersion = visualVersion;
	}

	SortLayer(cache_.content);
	SortLayer(cache_.overlay);
	CullLayer(cache_.content, graphics->viewportSize);
	CullLayer(cache_.overlay, graphics->viewportSize);
	ApplyOverlayOcclusion();
}

void RenderPipeline::RebuildRenderItems(DUIWindow* window)
{
	const size_t previousItemCount = cache_.items.size();
	cache_.ReserveItems(previousItemCount);
	cache_.ClearFrameLists();
	window->BuildRenderList(
		cache_.items,
		{0, 0},
		window->GetRectF()
	);
	RebuildLayerViews();
}

void RenderPipeline::RebuildLayerViews()
{
	cache_.content.Clear();
	cache_.overlay.Clear();
	cache_.content.Reserve(cache_.items.size());
	cache_.overlay.Reserve(cache_.items.size());

	for (size_t i = 0; i < cache_.items.size(); ++i) {
		const RenderItem& item = cache_.items[i];
		if (item.layer == RenderLayer::Overlay) {
			cache_.overlay.indices.push_back(i);
		} else {
			cache_.content.indices.push_back(i);
		}
	}
}

void RenderPipeline::RefreshLayerOrderState()
{
	for (RenderItem& item : cache_.items) {
		if (item.ctrl != nullptr)
			item.z = item.ctrl->zIndex;
	}
}

void RenderPipeline::SortLayer(LayerView& layer)
{
	if (!layer.orderDirty)
		return;

	std::stable_sort(
		layer.indices.begin(),
		layer.indices.end(),
		[this](size_t lhs, size_t rhs) {
			return cache_.items[lhs].z < cache_.items[rhs].z;
		});

	layer.orderDirty = false;
}

void RenderPipeline::CullLayer(
	LayerView& layer,
	const DUISize& viewportSize)
{
	if (!layer.cullDirty)
		return;

	cache_.occlusionScratch.clear();
	cache_.ReserveScratch(layer.indices.size());

	// Coverage must be evaluated front-to-back so top-most opaque controls
	// can cull only controls behind them. Layer indices are sorted in draw
	// order, so walk them backwards.
	for (auto it = layer.indices.rbegin(); it != layer.indices.rend(); ++it) {
		RenderItem& item = cache_.items[*it];
		const DUIRect cullRect = item.clipRect;

		if (!IsInView(cullRect, viewportSize)) {
			item.visible = false;
			continue;
		}

		bool fullyCovered = false;
		for (const RenderOcclusionRegion& region : cache_.occlusionScratch) {
			if (IsAncestorOrDescendantRange(region, item))
				continue;

			if (region.rect.ContainsRect(cullRect)) {
				fullyCovered = true;
				break;
			}
		}

		item.visible = !fullyCovered;

		if (item.visible && item.opaque) {
			cache_.occlusionScratch.push_back({
				cullRect,
				item.treeStart,
				item.treeEnd
			});
		}
	}

	layer.cullDirty = false;
}

void RenderPipeline::ApplyOverlayOcclusion()
{
	if (cache_.overlay.indices.empty())
		return;

	cache_.overlayOccluderIndices.clear();
	cache_.ReserveScratch(cache_.overlay.indices.size());

	for (size_t index : cache_.overlay.indices) {
		const RenderItem& overlay = cache_.items[index];
		if (!overlay.visible || !overlay.opaque || overlay.ctrl == nullptr)
			continue;
		if (!HasArea(overlay.clipRect))
			continue;

		cache_.overlayOccluderIndices.push_back(index);
	}

	if (cache_.overlayOccluderIndices.empty())
		return;

	for (size_t contentIndex : cache_.content.indices) {
		RenderItem& content = cache_.items[contentIndex];
		if (!content.visible || content.ctrl == nullptr)
			continue;
		if (!HasArea(content.clipRect))
			continue;

		for (size_t overlayIndex : cache_.overlayOccluderIndices) {
			const RenderItem& overlayItem = cache_.items[overlayIndex];

			if (IsAncestorOrDescendantRange(overlayItem, content))
				continue;

			if (overlayItem.clipRect.ContainsRect(content.clipRect)) {
				content.visible = false;
				break;
			}
		}
	}
}

bool RenderPipeline::OverlayNeedsBackdropBlur() const
{
	for (size_t index : cache_.overlay.indices) {
		const RenderItem& item = cache_.items[index];
		if (item.ctrl == nullptr)
			continue;

		if (item.ctrl->RequiresBackdropBlur())
			return true;
	}

	return false;
}

void RenderPipeline::ExecuteContentCapturePass(
	DUIWindow* window,
	Graphics* graphics)
{
	const int pixelWidth = std::max(
		1, static_cast<int>(std::lround(graphics->viewportSize.width * window->render_scale)));
	const int pixelHeight = std::max(
		1, static_cast<int>(std::lround(graphics->viewportSize.height * window->render_scale)));

	backdropBlurManager_.EnsureResources(
		graphics->_RenderTarget,
		pixelWidth,
		pixelHeight
	);

	backdropBlurManager_.BeginContentCapture();
	BeginFrame(graphics, nvgRGBAf(0, 0, 0, 0));

	graphics->SetBackdropBlurSource(0, {0, 0});
	RenderLayer(graphics, cache_.content);

	EndFrame(graphics);
	backdropBlurManager_.EndContentCapture();
}


static double SmoothValue(double current, double target, double amount) {
	return current + (target - current) * amount;
}

static std::string ToString1dp(double value) {
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(1) << value;
	return stream.str();
}

static void RenderDebugPerformanceText(
	Graphics* graphics,
	DUIWindow* window,
	double fps,
	double frameMs,
	double renderMs)
{
	if (graphics == nullptr || window == nullptr)
		return;

	std::string str = ToString1dp(fps);
	str += "fps";

	str += ", frame ";
	str += ToString1dp(frameMs);
	str += "ms";

	str += ", render ";
	str += ToString1dp(renderMs);
	str += "ms";

	str += ", layoutVersion ";
	str += std::to_string(window->rendInvalidation->layoutVersion);
	// str += "";


	constexpr float kPaddingX = 4.0f;
	constexpr float kPaddingY = 2.0f;

	auto debug_font = DUIFont(
		"Poppins", 12,
		FontWeight::SemiBold);

	const DUISize textSize =
		graphics->CalculateTextSize(str.c_str(), debug_font);

	const DUISize bgSize = {
		textSize.width + kPaddingX * 2.0f,
		textSize.height + kPaddingY * 2.0f
	};

	const float kX = graphics->viewportSafeArea.left;
	const float kY = graphics->viewportSize.height - graphics->viewportSafeArea.bottom - bgSize.height - 10.0f;



	const DUIRect bgRect = {
		kX, kY,
		bgSize.width,
		bgSize.height
	};

	graphics->FillRoundedRectangle(
		bgRect.x,
		bgRect.y,
		bgRect.width,
		bgRect.height,
		ColorFromRGB(255, 255, 255, 180),
		DUIInsets(0, 3, 0, 3)
	);

	graphics->RenderText(
		str.c_str(),
		debug_font,
		ColorFromRGB(0, 0, 0),
		bgRect.x + kPaddingX,
		bgRect.y + kPaddingY
	);
}

void RenderPipeline::ExecuteOnscreenPass(
	DUIWindow* window,
	Graphics* graphics,
	bool blurAvailable)
{
// #ifndef NDEBUG
	using Clock = std::chrono::steady_clock;

	static double smoothedFps = 0.0;
	static double smoothedFrameMs = 0.0;
	static double smoothedRenderMs = 0.0;
	static bool hasDebugSample = false;

	constexpr double kSmoothingAmount = 0.08;
// #endif

	DirectUIRenderBackend::BindDefaultFramebuffer();
	BeginFrame(graphics, nvgRGBAf(0, 0, 0, 1.0f));

	graphics->SetBackdropBlurSource(0, {0, 0});

// #ifndef NDEBUG
	const auto renderStartTime = Clock::now();
// #endif

	RenderLayer(graphics, cache_.content);

	if (blurAvailable) {
		graphics->SetBackdropBlurSource(
			backdropBlurManager_.GetBlurredImage(),
			graphics->viewportSize
		);
	}

	RenderLayer(graphics, cache_.overlay);
	graphics->SetBackdropBlurSource(0, {0, 0});

// #ifndef NDEBUG
	const auto renderEndTime = Clock::now();

	const double renderMs =
		std::chrono::duration<double, std::milli>(
			renderEndTime - renderStartTime
		).count();

	const double safeDeltaTime =
		graphics->DeltaTime > 0.0f
		? static_cast<double>(graphics->DeltaTime)
		: 1.0;

	const double fps = 1.0 / safeDeltaTime;
	const double frameMs = safeDeltaTime * 1000.0;

	if (!hasDebugSample) {
		smoothedFps = fps;
		smoothedFrameMs = frameMs;
		smoothedRenderMs = renderMs;
		hasDebugSample = true;
	} else {
		smoothedFps = SmoothValue(smoothedFps, fps, kSmoothingAmount);
		smoothedFrameMs = SmoothValue(smoothedFrameMs, frameMs, kSmoothingAmount);
		smoothedRenderMs = SmoothValue(smoothedRenderMs, renderMs, kSmoothingAmount);
	}

	RenderDebugPerformanceText(
		graphics,
		window,
		smoothedFps,
		smoothedFrameMs,
		smoothedRenderMs
	);
// #endif

	EndFrame(graphics);
}

void RenderPipeline::BeginFrame(
	Graphics* graphics,
	NVGcolor clearColor) const
{
	DirectUIRenderBackend::Clear(graphics->_RenderTarget, clearColor);
	nvgBeginFrame(
		graphics->_RenderTarget,
		graphics->viewportSize.width,
		graphics->viewportSize.height,
		graphics->viewportPixelRatio
	);
}

void RenderPipeline::EndFrame(Graphics* graphics) const
{
	nvgEndFrame(graphics->_RenderTarget);
}

void RenderPipeline::RenderLayer(
	Graphics* graphics,
	const LayerView& layer)
{
	for (size_t index : layer.indices) {
		RenderItem& item = cache_.items[index];
		if (!item.visible)
			continue;

		if (item.ctrl == nullptr || !item.ctrl->isVisible)
			continue;

		if (item.clipRect.width <= 0.0f || item.clipRect.height <= 0.0f)
			continue;

		Control* ctrl = item.ctrl;
		ctrl->cachedScreenLocation = {
			item.screenRect.x,
			item.screenRect.y
		};

		graphics->SaveDrawingState();
		graphics->SetOpacity(item.opacity);

		nvgResetTransform(graphics->_RenderTarget);
		nvgResetScissor(graphics->_RenderTarget);

		DUIRect clipRectForRender = item.clipRect;
		const bool hasInheritedScale =
			std::fabs(item.inheritedCentredScale - 1.0f) > 0.0001f;
		const bool isLocalScaleRoot =
			std::fabs(ctrl->GetCentredScale() - 1.0f) > 0.0001f;

		// Keep the scale-root item's clip in screen space so its own expanded
		// background can remain visible; descendants still receive scaled clips.
		if (hasInheritedScale && !isLocalScaleRoot) {
			clipRectForRender = ScaleRectAroundPoint(
				item.clipRect,
				item.inheritedCentredScaleOrigin,
				item.inheritedCentredScale);
		}

		nvgScissor(
			graphics->_RenderTarget,
			clipRectForRender.x,
			clipRectForRender.y,
			clipRectForRender.width,
			clipRectForRender.height);

		if (hasInheritedScale) {
			nvgTranslate(
				graphics->_RenderTarget,
				item.inheritedCentredScaleOrigin.x,
				item.inheritedCentredScaleOrigin.y);

			nvgScale(
				graphics->_RenderTarget,
				item.inheritedCentredScale,
				item.inheritedCentredScale);

			nvgTranslate(
				graphics->_RenderTarget,
				-item.inheritedCentredScaleOrigin.x,
				-item.inheritedCentredScaleOrigin.y);

			// DirectUIUtils::Log(
			// 	"render scale=%.5f origin=(%.3f, %.3f) visual=(%.3f, %.3f)\n",
			// 	item.inheritedCentredScale,
			// 	item.inheritedCentredScaleOrigin.x,
			// 	item.inheritedCentredScaleOrigin.y,
			// 	item.visualRect.x,
			// 	item.visualRect.y
			// );
		}

		nvgTranslate(
			graphics->_RenderTarget,
			item.visualRect.x,
			item.visualRect.y);

		ctrl->OnRender(graphics);

		if (ctrl->__debug__devtools_highlighted) {
			graphics->FillRectangle(
				0,
				0,
				ctrl->size.width,
				ctrl->size.height,
				ctrl->__debug__devtools_highlighted_color);

			graphics->DrawRectangle(
				0,
				0,
				ctrl->size.width,
				ctrl->size.height,
				ctrl->__debug__devtools_highlighted_color);
		}

		graphics->RestoreDrawingState();
	}
}
