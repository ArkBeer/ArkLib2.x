#pragma once
#include<wrl.h>
#include<atlbase.h>
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<directxmath.h>
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
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexbuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantbuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflect;
		struct ComInitializer {
			ComInitializer() { CoInitialize(nullptr); }
			~ComInitializer() { CoUninitialize(); }
		};
		ComInitializer scom;
		struct Vec3 {
			float x, y, z;
		};
		struct Color{
			float r,g,b,a;
		};
		struct Vertex {
			Vec3 vec;
			Color color;
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
		DXGI_FORMAT GetDxgiFormat(D3D10_REGISTER_COMPONENT_TYPE type, BYTE mask){
			if (mask & 0x0F){
            // xyzw
				switch (type){
				case D3D10_REGISTER_COMPONENT_FLOAT32:
					return DXGI_FORMAT_R32G32B32A32_FLOAT;
				}
			}

			if (mask & 0x07){
            // xyz
				switch (type){
				case D3D10_REGISTER_COMPONENT_FLOAT32:
					return DXGI_FORMAT_R32G32B32_FLOAT;
				}
			}

			if (mask & 0x3){
				// xy
				switch (type){
				case D3D10_REGISTER_COMPONENT_FLOAT32:
					return DXGI_FORMAT_R32G32_FLOAT;
				}
			}

			if (mask & 0x1){
				// x
				switch (type){
				case D3D10_REGISTER_COMPONENT_FLOAT32:
					return DXGI_FORMAT_R32_FLOAT;
				}
			}
	
			return DXGI_FORMAT_UNKNOWN;
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
				const std::array<D3D11_INPUT_ELEMENT_DESC, 2> layout{
					D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					D3D11_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};
				d3d11device->CreateInputLayout(layout.data(),layout.size(), vsblob->GetBufferPointer(), vsblob->GetBufferSize(), &inputlayout);
				d3d11context->IASetInputLayout(inputlayout.Get());

				psblob = CompileShaderFromFile(_T("PixelShader.hlsl"), "main", featurelevel >= D3D_FEATURE_LEVEL_11_0 ? "ps_5_0" : "ps_4_0");
				d3d11device->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), nullptr, &pixelshader);

				D3DReflect(vsblob->GetBufferPointer(),vsBlob->GetBufferSize(),IID_ID3D11ShaderReflection,&reflect);
				D3D11_SHADER_DESC shaderdesc{};
				reflect->GetDesc(&shaderdesc);
				std::vector<D3D11_INPUT_ELEMENT_DESC> eldesces;
				for(int i=0;i<shaderdesc.InputParamaters;++i){
					D3D11_SIGNATURE_PARAMETER_DESC sigdesc{};
					reflect->GetInputParameterDesc(i,&sigdesc);
					auto format = GetDxgiFormat(sigdesc.ComponentType,sigdesc.Mask);
					D3D11_INPUT_ELEMENT_DESC eldesc{
						sigdesc.SemanticName,
						sigdesc.SemanticIndex,
						format,
						0,
						D3D11_APPEND_ALIGNED_ELEMENT,
						D3D11_INPUT_PER_VERTEX_DATA,
						0
					};
					eldesces.push_back(eldesc);
				}
				if(!eldesces.empty()){
					d3d11device->CreateInputLayout(eldesces.data(),eldesces.size(), vsblob->GetBufferPointer(), vsblob->GetBufferSize(), &inputlayout);
				}
				
				std::array< Vertex, 3> vertices{
					Vertex{Vec3{ 0.0f, 0.5f, 0.5f },{1.0f,0.0f,0.0f,1.0f}},
					Vertex{Vec3{ 0.5f, -0.5f, 0.5f } ,{0.0f,1.0f,0.0f,1.0f}},
					Vertex{Vec3{ -0.5f, -0.5f, 0.5f },{0.0f,0.0f,1.0f,1.0f}},
				};
				D3D11_BUFFER_DESC desc{};
				desc.ByteWidth = sizeof(Vertex)*vertices.size();
				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				D3D11_SUBRESOURCE_DATA initdata{};
				initdata.pSysMem = vertices.data();
				d3d11device->CreateBuffer(&desc, &initdata, &vertexbuffer);
				
				std::array<UINT, 3> indexes{ 0,1,2 };
				D3D11_BUFFER_DESC idesc{};
				idesc.ByteWidth = indexes.size();
				idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				D3D11_SUBRESOURCE_DATA idata{};
				idata.pSysMem = indexes.data();
				auto hr=d3d11device->CreateBuffer(&idesc,&idata,&indexbuffer);
				hr;
				UINT s = sizeof(Vertex);
				UINT o = 0;
				d3d11context->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &s, &o);
				d3d11context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				d3d11context->IASetIndexBuffer(indexbuffer.Get(),DXGI_FORMAT_R16_UINT,0);

				d3d11context->PSSetShader(pixelshader.Get(), nullptr, 0);
				d3d11context->VSSetShader(vertexshader.Get(), nullptr, 0);

			}
		}
		void Render() {

			d3d11context->Draw(3, 0);
			swapchain->Present(1, 0);

		}
		void DrawClear() {
			const std::array<float, 4> color = { 0.0f,0.2f,0.4f,1.0f };
			d3d11context->ClearRenderTargetView(rendertarget.Get(), color.data());
		}
	};
}
