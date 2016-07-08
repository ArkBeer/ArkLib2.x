#pragma once
#include<wrl.h>
#include<atlbase.h>
#include<d3d11_1.h>
#include<fstream>

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
		std::vector<char> vs;
		std::vector<char> ps;
		void CreateVertexShader(const LPCTSTR lp) {
			std::ifstream ifs;
			ifs.open(lp, std::ios::binary | std::ios::in);
			while (!ifs.eof()) {
				char c;
				ifs.read(&c, sizeof(char));
				vs.push_back(c);
			}
		}
		void CreatePixelShader(const LPCTSTR lp) {
			std::ifstream ifs;
			ifs.open(lp, std::ios::binary | std::ios::in);
			while (!ifs.eof()) {
				char c;
				ifs.read(&c, sizeof(char));
				ps.push_back(c);
			}
		}
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
#ifdef _DEBUG
				const auto flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SINGLETHREADED;
#else
				const auto flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif
				D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, &Device, nullptr, &DevContext);
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

				D3D11Device->CreateInputLayout(InputLayout.data(),InputLayout.size(),vs.data(),sizeof(vs),&D3D11InputLayout);
				
				CreateVertexShader(_T("vshader.cso"));
				CreatePixelShader(_T("pshader.cso"));
				D3D11Device->CreateVertexShader(vs.data(),sizeof(vs),nullptr,&D3D11VertexShader);
				D3D11Device->CreatePixelShader(ps.data(),sizeof(ps),nullptr,&D3D11PixelShader);
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
			D3D11DevContext->IASetInputLayout(D3D11InputLayout.Get());
			D3D11DevContext->VSSetShader(D3D11VertexShader.Get(),nullptr,0);
			D3D11DevContext->PSSetShader(D3D11PixelShader.Get(),nullptr,0);
			
			const std::array<Vec3,3> vertex = { {
				{ { 0.5f, -0.5f, 0.f },{ 1.f, 1.f, 1.f, 1.f } },{ { -0.5f, 0.5f, 0.f },{ 1.f, 1.f, 1.f, 1.f } },{ { 0.5f, 0.5f, 0.f },{ 1.f, 1.f, 1.f, 1.f } }
				} };
			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = sizeof(vertex);
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.StructureByteStride = sizeof(float);
			D3D11_SUBRESOURCE_DATA sub = {};
			sub.pSysMem = vertex.data();
			UINT offsets = 0;
			UINT strides = sizeof(Vec3);
			Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
			D3D11Device->CreateBuffer(&desc,&sub,&buffer);
			D3D11DevContext->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), &strides, &offsets);
			D3D11DevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//D3D11DevContext->Draw(3, 0);
		}
	};
}