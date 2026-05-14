// #include "Application.h"
//
// #include "Utils.h"
//
// Application* Application::pCurrent = nullptr;
//
// void Application::init_window()
// {
//   pWindow = new DUIWindow();
//
//
// }
//
//
// bool Application::UserInitialise()
// {
//   DirectUIUtils::Log("Error: Please create a child class of `Application` and override Application::UserInitialise()\n");
//   return false;
// }
//
// bool Application::UserInitialised()
// {
//   return true;
// }
//
// Application::~Application()
// {
//   if (pWindow != nullptr)
//     delete pWindow;
//
//   // SafeRelease(pD2DFactory);
//   pCurrent = nullptr;
// }
//
// Application::Application()
// {
//   // printf("Warning: Default parameters used in Window constructor.\n");
// }
//
// int Application::Start()
// {
//   init_window();
//
//
//   if (pWindow == nullptr)
//   {
//     OnError();
//     return 1;
//   }
//
//   if (!UserInitialise())
//     return 1;
//
//   // pWindow->Show();
//   pWindow->GenerateWindow(
//     "Application");
//
//   if (!UserInitialised())
//     return 1;
//
//   MessageLoop();
//
//   return 0;
// }
//
// // int Application::StartRenderLoop()
// // {
// //   if (pWindow == nullptr)
// //   {
// //     OnError();
// //     return 1;
// //   }
// //
// //   if (!UserInitialise())
// //     return 1;
// //
// //   pWindow->Show();
// //   if (!UserInitialised())
// //     return 1;
// //
// //   RenderLoop();
// //
// //   return GetLastError();
// // }
//
// void Application::MessageLoop()
// {
//   // XEvent event;
//
//   // while (true)
//   // {
//   //   BOOL result = GetMessage(&msg, NULL, 0, 0);
//   //
//   //   if (result <= 0 ||
//   //       !IsWindow(pWindow->hWnd))
//   //     break;
//   //
//   //   TranslateMessage(&msg);
//   //   DispatchMessage(&msg);
//   // }
//
//   while (!pWindow->ShouldWindowClose())
//   {
//     pWindow->PollEvents();
//     pWindow->WindowRender();
//   }
// }
// // void Application::RenderLoop()
// // {
// //   MSG msg = { };
// //   while (1)
// //   {
// //     if (!IsWindow(pWindow->hWnd))
// //       break;
// //
// //     if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
// //     {
// //       TranslateMessage(&msg);
// //       DispatchMessage(&msg);
// //     }
// //
// //     pWindow->WindowRender();
// //   }
// // }
//
