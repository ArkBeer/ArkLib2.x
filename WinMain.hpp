#pragma once
namespace Ark {
	template<typename T>
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
		HWND hWnd;
		bool EndFlag;
		T& Draw_Struct;

		template<typename U>
		void Begin_Draw_(...) {}
		template<typename U>
		void Begin_Draw_(typename U::type) { Draw_Struct.Begin_Draw(hWnd); }

		template<typename U>
		void End_Draw_(...) {}
		template<typename U>
		void End_Draw_(typename U::type) { Draw_Struct.End_Draw(); }

		template<typename U>
		void Draw_Clear_(...) {}
		template<typename U>
		void Draw_Clear_(COLORREF color, typename U::type) { Draw_Struct.Draw_Clear(color); }

		template<typename U>
		void Draw_String_(...) {}
		template<typename U>
		void Draw_String_(const float x, const float y, LPCTSTR lp, COLORREF color, const float f, const float size, typename U::type) { Draw_Struct.Draw_String(x, y, lp, color, f, size); }

		template<typename U>
		void Draw_Line_(...) {}
		template<typename U>
		void Draw_Line_(const float x1, const float y1, const float x2, const float y2, COLORREF color, const float f, const float size, typename U::type) { Draw_Struct.Draw_Line(x1, y1, x2, y2, color, f, size); }

		template<typename U>
		void Draw_Rectangle_(...) {}
		template<typename U>
		void Draw_Rectangle_(const float x1, const float y1, const float x2, const float y2, COLORREF color, const float f, typename U::type) { Draw_Struct.Draw_Rectangle(x1, y1, x2, y2, color, f); }
		template<typename U>
		void Draw_Rectangle_(const float x1, const float y1, const float x2, const float y2, COLORREF color, const float f, const float size, typename U::type) { Draw_Struct.Draw_Rectangle(x1, y1, x2, y2, color, f, size); }

		template<typename U>
		void Draw_Round_Rectangle_(...) {}
		template<typename U>
		void Draw_Round_Rectangle_(const float x1, const float y1, const float x2, const float y2, const float rx, const float ry, COLORREF color, const float f, typename U::type) { Draw_Struct.Draw_Round_Rectangle(x1, y1, x2, y2, rx, ry, color, f); }
		template<typename U>
		void Draw_Round_Rectangle_(const float x1, const float y1, const float x2, const float y2, const float rx, const float ry, COLORREF color, const float f, const float size, typename U::type) { Draw_Struct.Draw_Round_Rectangle(x1, y1, x2, y2, rx, ry, color, f, size); }

		template<typename U>
		void Draw_Ellipse_(...) {}
		template<typename U>
		void Draw_Ellipse_(const float x1, const float y1, const float rx, const float ry, COLORREF color, const float f, typename U::type) { Draw_Struct.Draw_Ellipse(x1, y1, rx, ry, color, f); }
		template<typename U>
		void Draw_Ellipse_(const float x1, const float y1, const float rx, const float ry, COLORREF color, const float f, const float size, typename U::type) { Draw_Struct.Draw_Ellipse(x1, y1, rx, ry, color, f, size); }

		template<typename U>
		auto Set_Bitmap_(...) { return nullptr; }
		template<typename U>
		auto Set_Bitmap_(LPCTSTR image, typename U::type) { return Draw_Struct.Set_Bitmap(image); }

		template<typename U>
		void Draw_Bitmap_(...) {}
		template<typename U>
		void Draw_Bitmap_(typename U::Bitmap_type& bitmap, const float x1, const float y1, const float x2, const float y2, const float bx1, const float by1, const float bx2, const float by2, const float f) { Draw_Struct.Draw_Bitmap(bitmap, x1, y1, x2, y2, bx1, by1, bx2, by2, f); }

		template<typename U>
		void Draw_Shape_(...) {}
		template<typename U>
		void Draw_Shape_(const U& sp, COLORREF color, const double f, const double size) { Draw_Struct.Draw_Shape(sp, color, f, size); }

	public:
		WinClass(HINSTANCE hinst, HINSTANCE hpinst, LPSTR lp, int i, T& t) :WinClass(hinst, hpinst, lp, i, Create_WndClassEx(), t) {}
		WinClass(HINSTANCE hinst, HINSTANCE hpinst, LPSTR lp, int i, WNDCLASSEX wex, T& t) :Draw_Struct(t) {
			ptr = this;
			EndFlag = false;
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
				EndFlag = true;
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
			//SIZE size={rc.right-rc.left,rc.bottom-rc.top};
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
		void Boot() {
			MSG msg;
			while (!EndFlag) {
				if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
					if (msg.message != WM_QUIT) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					else break;
				}
				Main();
			}
		}
		int Main() {
			return 0;
		}
		void Begin_Draw() { Begin_Draw_<T>(nullptr); }

		void End_Draw() { End_Draw_<T>(nullptr); }

		void Draw_Clear(COLORREF color) { Draw_Clear_<T>(color, nullptr); }

		void Draw_String(const float x, const float y, LPCTSTR lp, COLORREF color, const float f, const float size) { Draw_String_<T>(x, y, lp, color, f, size, nullptr); }

		void Draw_Line(const float x1, const float y1, const float x2, const float y2, COLORREF color, const float f, const float size) { Draw_Line_<T>(x1, y1, x2, y2, color, f, size, nullptr); }

		void Draw_Rectangle(const float x1, const float y1, const float x2, const float y2, COLORREF color, const float f) { Draw_Rectangle_<T>(x1, y1, x2, y2, color, f, nullptr); }
		void Draw_Rectangle(const float x1, const float y1, const float x2, const float y2, COLORREF color, const float f, const float size) { Draw_Rectangle_<T>(x1, y1, x2, y2, color, f, size, nullptr); }

		void Draw_Round_Rectangle(const float x1, const float y1, const float x2, const float y2, const float rx, const float ry, COLORREF color, const float f) { Draw_Round_Rectangle_<T>(x1, y1, x2, y2, rx, ry, color, f, nullptr); }
		void Draw_Round_Rectangle(const float x1, const float y1, const float x2, const float y2, const float rx, const float ry, COLORREF color, const float f, const float size) { Draw_Round_Rectangle_<T>(x1, y1, x2, y2, rx, ry, color, f, size, nullptr); }

		void Draw_Ellipse(const float x1, const float y1, const float rx, const float ry, COLORREF color, const float f) { Draw_Ellipse_<T>(x1, y1, rx, ry, color, f, nullptr); }
		void Draw_Ellipse(const float x1, const float y1, const float rx, const float ry, COLORREF color, const float f, const float size) { Draw_Ellipse_<T>(x1, y1, rx, ry, color, f, size, nullptr); }

		auto Set_Bitmap(LPCTSTR image) { return Set_Bitmap_<T>(image, nullptr); }

		template<typename U>
		void Draw_Bitmap(typename U& bitmap, const float x1, const float y1, const float x2, const float y2, const float bx1, const float by1, const float bx2, const float by2, const float f) { Draw_Bitmap_<T>(bitmap, x1, y1, x2, y2, bx1, by1, bx2, by2, f); }

		template<typename U>
		void Draw_Shape(const U& sp, COLORREF color, const double f, const double size) { Draw_Shape_<U>(sp, color, f, size); }

	};
	template<typename T>
	WinClass<T>* WinClass<T>::ptr = nullptr;
}