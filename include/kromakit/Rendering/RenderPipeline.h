#pragma once

#include <kromakit/Rendering/render_state/LayerView.h>
#include <kromakit/Rendering/render_state/RenderCache.h>
#include <kromakit/Effects/BackdropBlurManager.h>

class DUIWindow;
class Graphics;

class RenderPipeline {
public:
	RenderPipeline() = default;
	~RenderPipeline() = default;

	void RenderWindow(DUIWindow* window, float deltaTime);

private:
	void ConfigureGraphicsState(DUIWindow* window, Graphics* graphics, float deltaTime) const;
	void BuildRenderLists(DUIWindow* window, Graphics* graphics);
	void RebuildRenderItems(DUIWindow* window);
	void RebuildLayerViews();
	void RefreshLayerOrderState();
	void SortLayer(LayerView& layer);
	void CullLayer(LayerView& layer, const DUISize& viewportSize);
	void ApplyOverlayOcclusion();
	void RenderLayer(Graphics* graphics, const LayerView& layer);

	bool OverlayNeedsBackdropBlur() const;

	void ExecuteContentCapturePass(DUIWindow* window, Graphics* graphics);
	void ExecuteOnscreenPass(
		DUIWindow* window,
		Graphics* graphics,
		bool blurAvailable
	);

	void BeginFrame(
		Graphics* graphics,
		NVGcolor clearColor
	) const;

	void EndFrame(Graphics* graphics) const;

	RenderCache cache_;
	BackdropBlurManager backdropBlurManager_;
};
