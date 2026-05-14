// #include <kromakit/old/Direct3D.h>
// #include <kromakit/Application.h>
//
// const D3D_FEATURE_LEVEL Direct3D::featureLevels[] =
// {
//     D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
//     D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
// };
//
// void Direct3D::DescribeWindowSwapChain(
//   HWND hWindow, int width, int height)
// {
//   scd.Width = width;   // e.g. 800
//   scd.Height = height; // e.g. 600
//   scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//   scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//   scd.BufferCount = 2;
//   scd.SampleDesc.Count = 1;
//   scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
//
//   hr = dxgiFactory->CreateSwapChainForHwnd(
//     d3dDevice.Get(),
//     hWindow,
//     &scd,
//     nullptr,
//     nullptr,
//     &swapChain
//   );
// }
//
// void Direct3D::ResizeD2D()
// {
//   ComPtr<IDXGISurface> dxgiBackBuffer;
//   hr = swapChain->GetBuffer(
//     0, IID_PPV_ARGS(&dxgiBackBuffer));
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   // 3. Create a D2D bitmap from the surface
//   D2D1_BITMAP_PROPERTIES1 bitmapProperties =
//     D2D1::BitmapProperties1(
//       D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
//       D2D1::PixelFormat(
//         DXGI_FORMAT_B8G8R8A8_UNORM,
//         D2D1_ALPHA_MODE_PREMULTIPLIED));
//
//   if (d2dTargetBitmap != nullptr)
//   {
//     _d2dContext->SetTarget(nullptr);
//     d2dTargetBitmap.Reset();
//   }
//
//   hr = _d2dContext->CreateBitmapFromDxgiSurface(
//     dxgiBackBuffer.Get(),
//     &bitmapProperties,
//     &d2dTargetBitmap
//   );
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   // 4. Set the new bitmap as the target
//   _d2dContext->SetTarget(d2dTargetBitmap.Get());
// }
//
// void Direct3D::InitD2D()
// {
//   HRESULT hr = Application::pCurrent->pD2DFactory->CreateDevice(
//     dxgiDevice.Get(), &_d2dDevice);
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   hr = _d2dDevice->CreateDeviceContext(
//     D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
//     &_d2dContext);
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   graphics = new Graphics(
//     _d2dContext.Get(),
//     Application::pCurrent->pD2DFactory
//   );
//
//   graphics->direct3D = this;
// }
//
// void Direct3D::InitD3D(
//   HWND hWindow, int width, int height)
// {
//   this->hWindow = hWindow;
//
//   hr = D3D11CreateDevice(
//     nullptr,                    // Use default adapter
//     D3D_DRIVER_TYPE_HARDWARE,   // Use hardware rendering
//     nullptr,                    // No software device
//     creationFlags,              // Device flags
//     featureLevels,              // Feature levels array
//     ARRAYSIZE(featureLevels),   // Num feature levels
//     D3D11_SDK_VERSION,          // SDK version
//     &d3dDevice,                 // Device out
//     &featureLevelCreated,       // Actual feature level
//     &d3dContext                 // Context out
//   );
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   d3dDevice.As(&dxgiDevice);
//   hr = dxgiDevice->GetAdapter(&dxgiAdapter);
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   hr = dxgiAdapter->GetParent(
//     __uuidof(IDXGIFactory2),
//     &dxgiFactory);
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   DescribeWindowSwapChain(
//     hWindow, width, height
//   );
//
//   InitD2D();
// }
//
//
// void Direct3D::Resize(int width, int height)
// {
//   //printf("resized to %i, %i\n", width, height);
//
//   _d2dContext->SetTarget(nullptr);
//   d2dTargetBitmap.Reset();
//
//   hr = swapChain->ResizeBuffers(
//     0, width, height,
//     DXGI_FORMAT_UNKNOWN, 0);
//
//   if (FAILED(hr)) { OnError(hr); return; }
//
//   ResizeD2D();
// }
