#pragma once
#include<wrl.h>
#include<atlbase.h>
#include<d3d11_1.h>

#pragma comment(lib,"d3d11.lib")
namespace Ark {
	class D3D {
		Microsoft::WRL::ComPtr<ID3D11Device> D3D11Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> D3D11DevContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> DXGISwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
		void Begin_Draw(HWND hwnd) {
			RECT rect;
			GetClientRect(hwnd, &rect);
			Microsoft::WRL::ComPtr<IDXGIDevice1> DXGIDevice;
			Microsoft::WRL::ComPtr<IDXGIAdapter> DXGIAdapter;
			Microsoft::WRL::ComPtr<IDXGIFactory2> DXGIFactory;
			Microsoft::WRL::ComPtr<IDXGISurface> DXGISurface;
			D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED, nullptr, 0, D3D11_SDK_VERSION, &D3D11Device, nullptr, nullptr);
			D3D11Device.As(&DXGIDevice);
			DXGIDevice->GetAdapter(&DXGIAdapter);
			DXGIAdapter->GetParent(IID_PPV_ARGS(&DXGIFactory));
			DXGIDevice->SetMaximumFrameLatency(1);

			DXGI_SWAP_CHAIN_DESC1 desc = { 0 };
			desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.BufferCount = 2;
			desc.Scaling = DXGI_SCALING_NONE;
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			DXGIFactory->CreateSwapChainForHwnd(D3D11Device.Get(), hwnd, &desc, nullptr, nullptr, &DXGISwapChain);
			DXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&DXGISurface));
			D3D11Device->CreateDeferredContext(0,&D3D11DevContext);
		}
	public:
		D3D(){
			CoInitialize(nullptr);
		}
		~D3D(){
			CoUninitialize();
		}
	};
}