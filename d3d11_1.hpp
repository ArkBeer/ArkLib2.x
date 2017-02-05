#pragma once
#include<wrl.h>
#include<atlbase.h>
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<fstream>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
namespace Ark {
	class D3D11_1 {
		Microsoft::WRL::ComPtr<ID3D11Device1> d3d11device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3d11context;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rendertarget;
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexbuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexshader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelshader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputlayout;

		struct ComInitializer {
			ComInitializer() { CoInitialize(nullptr); }
			~ComInitializer() { CoUninitialize(); }
		};
		ComInitializer scom;
	public:

		auto CompileShaderFromFile(LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel)
		{
			DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
			dwShaderFlags |= D3DCOMPILE_DEBUG;
			dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			Microsoft::WRL::ComPtr<ID3D10Blob> blob;
			Microsoft::WRL::ComPtr<ID3D10Blob> errorblob;
			D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,dwShaderFlags, 0, &blob, nullptr);
			return blob;
		}
		void CreateResource(HWND hwnd) {
			if (!d3d11context || !d3d11device) {
				RECT rect;
				GetClientRect(hwnd, &rect);
				Microsoft::WRL::ComPtr<ID3D11Device> device;
				Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
				D3D_FEATURE_LEVEL lv;
				D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_10_1;
				D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SINGLETHREADED,&fl , 1, D3D11_SDK_VERSION, &device, &lv, &context);
				if (lv == D3D_FEATURE_LEVEL_11_1)OutputDebugString(_T("11.1\n"));
				device.As(&d3d11device);
				context.As(&d3d11context);

				Microsoft::WRL::ComPtr<IDXGIDevice2> dxgidevice;
				d3d11device.As(&dxgidevice);

				Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
				dxgidevice->GetAdapter(&adapter);

				Microsoft::WRL::ComPtr<IDXGIFactory2> factory;
				adapter->GetParent(__uuidof(IDXGIFactory2), &factory);

				DXGI_SWAP_CHAIN_DESC1 sdesc{};
				sdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				sdesc.BufferCount = 2;
				sdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				sdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				sdesc.SampleDesc.Count = 1;
				factory->CreateSwapChainForHwnd(device.Get(), hwnd, &sdesc, nullptr, nullptr, &swapchain);

				Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer;
				swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);
				d3d11device->CreateRenderTargetView(backbuffer.Get(), nullptr, &rendertarget);
				Microsoft::WRL::ComPtr<ID3DBlob> vsblob;
				Microsoft::WRL::ComPtr<ID3DBlob> psblob;
				vsblob=CompileShaderFromFile(_T("VertexShader.hlsl"),"main","vs_4_0");
				psblob=CompileShaderFromFile(_T("PixelShader.hlsl"), "main", "ps_4_0");
				d3d11device->CreateVertexShader(vsblob->GetBufferPointer(),vsblob->GetBufferSize(),nullptr,&vertexshader);
				d3d11device->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), nullptr, &pixelshader);
				d3d11context->VSSetShader(vertexshader.Get(),nullptr,0);
				d3d11context->PSSetShader(pixelshader.Get(), nullptr, 0);
				const std::array<D3D11_INPUT_ELEMENT_DESC, 2> InputLayout{ 
					D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					D3D11_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }};
				const std::array<D3D11_INPUT_ELEMENT_DESC, 1> edesc{
					D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
				d3d11device->CreateInputLayout(edesc.data(),edesc.size(),vsblob->GetBufferPointer(),vsblob->GetBufferSize(),&inputlayout);
				d3d11context->IASetInputLayout(inputlayout.Get());
				D3D11_VIEWPORT vp{};
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				vp.Width = rect.right - rect.left;
				vp.Height = rect.bottom - rect.top;
				d3d11context->RSSetViewports(1,&vp);
			}
		}
		void DrawClear() {
			d3d11context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), nullptr);
			const std::array<float, 4> color = { 0.0f,0.2f,0.4f,1.0f };
			d3d11context->ClearRenderTargetView(rendertarget.Get(), color.data());
			
			struct Vec3 {
				double x, y, z;
			};
			std::array<Vec3, 3> vertices = { Vec3{ 0.0f, 0.5f, 0.0f },Vec3{ 0.45f, -0.5f, 0.0f },Vec3{ -0.45f, -0.5f, 0.0f } };
			D3D11_BUFFER_DESC desc{};
			desc.ByteWidth = sizeof(Vec3)*vertices.size();
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			D3D11_SUBRESOURCE_DATA data = { vertices.data(),0,0 };
			d3d11device->CreateBuffer(&desc,&data,&vertexbuffer);
			
			swapchain->Present(1, 0);
		}
	};
}
