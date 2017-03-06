#pragma once
#include<wrl.h>
#include<atlbase.h>
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<fstream>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#include<array>
namespace Ark {
	class D3D11_1 {
		Microsoft::WRL::ComPtr<ID3D11Device1> d3d11device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3d11context;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rendertarget;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexshader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelshader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputlayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexbuffer;
		struct ComInitializer {
			ComInitializer() { CoInitialize(nullptr); }
			~ComInitializer() { CoUninitialize(); }
		};
		ComInitializer scom;
		struct Vec3 {
			float x, y, z;
		};
		D3D_DRIVER_TYPE drivertype;
		D3D_FEATURE_LEVEL featurelevel;
		auto CompileShaderFromFile(LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel)
		{
			DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
			dwShaderFlags |= D3DCOMPILE_DEBUG;
			dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			Microsoft::WRL::ComPtr<ID3D10Blob> blob;
			D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, &blob, nullptr);
			return blob;
		}
	public:
		void InitDevice(HWND hwnd) {
			if (!d3d11context || !d3d11device) {
				RECT rect;
				GetClientRect(hwnd, &rect);
				Microsoft::WRL::ComPtr<ID3D11Device> device;
				Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
				std::array<D3D_DRIVER_TYPE, 3> drivertypearr{ 
					D3D_DRIVER_TYPE_HARDWARE,D3D_DRIVER_TYPE_WARP,D3D_DRIVER_TYPE_REFERENCE 
				};
				std::array<D3D_FEATURE_LEVEL, 4> featurelevelarr{
					D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0,D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0 
				};
				UINT flag = D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
				flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
				for (auto& v : drivertypearr) {
					drivertype = v;

					auto hr = D3D11CreateDevice(nullptr, drivertype, nullptr, flag, featurelevelarr.data(), featurelevelarr.size(), D3D11_SDK_VERSION, &device, &featurelevel, &context);
					if (hr == E_INVALIDARG) {
						hr = D3D11CreateDevice(nullptr, drivertype, nullptr, flag, &featurelevelarr.at(1), featurelevelarr.size() - 1, D3D11_SDK_VERSION, &device, &featurelevel, &context);
					}
					if (SUCCEEDED(hr))break;
				}
				device.As(&d3d11device);
				context.As(&d3d11context);
				Microsoft::WRL::ComPtr<IDXGIDevice2> dxgidevice;
				d3d11device.As(&dxgidevice);

				Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
				dxgidevice->GetAdapter(&adapter);
				Microsoft::WRL::ComPtr<IDXGIFactory2> factory;
				adapter->GetParent(__uuidof(IDXGIFactory2), &factory);

				device.As(&d3d11device);
				context.As(&d3d11context);

				DXGI_SWAP_CHAIN_DESC1 sdesc{};
				sdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				sdesc.BufferCount = 2;
				sdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				sdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				sdesc.SampleDesc.Count = 1;
				factory->CreateSwapChainForHwnd(device.Get(), hwnd, &sdesc, nullptr, nullptr, &swapchain);

				Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer;
				swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);

				d3d11device->CreateRenderTargetView(backbuffer.Get(), nullptr, &rendertarget);

				d3d11context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), nullptr);

				D3D11_VIEWPORT vp{};
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				vp.Width = rect.right - rect.left;
				vp.Height = rect.bottom - rect.top;
				d3d11context->RSSetViewports(1, &vp);
				Microsoft::WRL::ComPtr<ID3DBlob> vsblob;
				Microsoft::WRL::ComPtr<ID3DBlob> psblob;
				vsblob = CompileShaderFromFile(_T("VertexShader.hlsl"), "main", featurelevel >= D3D_FEATURE_LEVEL_11_0 ? "vs_5_0" : "vs_4_0");
				d3d11device->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), nullptr, &vertexshader);
				const std::array<D3D11_INPUT_ELEMENT_DESC, 1> layout{
					D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};
				d3d11device->CreateInputLayout(layout.data(),layout.size(), vsblob->GetBufferPointer(), vsblob->GetBufferSize(), &inputlayout);
				d3d11context->IASetInputLayout(inputlayout.Get());

				psblob = CompileShaderFromFile(_T("PixelShader.hlsl"), "main", featurelevel >= D3D_FEATURE_LEVEL_11_0 ? "ps_5_0" : "ps_4_0");
				d3d11device->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), nullptr, &pixelshader);

				std::array< Vec3, 3> vertices =
				{
					Vec3{ 0.0f, 0.5f, 0.5f },
					Vec3{ 0.5f, -0.5f, 0.5f },
					Vec3{ -0.5f, -0.5f, 0.5f },
				};

				D3D11_BUFFER_DESC desc{};
				desc.ByteWidth = sizeof(Vec3)*vertices.size();
				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				D3D11_SUBRESOURCE_DATA initdata{};
				initdata.pSysMem = vertices.data();
				auto hr=d3d11device->CreateBuffer(&desc, &initdata, &vertexbuffer);
				UINT s = sizeof(Vec3);
				UINT o = 0;
				d3d11context->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &s, &o);
				d3d11context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				const std::array<float, 4> color = { 0.0f,0.2f,0.4f,1.0f };
				d3d11context->ClearRenderTargetView(rendertarget.Get(), color.data());
				d3d11context->PSSetShader(pixelshader.Get(), nullptr, 0);
				d3d11context->VSSetShader(vertexshader.Get(), nullptr, 0);

			}
		}
		void Render() {

			d3d11context->Draw(3, 0);
			swapchain->Present(1, 0);

		}
	};
}