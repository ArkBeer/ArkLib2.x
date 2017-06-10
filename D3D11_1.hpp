#pragma once
#include<wrl.h>
#include<atlbase.h>
#include<d3d11_1.h>
#include<d3dcompiler.h>
#include<directxmath.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#include<vector>
#include<array>

#include"WIC.hpp"

namespace Ark {
	class D3D11_1 {
		struct ComInitializer {
			ComInitializer() { CoInitialize(nullptr); }
			~ComInitializer() { CoUninitialize(); }
		};
		struct Vec4 {
			float x, y, z, w;
		};
		struct Color {
			float r, g, b, a;
		};
		struct Tex {
			float x, y;
		};
		struct Vertex {
			Vec4 vec;
			Color color;
			Tex tex;
		};
		ComInitializer com;
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
		D3D_DRIVER_TYPE drivertype;
		D3D_FEATURE_LEVEL featurelevel;
		DirectX::XMFLOAT4X4 buff;

		const auto CompileShaderFromFile(LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel)
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
		const auto GetDxgiFormat(const D3D_REGISTER_COMPONENT_TYPE type, const BYTE byte) {
			if (type == D3D_REGISTER_COMPONENT_FLOAT32) {
				switch (byte) {
				case 0x0f:
					return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case 0x07:
					return DXGI_FORMAT_R32G32B32_FLOAT;
				case 0x03:
					return DXGI_FORMAT_R32G32_FLOAT;
				case 0x01:
					return DXGI_FORMAT_R32_FLOAT;
				default:
					return DXGI_FORMAT_UNKNOWN;
				}
			}
			return DXGI_FORMAT_UNKNOWN;
		}

	public:
		struct Texture {
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resourceview;
			Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
		};
		const bool InitDevice(HWND hwnd) {
			if (!d3d11context || !d3d11device) {
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

				device.As(&d3d11device);
				context.As(&d3d11context);
				Microsoft::WRL::ComPtr<IDXGIDevice2> dxgidevice;
				d3d11device.As(&dxgidevice);

				Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
				dxgidevice->GetAdapter(&adapter);
				Microsoft::WRL::ComPtr<IDXGIFactory2> factory;
				adapter->GetParent(__uuidof(IDXGIFactory2), &factory);


				DXGI_SWAP_CHAIN_DESC1 scdesc{};
				scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				scdesc.BufferCount = 2;
				scdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				scdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				scdesc.SampleDesc.Count = 1;
				factory->CreateSwapChainForHwnd(device.Get(), hwnd, &scdesc, nullptr, nullptr, &swapchain);

				Microsoft::WRL::ComPtr<ID3D11Texture2D> backbuffer;
				swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);

				d3d11device->CreateRenderTargetView(backbuffer.Get(), nullptr, &rendertarget);
				d3d11context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), nullptr);

				RECT rect;
				GetClientRect(hwnd, &rect);
				D3D11_VIEWPORT vp{};
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				vp.Width = rect.right - rect.left;
				vp.Height = rect.bottom - rect.top;
				d3d11context->RSSetViewports(1, &vp);

