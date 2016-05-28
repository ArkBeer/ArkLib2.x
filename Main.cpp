#include"Header.hpp"
#include<iostream>
#include<cmath>
#include<cstdlib>
#include<utility>
#include<algorithm>
#include<vector>
const double PI = 3.141592;
Ark::Key key;
Ark::Mouse ms;
Ark::Random rnd;
Ark::Frame_Rate fps;
Ark::D2D1_1 d1;
Ark::D2D1_0 d0;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Ark::TstringStream tstr;
	Ark::WinClass wc(hInstance);
	wc.Boot();
	return 0;
}

int Ark::WinClass::Boot() {
	while (!End_Flag()) {
		auto s = std::chrono::steady_clock::now();
		d1.Begin_Draw(hWnd);
		d1.Draw_Clear(RGB(255, 0, 0));
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s);
		d1.Draw_String(0, 0, tstr.str().c_str(), RGB(0, 0, 0), 1.0f, 20);
		d1.End_Draw();
	}
	return 0;
}
