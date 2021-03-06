#pragma once
namespace Ark {
	struct Mouse {
		int x, y;
		Mouse() :Mouse(0, 0) {}
		Mouse(int x_, int y_) :x(x_), y(y_) {}
		Mouse& operator=(const Mouse& m) {
			x = m.x;
			y = m.y;
			return *this;
		}
		static const Mouse GetScreenPosition() {
			POINT pt;
			GetCursorPos(&pt);
			return Mouse(pt.x, pt.y);
		}
		static const Mouse GetClientPosition(HWND hWnd) {
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hWnd, &pt);

			return Mouse(pt.x, pt.y);
		}

	};
}
