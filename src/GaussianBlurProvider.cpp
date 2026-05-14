// #include <kromakit/GaussianBlurProvider.h>
// #include <kromakit/Application.h>
//
// void GaussianBlurProvider::CreateRenderTarget()
// {
//   hr = target_d3d->_d2dDevice->CreateDeviceContext(
//     D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
//     &offscr_window_capt_ctx
//   );
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   D2D1_SIZE_U size = D2D1::SizeU(
//     target_control->size.width,
//     target_control->size.height);
//
//   D2D1_BITMAP_PROPERTIES1 props =
//     D2D1::BitmapProperties1(
//       D2D1_BITMAP_OPTIONS_TARGET,
//       D2D1::PixelFormat(
//         DXGI_FORMAT_B8G8R8A8_UNORM,
//         D2D1_ALPHA_MODE_PREMULTIPLIED
//       )
//     );
//
//   hr = target_d3d->_d2dContext->CreateBitmap(
//     size,        // Size of bitmap
//     nullptr, 0,  // No initial data
//     &props,
//     &offscr_window_capt);
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   offscr_window_capt_ctx->SetTarget(
//     offscr_window_capt.Get());
//
//   offscr_window_capt_graphics = new Graphics(
//     offscr_window_capt_ctx.Get(),
//     Application::pCurrent->pD2DFactory
//   );
//
//   Init();
// }
//
// void GaussianBlurProvider::CaptureWindow()
// {
//
//
//
//   offscr_window_capt_ctx->BeginDraw();
//
//   offscr_window_capt_ctx->Clear(ColorFromRGB(0, 0, 0, 0));
//
//   /*bool oldVisible = target_control->isVisible;
//   target_control->isVisible = false;*/
//   const std::map<Control*, bool> oldVisibleValues =
//     HideControlsAbove();
//
//
//
//   D2D_MATRIX_3X2_F transform;
//   offscr_window_capt_graphics->_RenderTarget->GetTransform(&transform);
//   transform.dx = -target_control->cachedScreenLocation.x;
//   transform.dy = -target_control->cachedScreenLocation.y;
//
//   offscr_window_capt_graphics->_RenderTarget->SetTransform(transform);
//
//   target_window->IsolatedWindowRender(offscr_window_capt_graphics, false);
//
//   transform.dx = transform.dy = 0;
//   offscr_window_capt_graphics->_RenderTarget->SetTransform(transform);
//
//   //offscr_window_capt_graphics->_RenderTarget->SetTransform();
//
//   for (auto vis : oldVisibleValues)
//   {
//     vis.first->isVisible = vis.second;
//   }
//
//   hr = offscr_window_capt_ctx->EndDraw();
//   if (FAILED(hr)) { OnError(hr); return; }
//
// }
//
// void GaussianBlurProvider::Init()
// {
//   HRESULT hr = target_d3d->_d2dContext->CreateEffect(
//     CLSID_D2D1GaussianBlur, &blurEffect);
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   /*HRESULT hr = target_d3d->_d2dContext->CreateEffect(
//     CLSID_D2D1, &blurEffect);
//
//   if (FAILED(hr)) { OnError(hr); return; }*/
//
//   blurEffect->SetInput(0, offscr_window_capt.Get());
//
//   float blurAmount = 8.0f;
//   hr = blurEffect->SetValue(
//     D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
//     blurAmount);
//
//   if (FAILED(hr)) { OnError(hr); return; }
// }
//
// void GaussianBlurProvider::CreateRRLayer()
// {
//   target_d3d->_d2dContext->CreateLayer(nullptr, &roundedRectLayer);
//
//   D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(
//     D2D1::RectF(0, 0, target_control->size.width, target_control->size.height),
//     100, 100);
//
//   target_d3d->graphics->_D2D1_Factory->CreatePathGeometry(&roundedRectGeometry);
//
//   if (SUCCEEDED(hr)) {
//     ComPtr<ID2D1GeometrySink> sink;
//     hr = roundedRectGeometry->Open(&sink);
//     if (SUCCEEDED(hr)) {
//
//       sink->BeginFigure(
//         D2D1::Point2F(CornerRadiusX, 0), // Start at top left, after corner
//         D2D1_FIGURE_BEGIN_FILLED
//       );
//       // Top edge
//       sink->AddLine(D2D1::Point2F(target_control->size.width - CornerRadiusX, 0));
//       // Top-right corner
//       sink->AddArc(D2D1::ArcSegment(
//         D2D1::Point2F(target_control->size.width, CornerRadiusY),
//         D2D1::SizeF(CornerRadiusX, CornerRadiusY),
//         0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
//       // Right edge
//       sink->AddLine(D2D1::Point2F(target_control->size.width, target_control->size.height - CornerRadiusY));
//       // Bottom-right corner
//       sink->AddArc(D2D1::ArcSegment(
//         D2D1::Point2F(target_control->size.width - CornerRadiusX, target_control->size.height),
//         D2D1::SizeF(CornerRadiusX, CornerRadiusY),
//         0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
//       // Bottom edge
//       sink->AddLine(D2D1::Point2F(CornerRadiusX, target_control->size.height));
//       // Bottom-left corner
//       sink->AddArc(D2D1::ArcSegment(
//         D2D1::Point2F(0, target_control->size.height),
//         D2D1::SizeF(CornerRadiusX, CornerRadiusY),
//         0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
//       // Left edge
//       sink->AddLine(D2D1::Point2F(0, CornerRadiusY));
//       // Top-left corner to close
//       sink->AddArc(D2D1::ArcSegment(
//         D2D1::Point2F(CornerRadiusX, 0),
//         D2D1::SizeF(CornerRadiusX, CornerRadiusY),
//         0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
//
//       sink->EndFigure(D2D1_FIGURE_END_CLOSED);
//       sink->Close();
//     }
//   }
// }
//
// const std::map<Control*, bool>
//   GaussianBlurProvider::HideControlsAbove()
// {
//   std::map<Control*, bool> old_visible_values;
//
//   auto& parent_children = target_control->parent->Children;
//   auto target_it = std::find_if(
//     parent_children.begin(),
//     parent_children.end(),
//     [target_control](const Control* child) { return child == target_control; });
//
//   for (auto it = parent_children.begin();
//     it != parent_children.end(); it++)
//   {
//     Control* ctrl = *it;
//     if (it >= target_it)
//     {
//       // above or equal
//       old_visible_values.emplace(
//         ctrl, ctrl->isVisible);
//
//       ctrl->isVisible = false;
//     }
//   }
//
//   return old_visible_values;
// }
//
// void GaussianBlurProvider::SetCornerRadius(float xy)
// {
//   SetCornerRadius(xy, xy);
// }
//
// void GaussianBlurProvider::SetCornerRadius(float x, float y)
// {
//   CornerRadiusX = x;
//   CornerRadiusY = y;
// }
//
// void GaussianBlurProvider::RenderBlur()
// {
//   if (!offscr_window_capt_ctx)
//   {
//     CreateRenderTarget();
//     //CreateRRLayer();
//   }
//
//   CaptureWindow();
//
//   //D2D1_LAYER_PARAMETERS1 params = D2D1::LayerParameters1(
//   //  D2D1::InfiniteRect(),
//   //  roundedRectGeometry.Get(),  // your rounded rect geometry
//   //  D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
//   //  D2D1::IdentityMatrix(),
//   //  1.0f,
//   //  nullptr,
//   //  D2D1_LAYER_OPTIONS1_NONE
//   //);
//
//   //target_graphics->_RenderTarget->PushLayer(
//   //  params, roundedRectLayer.Get());
//
//
//
//   target_graphics->PushRectangleClip(
//     target_control->size.width,
//     target_control->size.height);
//
//   target_graphics->_RenderTarget->DrawImage(
//     blurEffect.Get(),        // Your offscreen bitmap,
//     { 0,0 }, { 0, 0, target_control->size.width,
//     target_control->size.height }
//   );
//
//   target_graphics->PopRectangleClip();
//   //target_graphics->_RenderTarget->PopLayer();
// }
//
// GaussianBlurProvider::GaussianBlurProvider(
//   Window* target_window,
//   Graphics* target_graphics,
//   Direct3D* target_d3d,
//   Control* target_control)
// {
//   this->target_window = target_window;
//   this->target_graphics = target_graphics;
//   this->target_d3d = target_d3d;
//   this->target_control = target_control;
// }
