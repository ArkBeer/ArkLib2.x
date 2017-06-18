#include"Header.hpp"
#include"D3D11_1.hpp"
#include"WIC.hpp"
#include<array>
#include<vector>
#include"Matrix.hpp"
Ark::FrameRate fps;
Ark::Random rnd;
Ark::Key key;
bool flag = true;
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	Ark::WinClass wc(hInstance);
	wc.AddStyle(WS_THICKFRAME).SetTitle(_T("kuso")).SetSize(200,200);
	Ark::D3D11_1 d;
	Ark::WIC w;
	Ark::Mouse m;
	auto image=w.loadimage(_T("texture.png"));

	Ark::D3D11_1::Texture tex;
	while (!wc.EndFlag()) {
		auto s = std::chrono::steady_clock::now();
		wc.LockAspectRatio(1, 1);
		d.BeginDraw(wc.GethWnd());
		d.SetTexture(tex,image);
		d.DrawClear();
		if(key.KeyCheck('W',true))d.SetView(d.GetView()*DirectX::XMMatrixTranslation(0,0,-0.1f));
		if (key.KeyCheck('S', true))d.SetView(d.GetView()*DirectX::XMMatrixTranslation(0, 0, 0.1f));
		if (key.KeyCheck('D', true))d.SetView(d.GetView()*DirectX::XMMatrixTranslation(-0.1f, 0, 0));
		if (key.KeyCheck('A', true))d.SetView(d.GetView()*DirectX::XMMatrixTranslation(0.1f, 0, 0));
		if (key.KeyCheck(VK_LBUTTON,true)) {
			Ark::Mouse m2;
			m2 = m2.GetClientPosition(wc.GethWnd());
			if (!(m.x == m2.x && m.y == m2.y)) {
				m2.x = m2.x - m.x;
				m2.y = m2.y - m.y;
				d.SetView(Ark::Matrix(d.GetView()).RotationY(-1.0f*m2.x/wc.GetSize().right*3.141592).RotationX(-1.0f*m2.y/wc.GetSize().bottom*3.141592).GetMatrix());
				m = m.GetClientPosition(wc.GethWnd());
			}
		}else m = m.GetClientPosition(wc.GethWnd());

		if (key.KeyCheck('1',false)) {
			DirectX::XMVECTOR eye{ 0.0f,2.0f,0.0f,0.0f };
			DirectX::XMVECTOR at{ 0.0f,0.0f,0.0f,0.0f };
			DirectX::XMVECTOR up{ 0.0f,0.0f,1.0f,0.0f };
			auto View = DirectX::XMMatrixLookAtLH(eye, at, up);
			d.SetView(View);
		}
		if (key.KeyCheck('2', false)) {
			DirectX::XMVECTOR eye{ 0.0f,0.0f,-2.0f,0.0f };
			DirectX::XMVECTOR at{ 0.0f,0.0f,0.0f,0.0f };
			DirectX::XMVECTOR up{ 0.0f,1.0f,0.0f,0.0f };
			auto View = DirectX::XMMatrixLookAtLH(eye, at, up);
			d.SetView(View);
		}
		if (key.KeyCheck('3', false)) {
			DirectX::XMVECTOR eye{ 0.0f,-2.0f,0.0f,0.0f };
			DirectX::XMVECTOR at{ 0.0f,0.0f,0.0f,0.0f };
			DirectX::XMVECTOR up{ 0.0f,0.0f,1.0f,0.0f };
			auto View = DirectX::XMMatrixLookAtLH(eye, at, up);
			d.SetView(View);
		}
		if (key.KeyCheck('4', false)) {
			DirectX::XMVECTOR eye{ 0.0f,0.0f,2.0f,0.0f };
			DirectX::XMVECTOR at{ 0.0f,0.0f,0.0f,0.0f };
			DirectX::XMVECTOR up{ 0.0f,1.0f,0.0f,0.0f };
			auto View = DirectX::XMMatrixLookAtLH(eye, at, up);
			d.SetView(View);
		}
		if (key.KeyCheck('5', false)) {
			DirectX::XMVECTOR eye{ -2.0f,0.0f,0.0f,0.0f };
			DirectX::XMVECTOR at{ 0.0f,0.0f,0.0f,0.0f };
			DirectX::XMVECTOR up{ 0.0f,1.0f,0.0f,0.0f };
			auto View = DirectX::XMMatrixLookAtLH(eye, at, up);
			d.SetView(View);
		}
		if (key.KeyCheck('6', false)) {
			DirectX::XMVECTOR eye{ 2.0f,0.0f,0.0f,0.0f };
			DirectX::XMVECTOR at{ 0.0f,0.0f,0.0f,0.0f };
			DirectX::XMVECTOR up{ 0.0f,1.0f,0.0f,0.0f };
			auto View = DirectX::XMMatrixLookAtLH(eye, at, up);
			d.SetView(View);
		}
		static float angle=0.0f;
		d.DrawCube(Ark::Matrix().Identity().GetMatrix(),tex);
		d.DrawCube(Ark::Matrix().Identity().Scaling(0.3,0.3,0.3).RotationZ(angle).Translation(-4,0,0).RotationY(angle*2).RotationX(angle*3).GetMatrix(), tex);
		angle += 0.01f;
		
		d.EndDraw();
		fps.Count();
		Ark::TstringStream tstr;
		tstr << fps.perDuration<double>(s)<<"\n";
		OutputDebugString(tstr.str().c_str());
	}
	return 0;
}
