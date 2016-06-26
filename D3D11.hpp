#pragma once
#include<wrl.h>
#include<atlbase.h>
#include<d3d11_1.h>

#pragma comment(lib,"d3d11.lib")
namespace Ark {
	class D3D11 {
		struct Vec3 {
			std::array<float, 3> pos;
			std::array<float, 4> color;
		};
		struct ComInitializer {
			ComInitializer() { CoInitialize(nullptr); }
			~ComInitializer() { CoUninitialize(); }
		};
		ComInitializer scom;
		RECT size;
		Microsoft::WRL::ComPtr<ID3D11Device1> D3D11Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> D3D11DevContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> DXGISwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> D3D11RenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> D3D11DepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> D3D11InputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> D3D11VertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> D3D11PixelShader;

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
		void CreateResource(HWND hwnd) {
			RECT rect;
			GetClientRect(hwnd,&rect);
			if(!CompareRect(size,rect)){
				size = rect;
				D3D11Device.ReleaseAndGetAddressOf();
				D3D11DevContext.ReleaseAndGetAddressOf();
				DXGISwapChain.ReleaseAndGetAddressOf();
				D3D11RenderTargetView.ReleaseAndGetAddressOf();
				D3D11DepthStencilView.ReleaseAndGetAddressOf();
				D3D11InputLayout.ReleaseAndGetAddressOf();
				D3D11VertexShader.ReleaseAndGetAddressOf();
				D3D11PixelShader.ReleaseAndGetAddressOf();
			}
			if (!D3D11Device || !D3D11DevContext) {
				Microsoft::WRL::ComPtr<ID3D11Device> Device;
				Microsoft::WRL::ComPtr<ID3D11DeviceContext> DevContext;
				D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED, nullptr, 0, D3D11_SDK_VERSION, &Device, nullptr, &DevContext);
				Device.As(&D3D11Device);
				DevContext.As(&D3D11DevContext);
				
				Microsoft::WRL::ComPtr<IDXGIDevice2> DXGIDevice;
				D3D11Device.As(&DXGIDevice);
				
				Microsoft::WRL::ComPtr<IDXGIAdapter> DXGIAdapter;
				DXGIDevice->GetAdapter(&DXGIAdapter);
				DXGIDevice->SetMaximumFrameLatency(1);
				
				Microsoft::WRL::ComPtr<IDXGIFactory2> DXGIFactory;
				DXGIAdapter->GetParent(IID_PPV_ARGS(&DXGIFactory));

				DXGI_SWAP_CHAIN_DESC1 desc{ 0 };
				desc.Width = size.right - size.left;
				desc.Height = size.bottom - size.top;
				desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				desc.BufferCount = 2;
				desc.Scaling = DXGI_SCALING_NONE;
				desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
				DXGIFactory->CreateSwapChainForHwnd(D3D11Device.Get(), hwnd, &desc, nullptr, nullptr, &DXGISwapChain);
				
				Microsoft::WRL::ComPtr<ID3D11Texture2D> D3D11RenderTargetTexture;
				DXGISwapChain->GetBuffer(0, IID_PPV_ARGS(&D3D11RenderTargetTexture));
				D3D11Device->CreateRenderTargetView(D3D11RenderTargetTexture.Get(), nullptr, &D3D11RenderTargetView);
				
				Microsoft::WRL::ComPtr<ID3D11Texture2D> D3D11DepthTexture;
				D3D11_TEXTURE2D_DESC DepthTextureDesc  {};
				DepthTextureDesc.Width = desc.Width;
				DepthTextureDesc.Height = desc.Height;
				DepthTextureDesc.MipLevels = 1;
				DepthTextureDesc.ArraySize = 1;
				DepthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				DepthTextureDesc.SampleDesc = desc.SampleDesc;
				DepthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
				DepthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				D3D11Device->CreateTexture2D(&DepthTextureDesc, nullptr, &D3D11DepthTexture);
				D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc{};
				DepthStencilViewDesc.Format = DepthTextureDesc.Format;
				DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
				D3D11Device->CreateDepthStencilView(D3D11DepthTexture.Get(), &DepthStencilViewDesc, &D3D11DepthStencilView);
				D3D11DevContext->OMSetRenderTargets(1, D3D11RenderTargetView.GetAddressOf(), D3D11DepthStencilView.Get());
				D3D11_VIEWPORT viewport{};
				viewport.Width = desc.Width;
				viewport.Height = desc.Height;
				viewport.MaxDepth = 1.f;
				D3D11DevContext->RSSetViewports(1, &viewport);
				const std::array<D3D11_INPUT_ELEMENT_DESC, 2> InputLayout{ {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				} };
			}
		}
	public:
		void Begin_Draw(HWND hwnd){
			CreateResource(hwnd);
		}
		void End_Draw(){
			DXGI_PRESENT_PARAMETERS parameters {};
			DXGISwapChain->Present1(1, 0, &parameters);
		}
		void Draw_Clear(const COLORREF color,const float alpha){
			auto clr = Convert_RGBA(color,alpha);
			D3D11DevContext->ClearRenderTargetView(D3D11RenderTargetView.Get(),clr);
			D3D11DevContext->ClearDepthStencilView(D3D11DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
			const std::array<Vec3,3> vertex = { {
				{ { 0.5f, -0.5f, 0.f },{ 1.f, 1.f, 1.f, 1.f } },{ { -0.5f, 0.5f, 0.f },{ 1.f, 1.f, 1.f, 1.f } },{ { 0.5f, 0.5f, 0.f },{ 1.f, 1.f, 1.f, 1.f } }
				} };
		}
	};
}