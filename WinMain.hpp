#pragma once
namespace Ark {
	class WinClass {
		static const auto Create_WndClassEx() {
			WNDCLASSEX wex;
			ZeroMemory(&wex, sizeof(wex));
			wex.cbSize = sizeof(WNDCLASSEX);
			wex.style = CS_HREDRAW | CS_VREDRAW;
			wex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
			wex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wex.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
			wex.lpszMenuName = nullptr;
			wex.lpszClassName = _T("ArkLib");
			return wex;
		}

		struct WinMain_Arguments {
			HINSTANCE hInstance;
			HINSTANCE hPrevInstance;
			LPSTR lpCmdLine;
			int nCmdShow;
		};
		WNDCLASSEX WndClass_Ex;
		WinMain_Arguments WinMain_Arg;
		static WinClass* ptr;
		static HWND hWnd;
		static bool endflag;

	public:
		WinClass(HINSTANCE hinst, HINSTANCE hpinst, LPSTR lp, int i) :WinClass(hinst, hpinst, lp, i, Create_WndClassEx()) {}
		WinClass(HINSTANCE hinst, HINSTANCE hpinst, LPSTR lp, int i, WNDCLASSEX wex)  {
			ptr = this;
			endflag = false;
			WinMain_Arg.hInstance = hinst;
			WinMain_Arg.hPrevInstance = hpinst;
			WinMain_Arg.lpCmdLine = lp;
			WinMain_Arg.nCmdShow = i;
			WndClass_Ex = wex;
			WndClass_Ex.hInstance = WinMain_Arg.hInstance;
			WndClass_Ex.lpfnWndProc = CallProc;
			RegisterClassEx(&WndClass_Ex);
			hWnd = CreateWindowEx(0, WndClass_Ex.lpszClassName, _T("ArkLib"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
				CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr, nullptr, WinMain_Arg.hInstance, static_cast<LPVOID>(this));

			ShowWindow(hWnd, WinMain_Arg.nCmdShow);
			UpdateWindow(hWnd);
			SetWindowLong(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(this));
		}
		static LRESULT CALLBACK CallProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			auto ptr_ = reinterpret_cast<WinClass*>(GetWindowLong(WinClass::ptr->hWnd, GWLP_USERDATA));
			return ptr_ ? WinClass::ptr->MainProc(hwnd, msg, wParam, lParam) : DefWindowProc(hwnd, msg, wParam, lParam);
		}
		LRESULT MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			PAINTSTRUCT ps;
			switch (msg) {
			case WM_PAINT:
				BeginPaint(hwnd, &ps);
				EndPaint(hwnd, &ps);
				return 0;
			case WM_ERASEBKGND:
			{
				return FALSE;
			}
			case WM_DESTROY:
				PostQuitMessage(0);
				endflag = true;
				return 0;
			default:
				return DefWindowProc(hwnd, msg, wParam, lParam);
			}
		}
		bool Set_Window_Elements(int index, LONG lp) {
			if (SetWindowLongPtr(hWnd, index, lp))return true;
			return false;
		}
		bool Set_WndClass_Elements(int index, LONG lp) {
			if (SetClassLongPtr(hWnd, index, lp))return true;
			return false;
		}
		bool Set_Window_Pos(HWND z, int x, int y, int cx, int cy, UINT uf = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW) {
			if (SetWindowPos(hWnd, z, x, y, cx, cy, uf))return true;
			return false;
		}
		void Set_Window_Title(LPCTSTR lp) {
			SetWindowText(hWnd, lp);
		}
		bool Set_Window_Position(int x, int y) {
			if (SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW))return true;
			return false;
		}
		bool Set_Window_Size(int hx, int hy) {
			RECT rc;
			if (!SetWindowPos(hWnd, nullptr, 0, 0, hx, hy, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW))return false;
			GetClientRect(hWnd, &rc);
			const int fx = hx - (rc.right - rc.left);
			const int fy = hy - (rc.bottom - rc.top);
			if (SetWindowPos(hWnd, nullptr, 0, 0, hx + fx, hy + fy, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW))return true;
			return false;
		}
		bool Set_Transparent(const COLORREF color, const int alpha, const DWORD dw) {
			BLENDFUNCTION bf{};
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.SourceConstantAlpha = 105;
			RECT rc;
			GetClientRect(hWnd, &rc);
			SIZE size = { 800,600 };
			UPDATELAYEREDWINDOWINFO info{};
			info.cbSize = sizeof(UPDATELAYEREDWINDOWINFO);
			info.psize = &size;
			info.crKey = RGB(0, 0, 0);
			info.pblend = &bf;
			info.dwFlags = ULW_ALPHA;
			info.hdcSrc = GetDC(hWnd);
			UpdateLayeredWindowIndirect(hWnd, &info);
			return 0;

		}
		static const bool End_Flag(){
			MSG msg;
			if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
				if (msg.message != WM_QUIT) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else Exit();
			}
			return endflag; 
		}
		static const bool Exit() { endflag = true; return endflag; }
		static const auto Get_hWnd() {
			return hWnd;
		}
		int Boot();
	};
	WinClass* WinClass::ptr = nullptr;
	HWND WinClass::hWnd = nullptr;
	bool WinClass::endflag = false;
}