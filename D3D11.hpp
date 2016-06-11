#pragma once
#include<wrl.h>
#include<atlbase.h>
#include<d3d11_1.h>

#pragma comment(lib,"d3d11.lib")
namespace Ark {
	class D3D11 {
		Microsoft::WRL::ComPtr<ID3D11Device> D3D11Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> D3D11DevContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> DXGISwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
		//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
		RECT size;
		const int Set_RGB(COLORREF& color) {
			int i = color % 0x100;
			color /= 0x100;
			return i;
		}
		void Convert_RGB(int& r, int& g, int& b, COLORREF& color) {
			r = Set_RGB(color);
			g = Set_RGB(color);
			b = Set_RGB(color);
		}
		const bool CompareRect(const RECT& f, const RECT& s) {
			if (f.bottom == s.bottom && f.left == s.left && f.right == s.right && f.top == s.top)return true;
			else return false;
		}
		template<typename T>
		const auto Convert_RGBA(COLORREF color, const T f) {
			int r, g, b;
			Convert_RGB(r,g,b,color);
			const T result[4]{ static_cast<T>(r / 255),static_cast<T>(g / 255),static_cast<T>(b / 255),f };
			return result;
		}
	public:
		D3D11(){
			CoInitialize(nullptr);
		}
		~D3D11(){
			CoUninitialize();
		}
		void Begin_Draw(HWND hwnd) {
			RECT rect;
			GetClientRect(hwnd,&rect);
			if (!CompareRect(size,rect)) {
				DXGISwapChain.ReleaseAndGetAddressOf();
				RenderTargetView.ReleaseAndGetAddressOf();
				//ShaderResourceView.ReleaseAndGetAddressOf();
			}
			if (!D3D11Device || !D3D11DevContext) {
				GetClientRect(hwnd, &size);
				D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED, nullptr, 0, D3D11_SDK_VERSION, &D3D11Device, nullptr, &D3D11DevContext);
			}
			if(!DXGISwapChain||!RenderTargetView){
				Microsoft::WRL::ComPtr<IDXGIDevice1> DXGIDevice;
				Microsoft::WRL::ComPtr<IDXGIAdapter> DXGIAdapter;
				Microsoft::WRL::ComPtr<IDXGIFactory2> DXGIFactory;
				Microsoft::WRL::ComPtr<IDXGISurface> DXGISurface;
				Microsoft::WRL::ComPtr<ID3D11Texture2D> D3DTexture;
				D3D11Device.As(&DXGIDevice);
				DXGIDevice->GetAdapter(&DXGIAdapter);
				DXGIAdapter->GetParent(IID_PPV_ARGS(&DXGIFactory));
				DXGIDevice->SetMaximumFrameLatency(1);

				DXGI_SWAP_CHAIN_DESC1 desc = { 0 };
				desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT|DXGI_USAGE_SHADER_INPUT;
				desc.BufferCount = 2;
				desc.Scaling = DXGI_SCALING_NONE;
				desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
				DXGIFactory->CreateSwapChainForHwnd(D3D11Device.Get(), hwnd, &desc, nullptr, nullptr, &DXGISwapChain);
				DXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&D3DTexture));
				D3D11Device->CreateRenderTargetView(D3DTexture.Get(), nullptr, &RenderTargetView);
				//D3D11Device->CreateDepthStencilView();
				//D3D11Device->CreateShaderResourceView(D3DTexture.Get(),nullptr,&ShaderResourceView);
				D3D11_VIEWPORT vp;
				vp.Width = size.right - size.left;
				vp.Height = size.bottom - size.top;
				vp.MinDepth = 0.0f;
				vp.MaxDepth = 1.0f;
				vp.TopLeftX = 0.0f;
				vp.TopLeftY = 0.0f;
				D3D11DevContext->RSSetViewports(1, &vp);
			}
		}
		void End_Draw() {
			DXGISwapChain->Present(0, 0);
		}

		void Draw_Clear(COLORREF color,const float f){
			auto it = Convert_RGBA<float>(color, f);
			D3D11DevContext->ClearRenderTargetView(RenderTargetView.Get(), it);
		}
	};
}