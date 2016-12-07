#include"Header.hpp"
Ark::FrameRate fps;
Ark::D2D1_1 d1;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Ark::WinClass wc(hInstance);
	wc.AddStyle(WS_THICKFRAME).SetTitle(_T("Title")).SetSize(300,200);
	while (!wc.EndFlag()) {
		auto s = std::chrono::steady_clock::now();
		wc.LockAspectRatio(10, 16);
		d1.BeginDraw(wc.GethWnd());
		d1.DrawClear(RGB(0, 0, 0));
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s);
		d1.DrawString(0, 0, tstr.str().c_str(), 0xffffff, 1.0f, 10);
		d1.EndDraw();
	}
	return 0;
}
