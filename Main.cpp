#include"Header.hpp"
#include<iostream>
#include<cmath>
#include<cstdlib>
#include<utility>
#include<algorithm>
#include<vector>
#include<memory>
const double PI = 3.141592;
Ark::Key key;
Ark::Mouse ms;
Ark::Random rnd;
Ark::Frame_Rate fps;
Ark::Audio au;
Ark::D2D1_1 d1;
Ark::D2D1_0 d0;
Ark::D3D11 d3;

struct Wave {
	struct wavFileHeader {
		struct fmtChunk {
			std::uint32_t id;
			std::uint32_t bytes;
			std::uint16_t formatID;
			std::uint16_t channels;
			std::uint32_t rate;
			std::uint32_t velocity;
			std::uint16_t blocksize;
			std::uint16_t bits;
		};
		struct dataChunk {
			std::uint32_t id;
			std::uint32_t bytes;
		};
		std::uint32_t riffID;
		std::uint32_t size_8;
		std::uint32_t wavID;
		fmtChunk fmtchunk;
		dataChunk datachunk;
	};
	wavFileHeader head;
	std::ifstream ifs;
	WAVEFORMATEX wavex;
	std::unique_ptr<std::uint8_t[]> data;
	XAUDIO2_BUFFER buf;
	Wave(const LPCTSTR lp) :head{}, wavex{}, buf{} {
		ifs.open(lp,std::ios::binary | std::ios::in);
		ifs.read(reinterpret_cast<char*>(&head), sizeof(head));
		wavex.cbSize = 0;
		wavex.wFormatTag = WAVE_FORMAT_PCM;
		wavex.nSamplesPerSec = head.fmtchunk.rate;
		wavex.wBitsPerSample = head.fmtchunk.bits;
		wavex.nChannels = head.fmtchunk.channels;
		wavex.nBlockAlign = head.fmtchunk.blocksize;
		wavex.nAvgBytesPerSec = head.fmtchunk.velocity;
		data = std::make_unique<std::uint8_t[]>(head.datachunk.bytes);
		ifs.read(reinterpret_cast<char*>(data.get()), head.datachunk.bytes);
		buf.AudioBytes = head.datachunk.bytes;
		buf.pAudioData = &data[0];
		buf.Flags = XAUDIO2_END_OF_STREAM;
	}
};
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Wave w(_T("test.wav"));
	if(FAILED(au.XAudio->CreateSourceVoice(&au.SVoice,&w.wavex)))OutputDebugString(_T("failed source create\n"));
	Ark::WinClass wc(hInstance);
	wc.Set_Window_Title(_T("title"));
	wc.Set_Window_Elements(GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX|WS_THICKFRAME);
	wc.Set_Window_Size(300,300);
	au.SVoice->SubmitSourceBuffer(&w.buf);
	au.SVoice->Start();
	wc.Boot();
	au.SVoice->Stop();

	return 0;
}
int Ark::WinClass::Boot() {
	while (!End_Flag()) {
		auto s = std::chrono::steady_clock::now();
		d3.Begin_Draw(hWnd);
		//d1.Begin_Draw(hWnd);
		//d1.Draw_Clear(RGB(255, 0, 0));
		d3.Draw_Clear(RGB(0,255,0),0.2f);
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s);
		//d1.Draw_String(0, 0, tstr.str().c_str(), RGB(0, 0, 0), 1.0f, 20);
		//d1.End_Draw();
		d3.End_Draw();
	}
	return 0;
}
