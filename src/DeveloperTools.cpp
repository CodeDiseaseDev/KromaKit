// #include <kromakit/DeveloperTools.h>
// #include <filesystem>
//
// DeveloperTools::DeveloperTools(
//   DUIWindow* window_instance) : DUIWindow("DirectUIDevTools")
// {
//   target_window = window_instance;
//
//   GenerateWindow(
//     "DirectUI Developer Tools");
//
//   DUIWindow::SetSize(500, 250);
//
//   highlightedControlLabel = CreateControl<Label>();
//   target_sContent_field = CreateControl<TextBox>();
//   SelectModeBtn = CreateControl<Button>();
//
//   highlightedControlLabel->autoWidth = true;
//   highlightedControlLabel->autoHeight = true;
//
//   target_sContent_field->sPlaceholderContent = "(empty)";
//   SelectModeBtn->sContent = "Select Control";
//
//   SelectModeBtn->OnClick = [&]() {
//     selectMode = !selectMode;
//
//     if (!selectMode)
//       UnselectSelectedControl();
//
//   };
//
//   //AddControl(&ctrl_preview);
//
//   UpdatePos();
// }
//
// void DeveloperTools::ShowDevTools()
// {
//   Show();
//   // BringWindowToTop(target_window->hWnd);
//   windowIsVisible = true;
// }
//
// void DeveloperTools::CloseDevTools()
// {
//   // ShowWindow(hWnd, SW_HIDE);
//   windowIsVisible = false;
//
//   UnselectSelectedControl();
// }
//
// void DeveloperTools::UnselectSelectedControl()
// {
//   if (hoverControl != nullptr)
//   {
//     hoverControl->__debug__devtools_highlighted = false;
//     hoverControl = nullptr;
//
//   }
// }
//
// void DeveloperTools::OnTargetWindowProc(
//   UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
//   /* background events  -  received while devtools is hidden */
//   switch (uMsg)
//   {
//   case WM_MOUSEMOVE:
//   {
//     int xPos = GET_X_LPARAM(lParam);
//     int yPos = GET_Y_LPARAM(lParam);
//     OnTargetWindowMouseMove(
//       xPos, yPos);
//   }
//   break;
//
//   case WM_MOVE:
//     UpdatePos();
//     break;
//   }
//   /* background events  -  received while devtools is hidden */
//
//
//
//   if (!windowIsVisible)
//     return;
//
//   switch (uMsg)
//   {
//   case WM_PAINT:
//     RerenderWindow();
//     break;
//   }
// }
//
// void DeveloperTools::UpdatePos()
// {
//   RECT rect;
//   GetWindowRect(target_window->hWnd, &rect);
//
//   SetLocation(
//     rect.right,
//     rect.top
//   );
// }
//
// void DeveloperTools::OnRender(Graphics* graphics)
// {
//
//
//   ctrl_preview.TargetControl = hoverControl;
// }
//
// void DeveloperTools::DoLayout(Graphics* renderTarget)
// {
//   target_sContent_field->location = {
//      10,
//      highlightedControlLabel->size.height + highlightedControlLabel->location.y
//   };
//
//   SelectModeBtn->location = {
//     target_sContent_field->location.x + target_sContent_field->size.width,
//     target_sContent_field->location.y
//   };
//
//   ctrl_preview.location = {
//     10,
//     target_sContent_field->size.height + target_sContent_field->location.y
//   };
// }
//
// void DeveloperTools::OnTargetWindowMouseMove(
//   int x, int y)
// {
//   if (!windowIsVisible)
//     return;
//
//   DUIPoint relativePos;
//   Control* ctrl = target_window->HitTestAt(
//     x, y, relativePos, false);
//
//   hoverControl = ctrl;
//
//   Color highlightedColor = ColorFromRGB(255, 255, 50, 120);
//
//   //printf("%i\n", (int)(hoverControl == nullptr));
//
//   if (hoverControl != lastHoverControl && hoverControl != nullptr)
//   {
//     if (selectMode)
//     {
//       hoverControl->__debug__devtools_highlighted = true;
//       hoverControl->__debug__devtools_highlighted_color = highlightedColor;
//     }
//
//
//     if (lastHoverControl != nullptr)
//       lastHoverControl->__debug__devtools_highlighted = false;
//
//     if (ctrl == nullptr)
//     {
//       highlightedControlLabel->sContent = L"(null)";
//     }
//     else
//     {
//
//       std::wstringstream wss;
//       wss << ctrl->GetControlName().value_or("<no name>").c_str() << std::endl;
//       wss << "Control ID: 0x" << std::hex << std::uppercase << ctrl->ControlID << std::endl;
//
//       wss << "Size: "
//         << std::fixed << std::setprecision(2) << ctrl->size.width << "x"
//         << std::fixed << std::setprecision(2) << ctrl->size.height << " (px)";
//
//       highlightedControlLabel->sContent = wss.str();
//
//
//
//       target_sContent_field->sContent = std::wstring(hoverControl->sContent);
//     }
//
//     RerenderWindow();
//     target_window->RerenderWindow();
//   }
//
//   lastHoverControl = hoverControl;
// }
