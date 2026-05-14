// #pragma once
// #include <kromakit/graphics/Graphics.h>
// #include <kromakit/DUIWindow.h>
//
// #include <d2d1effects.h>
// #include <map>
//
// class GaussianBlurProvider
// {
//   HRESULT hr;
//
//   DUIWindow* target_window;
//   Graphics* target_graphics;
//   Direct3D* target_d3d;
//   Control* target_control;
//
//   //Direct3D* offscr_window_capt_;
//   Graphics* offscr_window_capt_graphics = nullptr;
//   ComPtr<ID2D1DeviceContext> offscr_window_capt_ctx;
//   ComPtr<ID2D1Bitmap1> offscr_window_capt;
//
//   ComPtr<ID2D1Effect> blurEffect;
//   ComPtr<ID2D1Layer> roundedRectLayer;
//   ComPtr<ID2D1PathGeometry> roundedRectGeometry;
//
//   void CreateRenderTarget();
//   void CaptureWindow();
//
//   void Init();
//
//   void CreateRRLayer();
//
//   const std::map<Control*, bool> HideControlsAbove();
//
//   float CornerRadiusX = 100;
//   float CornerRadiusY = 100;
//
// public:
//
//   void SetCornerRadius(float xy);
//   void SetCornerRadius(float x, float y);
//   void RenderBlur();
//
//   GaussianBlurProvider(
//     DUIWindow* target_window,
//     Graphics* target_graphics,
//     Direct3D* target_d3d,
//     Control* target_control);
// };
