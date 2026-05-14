// #pragma once
// #include <d3d11.h>
// #include <dxgi1_2.h>
// #include <d2d1_1.h>
//
// #include <wrl/client.h>
//
// #include <Windows.h>
//
// #include <kromakit/graphics/Graphics.h>
//
// #pragma comment(lib, "d3d11.lib")
//
// using Microsoft::WRL::ComPtr;
//
// class Direct3D
// {
// private:
//   HWND hWindow;
//
//   D3D_FEATURE_LEVEL featureLevelCreated;
//
//   static const D3D_FEATURE_LEVEL featureLevels[];
//   DXGI_SWAP_CHAIN_DESC1 scd = {};
//
//   UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
//   HRESULT hr = S_OK;
//
//   void DescribeWindowSwapChain(
//     HWND hWindow,
//     int width, int height
//   );
//
//   void ResizeD2D();
//   void InitD2D();
//
// public:
//   Graphics* graphics;
//
//   bool bmp_mode = false;
//
//   ComPtr<ID3D11Device> d3dDevice;
//   ComPtr<IDXGIDevice> dxgiDevice;
//   ComPtr<ID3D11DeviceContext> d3dContext;
//   ComPtr<IDXGIAdapter> dxgiAdapter;
//   ComPtr<IDXGIFactory2> dxgiFactory;
//
//   ComPtr<IDXGISwapChain1> swapChain;
//
//   ComPtr<ID2D1Bitmap1> d2dTargetBitmap;
//
//
//   // Direct2D1.1
//   ComPtr<ID2D1Device> _d2dDevice;
//   ComPtr<RenderTarget> _d2dContext;
//
//   // Direct2D1.1
//
//   void InitD3D(
//     HWND hWindow,
//     int width, int height
//   );
//
//   void Resize(int width, int height);
// };
