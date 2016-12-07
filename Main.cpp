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
const auto trans(const int x, const int y,const float xratio,const float yratio) {
	std::pair<int, int> pair;
	if (x/yratio==y) {
		pair.first = x;
		pair.second = y;
	}else if (x / yratio*xratio < y) {
		pair.first = x;
		pair.second = x / yratio*xratio;
	}
	else {
		pair.first = y / xratio * yratio;
		pair.second = y;
	}
	return pair;
}
void LockAspectRatio(const float xratio,const float yratio) {
	RECT rc;
	GetClientRect(Ark::WinClass::Get_hWnd(), &rc);
	auto pair = trans(rc.right,rc.bottom,xratio,yratio);
	if (!(pair.first == rc.right && pair.second == rc.bottom)) {
		Ark::WinClass::SetSize(pair.first,pair.second);
	}
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Ark::WinClass wc(hInstance);
	wc.AddStyle(WS_THICKFRAME).SetTitle(_T("Title")).SetSize(300,200);
	while (!wc.End_Flag()) {
		auto s = std::chrono::steady_clock::now();
		LockAspectRatio(10, 16);
		d1.Begin_Draw(wc.Get_hWnd());
		d1.Draw_Clear(RGB(0, 0, 0));
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s);
		d1.Draw_String(0, 0, tstr.str().c_str(), 0xffffff, 1.0f, 10);
		d1.End_Draw();
	}
	return 0;
}
