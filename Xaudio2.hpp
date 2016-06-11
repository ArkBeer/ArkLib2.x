#pragma once
#include<xaudio2.h>
#pragma comment(lib,"Xaudio2.lib")
#include<fstream>
namespace Ark {
	struct Audio {
		template<typename T>
		class Com {
			T* ptr;
		public:
			Com() {
			}
			~Com() {
				if (ptr != nullptr) {
					ptr->DestroyVoice();
					ptr = nullptr;
				}
			}
			T** operator&() { return &ptr; }
			T** operator&()const { return &ptr; }
			T* operator->() { return ptr; }
			T* operator->()const { return ptr; }
			T* Get()const { return ptr; }
		};
		Microsoft::WRL::ComPtr<IXAudio2> XAudio;
		Com<IXAudio2MasteringVoice> MVoice;
		Com<IXAudio2SourceVoice> SVoice;
		Audio(){
			CoInitialize(nullptr);
			if(FAILED(XAudio2Create(&XAudio)))OutputDebugString(_T("faild xaudio crate\n"));
			if (FAILED(XAudio->CreateMasteringVoice(&MVoice)))OutputDebugString(_T("failed mastering create\n"));
		}
		~Audio(){
			CoUninitialize();
		}
	};
}