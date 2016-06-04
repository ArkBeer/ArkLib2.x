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
		Microsoft::WRL::ComPtr<IXAudio2> IAudio;
		Com<IXAudio2MasteringVoice> MVoice;
		Com<IXAudio2SourceVoice> SVoice;
		Audio(){
		IAudio.Get();
			CoInitialize(nullptr);
			XAudio2Create(&IAudio);
			IAudio->CreateMasteringVoice(&MVoice);
		}
		~Audio(){
			CoUninitialize();
		}
	};
}