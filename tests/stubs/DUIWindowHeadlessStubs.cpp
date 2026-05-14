#include <kromakit/DUIWindow.h>

DUIWindow::DUIWindow() {
  overlayStack = nullptr;
  contentStack = nullptr;
}

DUIWindow::~DUIWindow() = default;

void DUIWindow::MarkRenderTreeDirty() {}
void DUIWindow::MarkRenderOrderDirty() {}
void DUIWindow::MarkVisualDirty() {}
void DUIWindow::MarkLayoutVersionDirty() {}

uint64_t DUIWindow::GetRenderTreeVersion() const {
  return 0;
}

uint64_t DUIWindow::GetRenderOrderVersion() const {
  return 0;
}

uint64_t DUIWindow::GetVisualVersion() const {
  return 0;
}

uint64_t DUIWindow::GetLayoutVersion() const {
  return 0;
}

void DUIWindow::create_render_target(NVGcontext*) {}

std::string DUIWindow::GetControlName() const {
  return "DUIWindowStub";
}

const std::vector<std::string> DUIWindow::GetDebugOverlayItems() {
  return {};
}

void DUIWindow::SetSafeArea(float, float, float, float) {}
void DUIWindow::GenerateWindow(const char*) {}
void DUIWindow::WindowRender(float) {}
void DUIWindow::IsolatedWindowRender(Graphics*, bool) {}
void DUIWindow::Show() {}
void DUIWindow::SetSize(float width, float height) {
  Control::SetSize(width, height);
}
void DUIWindow::SetLocation(int, int) {}
void DUIWindow::Rerender() {}

DUIWindow* DUIWindow::GetRootWindow() {
  return this;
}

void DUIWindow::SetTitle(const char*) {}
DeveloperTools* DUIWindow::StartDeveloperToolsInstance() {
  return nullptr;
}
void DUIWindow::CloseCurrentDeveloperToolsInstance() {}
Control* DUIWindow::SolveScrollTarget(Control* ctrl) {
  return ctrl;
}
Control* DUIWindow::SolveClickTarget(Control* ctrl) {
  return ctrl;
}
Control* DUIWindow::SolveContextMenuTarget(Control* ctrl) const {
  return ctrl;
}
void DUIWindow::OnControlRemoved(Control*) {}
void DUIWindow::UpdateCursor(int, int) {}
void DUIWindow::UpdateHover(Control*, DUIPoint) {}

PointerEvent DUIWindow::BuildPointerEventForTarget(
  Control*,
  int,
  int,
  float,
  float,
  MouseButton
) const {
  return {};
}

WheelScrollEvent DUIWindow::BuildWheelScrollEventForTarget(
  Control*,
  float,
  float,
  bool
) const {
  return {};
}

void DUIWindow::UpdateGestureState(float, float) {}
void DUIWindow::DispatchMovement(Control*, DUIPoint, int, int, float, float) {}
void DUIWindow::OnMouseMove(int, int) {}
void DUIWindow::OnMouseDown(MouseButton) {}
void DUIWindow::OnMouseUp(MouseButton) {}
void DUIWindow::OnMouseWheel(float, float, bool) {}
void DUIWindow::OnMouseDown() {}
void DUIWindow::OnMouseUp() {}
void DUIWindow::OnMouseIn() {}
void DUIWindow::OnMouseOut() {}
void DUIWindow::OnKeyDown(int) {}
void DUIWindow::OnKeyUp(int) {}
void DUIWindow::OnResize() {}
void DUIWindow::OnRender(Graphics*) {}
void DUIWindow::DoLayout(Graphics*) {}
