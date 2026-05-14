#pragma once

#include <kromakit/Control.h>
#include <kromakit/Rendering/RenderPipeline.h>
#include <memory>

#include <kromakit/Panel.h>
#include <kromakit/RootStackCoordinator.h>
#include <kromakit/overlay_background_layer/OverlayBackgroundLayer.h>
#include <kromakit/overlay_background_layer/OverlayStack.h>
#include <kromakit/core/properties/DUIInternalProperty.h>
#include <kromakit/event_types/keyboard/DUIKeyEvent.h>
#include <kromakit/event_types/keyboard/DUITextInputEvent.h>
#include <kromakit/overlay_background_layer/ContentStack.h>
#include <kromakit/Rendering/render_state/RenderInvalidationState.h>
// #include "KeyboardHandler.h"
// #include "Direct3D.h"



class DeveloperTools;
class ContextMenu;

using DUIPlatformWindowHandle = void*;

class DUIWindow :
		public Control
{

public:
	enum class GestureDirection {
		None,
		Horizontal,
		Vertical,
		Free
	};

	DUIInternalProperty<RenderInvalidationState, "rendInvalidation">
		rendInvalidation = propRegistry.NewInternalProperty
			<RenderInvalidationState, "rendInvalidation">();

private:
	void CloseWindow();
  void ResetContextMenuHoldTracking();
  void BeginContextMenuHoldIfAvailable(Control* target);
  void CancelContextMenuHoldTracking();
  Control* SolveContextMenuTarget(Control* ctrl) const;
  void UpdateContextMenuHoldState();
  bool TryPresentContextMenuForTarget(Control* target);
  void TeardownActiveHoldContextMenuPresentation();

	bool LoadedWindowYet = false;
	// KeyboardHandler keyboard;
	bool isClosed = false;

	OverlayStack* overlayStack;
	ContentStack* contentStack;

	// RootStackCoordinator manages the "zoom out" effect of the `ContentStack`
	RootStackCoordinator rootStackCoordinator;

public:
	OverlayStack* GetOverlayStack() {
		return overlayStack;
	}
	ContentStack* GetContentStack() {
		return contentStack;
	}



	// template <typename T>
	// T* AddControl(std::unique_ptr<T> control) {
	// 	static_assert(std::is_base_of_v<Control, T>, "T must derive from Control");
	// 	return contentStack.AddControl(std::move(control));
	// }
	// Control* AddControl(std::unique_ptr<Control> control) override {
	// 	return contentStack.AddControl(std::move(control));
	// }
	// Control* AddControl(Control* control) override {
	// 	return contentStack.AddControl(control);
	// }

	/*
	 * DUIWindow stores all of its overlays and content controls separately
	 * so we must intercept normal AddControl calls and redirect them to:
	 * `GetContentStack()->AddControl(...)`
	 */
	template<typename T, typename... Args>
		T* CreateControl(Args&&... args) {
		return contentStack->CreateControl<T>(std::forward<Args>(args)...);
	}
	void AddBorrowedControl(Control* control) override {
		contentStack->AddBorrowedControl(control);
	}




	void MarkRenderTreeDirty();
	void MarkRenderOrderDirty();
	void MarkVisualDirty();
	void MarkLayoutVersionDirty();

	uint64_t GetRenderTreeVersion() const;
	uint64_t GetRenderOrderVersion() const;
	uint64_t GetVisualVersion() const;
	uint64_t GetLayoutVersion() const;



	// Control *AddControl(std::unique_ptr<Control> control) override;

	std::function<void(int)> keyDownCallback;
	std::function<void(int)> keyUpCallback;

	DUIInsets safeArea {};

	Graphics* graphics = nullptr;

	DUIPlatformWindowHandle window = nullptr;

	// std::unique_ptr<OverlayBackgroundLayer> overlayLayer;

	Control* hoverControl = nullptr;
	Control* mouseHeldControl = nullptr;
	Control* selectedControl = nullptr;
	Control* gestureStartControl = nullptr;
	Control* activeGestureOwner = nullptr;
  Control* activeContextMenuHoldTarget = nullptr;
	bool activeGestureIsDrag = false;
  bool contextMenuHoldFired = false;
  DUIPoint contextMenuHoldStartPoint = { 0, 0 };
	GestureDirection gestureLock = GestureDirection::None;
	DUIPoint gestureStartPoint = { 0, 0 };
	DUIPoint gestureCurrentPoint = { 0, 0 };
	DUIPoint totalDelta = { 0, 0 };
  std::shared_ptr<ContextMenu> activeHoldContextMenu = nullptr;
  std::unique_ptr<OverlayBackgroundLayer> activeHoldContextMenuLayer = nullptr;
	std::unique_ptr<RenderPipeline> renderPipeline;

	// MessageBoxCtrl* msgBox = nullptr;

	// DeveloperTools* devtools_inst = nullptr;

	DUIPoint cursorLocation = { 0, 0 };
	MouseButton activeMouseButton = MouseButton::None;

	void create_render_target(NVGcontext* renderTarget);

	std::optional<std::string> GetControlName() const override;
	const std::vector<std::string> GetDebugOverlayItems() override;

	~DUIWindow();
	DUIWindow();

	void SetSafeArea(
		float left, float top,
		float right, float bottom);

	void GenerateWindow(const char* title);
	virtual void WindowRender(float deltaTime);
	void IsolatedWindowRender(Graphics* graphics,
		bool cache_screen_loc = true);
	void Show();

	void SetSize(float width, float height) override;
	void SetLocation(int x, int y);

	void Rerender();

	virtual DUIWindow *GetRootWindow() override;

	void PollEvents() { }
	bool ShouldWindowClose() { return false; }

	// void ShowMessageBox(
	// 	const wchar_t* content,
	// 	const wchar_t* title
	// );




	void SetTitle(const char* title);

	DeveloperTools* StartDeveloperToolsInstance();
	void CloseCurrentDeveloperToolsInstance();

	Control *SolveScrollTarget(Control *ctrl);
	Control *SolveClickTarget(Control* ctrl);
	void OnControlRemoved(Control* removedRoot);

	void UpdateCursor(int x, int y);

	void UpdateHover(Control *ctrl, DUIPoint relativeLocation);

	PointerEvent BuildPointerEventForTarget(
		Control *target,
		int x,
		int y,
		float dx,
		float dy,
		MouseButton mouseButton = MouseButton::None) const;

	WheelScrollEvent BuildWheelScrollEventForTarget(
		Control* target,
		float deltaX,
		float deltaY,
		bool isPrecise) const;

	void UpdateGestureState(float deltaX, float deltaY);

	void DispatchMovement(Control *ctrl, DUIPoint relativeLocation, int x, int y, float dx, float dy);

	//protected:
	// virtual LRESULT HandleMessage(
	// 	UINT uMsg, WPARAM wParam,
	// 	LPARAM lParam
	// );

	// Control* SolveScrollEvent(Control* ctrl, float deltaX, float deltaY);

	void SetSelectedControl(Control* control);
	void ClearSelectedControl();

	Control* GetSelectedControl() const;
	bool IsControlSelected(const Control* control) const;

	void SelectControlFromPointerEvent(Control* control);

	virtual void OnMouseMove(int x, int y) override;
	void OnMouseDown(MouseButton mouseButton);
	void OnMouseUp(MouseButton mouseButton);
	void OnMouseWheel(float deltaX, float deltaY, bool isPrecise = false);

	virtual void OnMouseDown() override;
	virtual void OnMouseUp() override;
	virtual void OnMouseIn() override;
	virtual void OnMouseOut() override;

	virtual void OnKeyDown(const DUIKeyEvent &keyEvent) override;
	virtual void OnKeyUp(const DUIKeyEvent &keyEvent) override;
	virtual void OnTextInput(const DUITextInputEvent &textInputEvent) override;

	bool RouteShortcut(const DUIKeyEvent& keyEvent);

	[[deprecated]]
	virtual void OnKeyDown(int key) override;
	[[deprecated]]
	virtual void OnKeyUp(int key) override;

	virtual void OnResize();

	void OnRender(Graphics* rendTarget) override;
	void DoLayout(Graphics* renderTarget) override;
	bool NeedsLayoutWhenOwnSizeChanges() const override { return true; }
};
