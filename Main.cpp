#include"Header.hpp"
#include"D3D11_1.hpp"
Ark::FrameRate fps;
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	Ark::WinClass wc(hInstance);
	wc.AddStyle(WS_THICKFRAME).SetTitle(_T("Title")).SetSize(300,200);
	Ark::D3D11_1 d;
	while (!wc.EndFlag()) {
		auto s = std::chrono::steady_clock::now();
		wc.LockAspectRatio(10, 16);
		d.BeginDraw(wc.GethWnd());
		d.DrawClear();
		d.Draw();
		d.EndDraw();
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s);
	}
	return 0;
}
