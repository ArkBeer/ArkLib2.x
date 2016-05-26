#pragma once
#include<atlbase.h>
/*Include(WIC)*/
#include<wincodec.h>
#include<wincodecsdk.h>
#pragma comment(lib,"WindowsCodecs.lib")
/*Include(Direct2D)*/
#include<d2d1.h>
#include<d2d1helper.h>
#include<dwrite.h>
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
namespace Ark {
	class D2D1_0 :Draw_Class {
		CComQIPtr<IWICImagingFactory> IWICIFactory;
		CComQIPtr<ID2D1Factory> D2DFactory;
		CComQIPtr<ID2D1HwndRenderTarget> D2DTarget;
		CComQIPtr<ID2D1SolidColorBrush> D2DSCBrush;
		CComQIPtr<IDWriteFactory> D2DWFactory;
		CComQIPtr<IDWriteTextFormat> D2DWFormat;
		bool Create_D2D_Resource(HWND hwnd) {
			if (!D2DTarget) {
				RECT rect;
				GetClientRect(hwnd, &rect);
				HRESULT hr = D2DFactory->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(hwnd,
						D2D1::Size(static_cast<UINT>(rect.right - rect.left), static_cast<UINT>(rect.bottom - rect.top))),
					&D2DTarget);
				if (FAILED(hr))return false;
			}
			if (!D2DSCBrush) {
				HRESULT hr = D2DTarget->CreateSolidColorBrush(D2D1::ColorF(RGB(0, 0, 0), 1.0f), &D2DSCBrush);
				if (FAILED(hr))return false;
			}
			return true;
		}
		int Set_RGB(COLORREF& color) {
			int i = color % 0x100;
			color /= 0x100;
			return i;
		}
		void Convert_RGB(int& r, int& g, int& b, COLORREF& color) {
			r = Set_RGB(color);
			g = Set_RGB(color);
			b = Set_RGB(color);
		}
	public:
		D2D1_0() {
			CoInitialize(nullptr);
			D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DFactory);
			DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&D2DWFactory));
			CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void**>(&IWICIFactory));
		}
		~D2D1_0() {
			IWICIFactory.Release();
			CoUninitialize();
		}
		void Begin_Draw(HWND hwnd) {
			if (Create_D2D_Resource(hwnd))D2DTarget->BeginDraw();
		}
		void End_Draw() {
			if (D2DTarget->EndDraw() == D2DERR_RECREATE_TARGET) {
				D2DTarget.Release();
				D2DSCBrush.Release();
			}
		}
		void Draw_Clear(COLORREF color) {
			int r, g, b;
			Convert_RGB(r, g, b, color);
			D2DTarget->Clear(D2D1::ColorF(RGB(b, g, r)));
		}
		void Draw_String(const float x, const float y, LPCTSTR lp, COLORREF color, const float f, const float size) {
			int r, g, b;
			Convert_RGB(r, g, b, color);
			D2DWFactory->CreateTextFormat(_T("ƒƒCƒŠƒI"), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL, size, L"ja-jp", &D2DWFormat);
			D2DSCBrush->SetColor(D2D1::ColorF(RGB(b, g, r), f));
			D2DTarget->DrawText(lp, lstrlen(lp), D2DWFormat, D2D1::RectF(x, y, x + lstrlen(lp)*size, y + size), D2DSCBrush);
			D2DWFormat.Release();
		}
		void Draw_Line(const float x1, const float y1, const float x2, const float y2, COLORREF color, const float f, const float size) {
			int r, g, b;
			Convert_RGB(r, g, b, color);
			D2DSCBrush->SetColor(D2D1::ColorF(RGB(b, g, r), f));
			D2DTarget->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), D2DSCBrush, size);
		}
		void Draw_Rectangle(const float x1, const float y1, const float x2, const float y2, COLORREF color, const float f) {
			int r, g, b;
			Convert_RGB(r, g, b, color);
			D2DSCBrush->SetColor(D2D1::ColorF(RGB(b, g, r), f));
			D2DTarget->FillRectangle(D2D1::RectF(x1, y1, x2, y2), D2DSCBrush);
		}
		void Draw_Rectangle(const float x1, const float y1, const float x2, const float y2, COLORREF color, const float f, const float size) {
			int r, g, b;
			Convert_RGB(r, g, b, color);
			D2DSCBrush->SetColor(D2D1::ColorF(RGB(b, g, r), f));
			D2DTarget->DrawRectangle(D2D1::RectF(x1, y1, x2, y2), D2DSCBrush, size);
		}void Draw_Round_Rectangle(const float x1, const float y1, const float x2, const float y2, const float rx, const float ry, COLORREF color, const float f) {
			int r, g, b;
			Convert_RGB(r, g, b, color);
			D2DSCBrush->SetColor(D2D1::ColorF(RGB(b, g, r), f));
			D2DTarget->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x1, y1, x2, y2), rx, ry), D2DSCBrush);
		}
		void Draw_Round_Rectangle(const float x1, const float y1, const float x2, const float y2, const float rx, const float ry, COLORREF color, const float f, const float size) {
			int r, g, b;
			Convert_RGB(r, g, b, color);
			D2DSCBrush->SetColor(D2D1::ColorF(RGB(b, g, r), f));
			D2DTarget->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x1, y1, x2, y2), rx, ry), D2DSCBrush, size);
		}
		void Draw_Ellipse(const float x, const float y, const float rx, const float ry, COLORREF color, const float f) {
			int r, g, b;
			Convert_RGB(r, g, b, color);
			D2DSCBrush->SetColor(D2D1::ColorF(RGB(b, g, r), f));
			D2DTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), rx, ry), D2DSCBrush);
		}
		void Draw_Ellipse(const float x, const float y, const float rx, const float ry, COLORREF color, const float f, const float size) {
			int r, g, b;
			Convert_RGB(r, g, b, color);
			D2DSCBrush->SetColor(D2D1::ColorF(RGB(b, g, r), f));
			D2DTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), rx, ry), D2DSCBrush, size);
		}
		CComQIPtr<ID2D1Bitmap> Set_Bitmap(LPCTSTR image) {
			CComQIPtr<ID2D1Bitmap> bitmap;
			if (IWICIFactory && !bitmap) {
				CComQIPtr<IWICBitmapDecoder> dec;
				IWICIFactory->CreateDecoderFromFilename(image, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &dec);
				CComQIPtr<IWICBitmapFrameDecode> frame;
				dec->GetFrame(0, &frame);
				CComQIPtr<IWICFormatConverter> converter;
				IWICIFactory->CreateFormatConverter(&converter);
				converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
				if (D2DTarget)D2DTarget->CreateBitmapFromWicBitmap(converter, NULL, &bitmap);
			}
			return bitmap;
		}
		void Draw_Bitmap(CComQIPtr<ID2D1Bitmap>& bitmap, const float x1, const float y1, const float x2, const float y2, const float bx1, const float by1, const float bx2, const float by2, const float f) {
			//D2DTarget->DrawBitmap(bitmap, D2D1::Rect<float>(x1, y1, bitmap->GetSize().width, bitmap->GetSize().height));
			D2DTarget->DrawBitmap(bitmap, D2D1::RectF(x1, y1, x2, y2), f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(bx1, by1, bx2, by2));
		}
		void Transform(D2D1::Matrix3x2F& matrix) {
			D2DTarget->SetTransform(matrix);
		}
		D2D1::Matrix3x2F Rotation(const float angle, const float x, const float y) {
			return D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(x, y));
		}
		D2D1::Matrix3x2F Scale(const float scalex, const float scaley, const float x, const float y) {
			return D2D1::Matrix3x2F::Scale(D2D1::SizeF(scalex, scaley), D2D1::Point2F(x, y));
		}
		D2D1::Matrix3x2F Translation(const float x, const float y) {
			return D2D1::Matrix3x2F::Translation(D2D1::SizeF(x, y));
		}
		D2D1::Matrix3x2F Skew(const float anglex, const float angley, const float x, const float y) {
			return D2D1::Matrix3x2F::Skew(anglex, angley, D2D1::Point2F(x, y));
		}
	};
}