#include"Header.hpp"
#include<iostream>
#include<cmath>
#include<cstdlib>
#include<utility>
#include<algorithm>
#include<array>
#include<vector>
const double PI = 3.141592;
Ark::Key key;
Ark::Mouse ms;
Ark::Random rnd;
Ark::Frame_Rate fps;
Ark::D2D1_0 d0;
Ark::D2D1_1 d1;
int i;
std::tuple<int, double, char> tpl{int(),double(),char()};
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Ark::WinClass<int> wc(hInstance,hPrevInstance,lpCmdLine,nCmdShow,i);
	wc.Boot();
	return 0;
}
int Ark::WinClass<int>::Main() {
	auto s = std::chrono::steady_clock::now();
	d1.Begin_Draw(hWnd);
	d1.Draw_Clear(RGB(255,0,0));
	fps.Count();
	Ark::TstringStream tstr;
	tstr << static_cast<double>(1 / fps.Duration<double>(s));
	d1.End_Draw();
	return 0;
}