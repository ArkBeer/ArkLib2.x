#include"Header.hpp"
#include"D3D11_1.hpp"
#include"WIC.hpp"
#include<array>
#include<vector>

Ark::FrameRate fps;
Ark::Random rnd;
Ark::Key key;
bool flag = true;
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	Ark::WinClass wc(hInstance);
	wc.AddStyle(WS_THICKFRAME).SetTitle(_T("Title")).SetSize(300,200);
	Ark::D3D11_1 d;
	Ark::WIC w;
	auto image=w.loadimage(_T("icon.png"));
	Ark::D3D11_1::Texture tex;
	while (!wc.EndFlag()) {
		auto s = std::chrono::steady_clock::now();
		wc.LockAspectRatio(9, 16);
		d.BeginDraw(wc.GethWnd());
		tex=d.createtexture(tex,image);
		d.DrawClear();
		d.Draw(tex);
		d.EndDraw();
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s)<<"\n";
		//OutputDebugString(tstr.str().c_str());
	}
	return 0;
}
