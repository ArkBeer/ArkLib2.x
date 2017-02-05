#include"Header.hpp"
#include"d3d11_1.hpp"
Ark::FrameRate fps;
Ark::D3D11_1 d11;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Ark::WinClass wc(hInstance);
	wc.AddStyle(WS_THICKFRAME).SetTitle(_T("Title")).SetSize(300,200);
	while (!wc.EndFlag()) {
		auto s = std::chrono::steady_clock::now();
		d11.CreateResource(wc.GethWnd());
		wc.LockAspectRatio(10, 16);
		d11.DrawClear();
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s);
	}
	return 0;
}