				Microsoft::WRL::ComPtr<ID3DBlob> vsblob;
				Microsoft::WRL::ComPtr<ID3DBlob> psblob;
				vsblob = CompileShaderFromFile(_T("Shader.fx"), "vsmain", featurelevel >= D3D_FEATURE_LEVEL_11_0 ? "vs_5_0" : "vs_4_0");
				psblob = CompileShaderFromFile(_T("Shader.fx"), "psmain", featurelevel >= D3D_FEATURE_LEVEL_11_0 ? "ps_5_0" : "ps_4_0");
				d3d11device->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), nullptr, &vertexshader);
				d3d11device->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), nullptr, &pixelshader);
				d3d11context->PSSetShader(pixelshader.Get(), nullptr, 0);
				d3d11context->VSSetShader(vertexshader.Get(), nullptr, 0);

				Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflect;
				D3DReflect(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), IID_ID3D11ShaderReflection, &reflect);

				D3D11_SHADER_DESC saddesc{};
				reflect->GetDesc(&saddesc);
				std::vector<D3D11_INPUT_ELEMENT_DESC> element;
				for (int i = 0; i<saddesc.InputParameters; ++i) {
					D3D11_SIGNATURE_PARAMETER_DESC sigdesc{};
					reflect->GetInputParameterDesc(i, &sigdesc);
					const auto format = GetDxgiFormat(sigdesc.ComponentType, sigdesc.Mask);
					D3D11_INPUT_ELEMENT_DESC eldesc = {
						sigdesc.SemanticName,
						sigdesc.SemanticIndex,
						format,
						0,
						D3D11_APPEND_ALIGNED_ELEMENT,
						D3D11_INPUT_PER_VERTEX_DATA,
						0
					};
					element.push_back(eldesc);
				}
				//element.at(0).Format = DXGI_FORMAT_R32G32B32_FLOAT;
				if (!element.empty()) {
					d3d11device->CreateInputLayout(element.data(), element.size(), vsblob->GetBufferPointer(), vsblob->GetBufferSize(), &inputlayout);
					d3d11context->IASetInputLayout(inputlayout.Get());
				}

				D3D11_BUFFER_DESC cbdesc{};
				cbdesc.ByteWidth = sizeof(DirectX::XMMATRIX);
				cbdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				d3d11device->CreateBuffer(&cbdesc, nullptr, &constantbuffer);

				return true;
			}
			else return true;
		}
		auto& createtexture(Texture& tex,Ark::WIC::Image& i) {
			if (d3d11device && !tex.texture) {
				D3D11_TEXTURE2D_DESC desc{};
				desc.Width = i.getwidth();
				desc.Height = i.getheight();
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

				D3D11_SUBRESOURCE_DATA initdata{};
				initdata.pSysMem = i.getdata();
				initdata.SysMemPitch = i.stride();
				initdata.SysMemSlicePitch = i.size();
				auto hr=d3d11device->CreateTexture2D(&desc, &initdata, &tex.texture);
				D3D11_SHADER_RESOURCE_VIEW_DESC rdesc{};
				rdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				rdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				rdesc.Texture2D.MipLevels = 1;
				hr=d3d11device->CreateShaderResourceView(tex.texture.Get(), &rdesc, &tex.resourceview);

				D3D11_SAMPLER_DESC sdesc{};
				sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				sdesc.MaxAnisotropy = 1;
				sdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				sdesc.MaxLOD = D3D11_FLOAT32_MAX;
				hr=d3d11device->CreateSamplerState(&sdesc, &tex.sampler);
				//d3d11context->PSSetSamplers(0, 1, tex.sampler.GetAddressOf());
			}
			return tex;
		}
		void BeginDraw(HWND hwnd) {
			InitDevice(hwnd);
		}
		void EndDraw() {
			d3d11context->DrawIndexed(4, 0, 0);
			swapchain->Present(1, 0);
		}
		void DrawClear() {
			const std::array<float, 4> color = { 0.0f,0.2f,0.4f,1.0f };
			d3d11context->ClearRenderTargetView(rendertarget.Get(), color.data());
		}
		void Draw(Texture& tex) {
			std::array< Vertex, 4> vertices{
				Vertex{ Vec4{ -0.5f, -0.5f, 0.5f, 1.0f },{ 1.0f,0.0f,0.0f,1.0f },{0.0f,1.0f} },
				Vertex{ Vec4{ -0.5f, 0.5f, 0.5f, 1.0f },{ 0.0f,1.0f,0.0f,1.0f },{ 0.0f,0.0f } },
				Vertex{ Vec4{ 0.5f, -0.5f, 0.5f, 1.0f },{ 0.0f,0.0f,1.0f,1.0f } ,{ 1.0f,1.0f } },
				Vertex{ Vec4{ 0.5f, 0.5f, 0.5f, 1.0f },{ 0.0f,0.0f,1.0f,1.0f } ,{ 1.0f,0.0f } }
			};

			D3D11_BUFFER_DESC vdesc{};
			vdesc.ByteWidth = sizeof(vertices);
			vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vdata{};
			vdata.pSysMem = vertices.data();
			d3d11device->CreateBuffer(&vdesc, &vdata, &vertexbuffer);
			const UINT s = sizeof(Vertex);
			const UINT o = 0;
			d3d11context->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &s, &o);

			std::array<UINT, 4> indexes{ 0,1,2,3 };
			D3D11_BUFFER_DESC idesc{};
			idesc.ByteWidth = sizeof(indexes);
			idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA idata{};
			idata.pSysMem = indexes.data();
			d3d11device->CreateBuffer(&idesc, &idata, &indexbuffer);
			d3d11context->IASetIndexBuffer(indexbuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			d3d11context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			
			static float angleRadians = 0;
			const auto DELTA = DirectX::XMConvertToRadians(0.2f);
			angleRadians += DELTA;
			auto m = DirectX::XMMatrixRotationZ(angleRadians);
			DirectX::XMStoreFloat4x4(&buff, m);
			d3d11context->UpdateSubresource(constantbuffer.Get(), 0, nullptr, &buff, 0, 0);
			d3d11context->VSSetConstantBuffers(0, 1, constantbuffer.GetAddressOf());
			d3d11context->PSSetShaderResources(0,1,tex.resourceview.GetAddressOf());
		}
	};
}