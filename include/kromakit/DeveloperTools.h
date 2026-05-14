// #pragma once
// #include <kromakit/DUIWindow.h>
// #include <sstream>
// #include <kromakit/ControlPreview.h>
// #include <kromakit/text_input/TextBox.h>
//
// class DeveloperTools :
//     public DUIWindow
// {
//   DUIWindow* target_window = nullptr;
//
//   Label* highlightedControlLabel = nullptr;
//
//   Control *hoverControl = nullptr,
//           *lastHoverControl = nullptr;
//
//   ControlPreview ctrl_preview;
//   TextBox* target_sContent_field = nullptr;
//   Button* SelectModeBtn = nullptr;
//
//   bool selectMode = false;
//
// public:
//   bool windowIsVisible = false;
//
//   DeveloperTools(DUIWindow* window_instance);
//
//   void ShowDevTools();
//   void CloseDevTools();
//   void UnselectSelectedControl();
//
//   void OnTargetWindowProc(
//     UINT uMsg, WPARAM wParam, LPARAM lParam
//   );
//
//   void UpdatePos();
//
//   void OnRender(Graphics* graphics) override;
//   void DoLayout(Graphics* renderTarget) override;
//
// private:
//   void OnTargetWindowMouseMove(
//     int x, int y);
// };
//
