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
Ark::D2D1_0 d0;
Ark::D2D1_1 d1;
Ark::D3D11 d3;
Ark::Audio::WavSource ws(_T("test.wav"),au);
int Ark::WinClass::Boot() {
	Set_Window_Title(_T("title"));
	Set_Window_Elements(GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME);
	Set_Window_Size(300, 300);
	ws.Play();
	while (!End_Flag()) {
		auto s = std::chrono::steady_clock::now();
		//d3.Begin_Draw(hWnd);
		d1.Begin_Draw(hWnd);
		d1.Draw_Clear(RGB(255, 0, 0));
		//d3.Draw_Clear(RGB(0,255,0),0.2f);
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s);
		d1.Draw_String(0, 0, tstr.str().c_str(), RGB(0, 0, 0), 1.0f, 20);
		d1.End_Draw();
		//d3.End_Draw();
	}
	return 0;
}
