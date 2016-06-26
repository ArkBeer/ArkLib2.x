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
Ark::D3D11 d3;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Ark::WinClass wc(hInstance);
	wc.Set_Window_Title(_T("title"));
	wc.Set_Window_Elements(GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX|WS_THICKFRAME);
	wc.Set_Window_Size(300,300);
	wc.Boot();

	return 0;
}
int Ark::WinClass::Boot() {
	while (!End_Flag()) {
		auto s = std::chrono::steady_clock::now();
		d3.Begin_Draw(hWnd);
		d3.Draw_Clear(RGB(255,0,0),1.0f);
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s);
		d3.End_Draw();
	}
	return 0;
}
