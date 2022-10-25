// MyClass.cpp : implementation file
//

#include "stdafx.h"
//#include "nxrmRMC.h"
#include "BtnClass.h"
#include "gdiplus.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Gdiplus;

/////////////////////////////////////////////////////////////////////////////
// CBtnClass

//CBtnClass::CBtnClass(int nIdNormal, int nIdFocusDisable, int nIdDepressed, CString szLabel)
//{
//}
//
//CBtnClass::CBtnClass()
//{
//}

IMPLEMENT_DYNAMIC(CBtnClass, CButton)

BEGIN_MESSAGE_MAP(CBtnClass, CButton)
    //ON_WM_SETCURSOR()
END_MESSAGE_MAP()

CBtnClass::~CBtnClass()
{
}

void CBtnClass::SetImagetText(int activeID, int deactiveID, ImageVarType drawType, COLORREF rgbHighlight, CString text)
{
    m_BtnImgNormalID = activeID;
    //m_BtnImgDisabledlID = disableID;
    m_BtnImgPressedID = deactiveID;
    m_imgFlag = drawType;
    m_label = text;
    m_rgbBkGrnd = rgbHighlight;
   
}

void CBtnClass::SetColors(COLORREF rgbBkGrnd, COLORREF rgbFrGrnd, BOOL bBorder, COLORREF rgbDisabledBkGrnd, COLORREF rgbDisabledFrGrnd)
{
	if (!bBorder)
		m_imgFlag = NX_USE_COLOR_AND_TEXT;
	else
		m_imgFlag = NX_USE_COLOR_AND_TEXT_BORDER;

    m_rgbBkGrnd = rgbBkGrnd;
    m_rgbFrGrnd = rgbFrGrnd;
    m_rgbDisabledBkGrnd = rgbDisabledBkGrnd;
    m_rgbDisabledFrGrnd = rgbDisabledFrGrnd;
}


void CBtnClass::SetImages(int normalID, int disableID, int depressedID, COLORREF rgbBk )
{
    m_BtnImgNormalID = normalID;
    m_BtnImgDisabledlID = disableID;
    m_BtnImgPressedID = depressedID;
    m_imgFlag = NX_USE_IMAGE_ID;
    m_rgbBkGrnd = rgbBk;
}

void CBtnClass::SetCheckBtnImages(int uncheckedID, int checkedID, COLORREF rgbBk)
{
    m_BtnImgNormalID = uncheckedID;
    m_BtnImgPressedID = checkedID;
    m_imgFlag = NX_USE_IMAGE_AS_CHKBTN;
    m_rgbBkGrnd = rgbBk;

}

void CBtnClass::SetImagesHandles(HBITMAP hNormalID, HBITMAP hDisableID, HBITMAP hDepressedID)
{
    m_hBtnImgNormal = hNormalID;
    m_hBtnImgDisabled = hDisableID;
    m_hBtnImgPressed = hDepressedID;
    m_imgFlag = NX_USE_BMP_HANDLE;
}


void CBtnClass::SetLabel(CString szLabel)
{
    m_label = szLabel;
}


//BEGIN_MESSAGE_MAP(CBtnClass, CButton)
//	//{{AFX_MSG_MAP(CBtnClass)
//		// NOTE - the ClassWizard will add and remove mapping macros here.
//	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBtnClass message handlers

void CBtnClass::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    HDC hDc = lpDrawItemStruct->hDC;
    CRect rcBtn = lpDrawItemStruct->rcItem;

    switch (lpDrawItemStruct->itemAction)
    {
    case ODA_DRAWENTIRE:
    {

        switch (m_imgFlag)
        {
        case NX_USE_IMAGE_ID:
        {
            HBRUSH nxhBrush = (HBRUSH)CreateSolidBrush(m_rgbBkGrnd);
            FillRect(hDc, &rcBtn, nxhBrush);
            CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
            bool bBmpLoaded = false;

			if (lpDrawItemStruct->itemState &  ODS_DISABLED)
			{
				bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgDisabledlID), L"PNG", AfxGetInstanceHandle());
			}
			else
			{
				bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgNormalID), L"PNG", AfxGetInstanceHandle());
			}

            if (bBmpLoaded)
            {
                Gdiplus::Graphics graphics(hDc);
                Rect rect(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height());
                graphics.SetSmoothingMode(SmoothingModeHighQuality);
                //int iconHeight = rcBtn.Width();
                //int iconWidth = iconHeight;
                Rect rc(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height());
                //rc.Inflate(Point(-2, -2));
                graphics.DrawImage(*pBitmap, (const Rect&)rc);
            }
            delete(pBitmap);
            DeleteObject(nxhBrush);
        }
        break;
        case NX_USE_IMAGE_AS_CHKBTN:
        {
            HBRUSH nxhBrush = (HBRUSH)CreateSolidBrush(m_rgbBkGrnd);
            FillRect(hDc, &rcBtn, nxhBrush);
            CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
            bool bBmpLoaded = false;

            if (GetBtnCheck() == BST_CHECKED)
            {
                bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgPressedID), L"PNG", AfxGetInstanceHandle());

            }
            else
            {
                bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgNormalID), L"PNG", AfxGetInstanceHandle());
            }

            if (bBmpLoaded)
            {
                Gdiplus::Graphics graphics(hDc);
                Rect rect(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height());
                graphics.SetSmoothingMode(SmoothingModeHighQuality);
                int iconHeight = rcBtn.Width();
                int iconWidth = iconHeight;
                Rect rc(rcBtn.left, rcBtn.top, iconWidth, iconWidth);
                //rc.Inflate(Point(-2, -2));
                graphics.DrawImage(*pBitmap, (const Rect&)rc);
            }
            delete(pBitmap);
            DeleteObject(nxhBrush);
        }
        break;
        case NX_USE_BMP_HANDLE:
        { //Here we get the bitmap handle
            HBRUSH nxhBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
            FillRect(hDc, &rcBtn, nxhBrush);

            CRect r;
            CBitmap bitmap;
            CDC dc, *pDC;
            CDC drawDC;
            drawDC.Attach(lpDrawItemStruct->hDC);
            pDC = &drawDC;
            BITMAP bmp;
            bitmap.Attach(m_hBtnImgNormal);
            bitmap.GetBitmap(&bmp);
            dc.CreateCompatibleDC(pDC);
            dc.SelectObject(bitmap);
            pDC->BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &dc, 0, 0, SRCCOPY);
            //bitmap.DeleteObject();
            bitmap.Detach();
            pDC->Detach();
        }
        break;
        case NX_USE_BMP_TEXT_VERTICAL:
        {
            CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;

            bool bBmpLoaded = false;
            CWnd wnd;
            CString wndText;
            GetWindowText(wndText);
            if (GetBtnCheck() == BST_CHECKED)
            {
                //HBRUSH nxhBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
                CBrush br;
                br.CreateSolidBrush(RGB(240,240,240));
                FillRect(hDc, &rcBtn, (HBRUSH)br.GetSafeHandle());
                bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgNormalID), L"PNG", AfxGetInstanceHandle());
            }
            else
            {
                HBRUSH nxhBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
                FillRect(hDc, &rcBtn, nxhBrush);
                bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgPressedID), L"PNG", AfxGetInstanceHandle());
            }


            if (bBmpLoaded)
            {
                Gdiplus::Graphics graphics(hDc);
                Rect rect(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height());
                graphics.SetSmoothingMode(SmoothingModeHighQuality);
                int iconHeight = rcBtn.Width();
                int iconWidth = iconHeight;
                Rect rc(rcBtn.left, rcBtn.top, iconWidth, iconWidth);
                int inflateScale = -rcBtn.Width() / 4;
                rc.Inflate(Point(inflateScale, inflateScale));
                rc.Offset(0, -rc.GetTop() + iconHeight * 5/32 );

                graphics.DrawImage(*pBitmap, rc);
                if (GetBtnCheck() == BST_CHECKED)
                {
                    CBrush br;
                    rcBtn.right = rcBtn.left + rcBtn.Width() / 20;
                    br.CreateSolidBrush(m_rgbBkGrnd);
                    FillRect(hDc, rcBtn, (HBRUSH)br.GetSafeHandle());
                }
                CString caption;
                GetWindowText(caption);

                Gdiplus::Font font(&FontFamily(L"Arial"), 10);
                LinearGradientBrush brush(Rect(0, 0, 100, 100), Color::Black, Color::Black, LinearGradientModeHorizontal);
                RectF rcf((Gdiplus::REAL)rcBtn.left, (Gdiplus::REAL)(rcBtn.top + rcBtn.Height() * 9 /16), 
                    (Gdiplus::REAL)iconWidth, (Gdiplus::REAL)rcBtn.Height() * 1 / 3);
                Gdiplus::StringFormat strFmt(0, 0);
                strFmt.SetAlignment(StringAlignmentCenter);
                Status st = graphics.DrawString(caption, -1, &font, rcf, &strFmt, &brush);

            }
            delete(pBitmap);
        }
        break;
		case NX_USE_BMP_TEXT_HORIZONTAL:
		{
			CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;

			bool bBmpLoaded = false;
			CWnd wnd;
			CString wndText;
			GetWindowText(wndText);
			if (GetBtnCheck() == BST_CHECKED || GetBtnCheck() == BST_INDETERMINATE)
			{
				//HBRUSH nxhBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
                CBrush br;
				if (GetBtnCheck() == BST_INDETERMINATE) {
					br.CreateSolidBrush(RGB(255, 255, 255));
				}
				else {
					br.CreateSolidBrush(RGB(240, 240, 240));
				}
				//FillRect(hDc, &rcBtn, (HBRUSH)br.GetSafeHandle());
				bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgNormalID), L"PNG", AfxGetInstanceHandle());
				HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
                auto hOldBr = SelectObject(hDc, br.GetSafeHandle());
				auto hOld = SelectObject(hDc, hPen);
                //auto hOldBr = SelectObject(hDc, hBackBrush);
				Rectangle(hDc, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
				SelectObject(hDc, hOld);
                SelectObject(hDc, hOldBr);
				DeleteObject(hPen);
			}
			else
			{
                CBrush br;
                br.CreateSolidBrush(RGB(255, 255, 255));
                FillRect(hDc, &rcBtn, (HBRUSH)br.GetSafeHandle());
                bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgPressedID), L"PNG", AfxGetInstanceHandle());
			}


			if (bBmpLoaded)
			{
				Gdiplus::Graphics graphics(hDc);
				Rect rect(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height());
				graphics.SetSmoothingMode(SmoothingModeHighQuality);
				int iconHeight = rcBtn.Height();
				int iconWidth = iconHeight;
				
				Rect rc(rcBtn.left, rcBtn.top, iconWidth, iconWidth);
				rc.Inflate(-iconHeight/4, -iconHeight / 4);
				rc.Offset(Point(iconHeight / 8, 0));

				graphics.DrawImage(*pBitmap, rc);
				CString caption;
				GetWindowText(caption);

				
				Color clrFrom;
				Color clrTo;
				CString fontName = L"Arial";
				Gdiplus::FontStyle fsStyle;
				//int fontSize;
				if (GetBtnCheck() == BST_CHECKED)
				{
					clrFrom.SetFromCOLORREF(m_rgbBkGrnd);
					clrTo.SetFromCOLORREF(m_rgbBkGrnd);
					fsStyle = Gdiplus::FontStyleBold;
					
					
				} 
				else
				{
					clrFrom = Color::Black;
					clrTo = Color::Black;
					fsStyle = Gdiplus::FontStyleRegular;
					
				}

				if (lpDrawItemStruct->itemState & ODS_DISABLED) {
					clrFrom = Color::Gray;
					clrTo = Color::Gray;
				}

				Gdiplus::Font font(&FontFamily(fontName), 10, fsStyle);

				LinearGradientBrush brush(Rect(0, 0, 100, 100), clrFrom, clrTo, LinearGradientModeHorizontal);
				RectF rcf((Gdiplus::REAL)rcBtn.left + iconHeight, (Gdiplus::REAL)(rcBtn.top),
					(Gdiplus::REAL)rcBtn.Width() - rcBtn.Height(), (Gdiplus::REAL)rcBtn.Height());
				Gdiplus::StringFormat strFmt(0, 0);
				strFmt.SetAlignment(StringAlignmentNear);
				strFmt.SetLineAlignment(StringAlignmentCenter);
				Status st = graphics.DrawString(caption, -1, &font, rcf, &strFmt, &brush);

			}
			delete(pBitmap);
		}
		break;
        case NX_USE_BMP_TEXT_HORIZONTAL_CIRCLE_TEXT:
        {
            CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;

            bool bBmpLoaded = false;
            CWnd wnd;
            CString wndText;
            GetWindowText(wndText);
            if (GetBtnCheck() == BST_CHECKED)
            {
                //HBRUSH nxhBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
                CBrush br;
                br.CreateSolidBrush(RGB(240, 240, 240));
                //FillRect(hDc, &rcBtn, (HBRUSH)br.GetSafeHandle());
                bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgNormalID), L"PNG", AfxGetInstanceHandle());
                HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
                auto hOldBr = SelectObject(hDc, br.GetSafeHandle());
                auto hOld = SelectObject(hDc, hPen);
                //auto hOldBr = SelectObject(hDc, hBackBrush);
                Rectangle(hDc, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
                SelectObject(hDc, hOld);
                SelectObject(hDc, hOldBr);
                DeleteObject(hPen);
            }
            else
            {
                CBrush br;
                br.CreateSolidBrush(RGB(255, 255, 255));
                FillRect(hDc, &rcBtn, (HBRUSH)br.GetSafeHandle());
                bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(m_BtnImgPressedID), L"PNG", AfxGetInstanceHandle());
            }


            if (bBmpLoaded)
            {
                Gdiplus::Graphics graphics(hDc);
                Rect rect(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height());
                graphics.SetSmoothingMode(SmoothingModeHighQuality);
                int iconHeight = rcBtn.Height();
                int iconWidth = iconHeight;

                Rect rc(rcBtn.left, rcBtn.top, iconWidth, iconWidth);
                rc.Inflate(-iconHeight / 4, -iconHeight / 4);
                rc.Offset(Point(iconHeight / 8, 0));

                graphics.DrawImage(*pBitmap, rc);
                CString caption;
                GetWindowText(caption);


                Color clrFrom;
                Color clrTo;
                Color clrCircleFrom;
                Color clrCircleTo;
                CString fontName = L"Arial";
                Gdiplus::FontStyle fsStyle;
                CRect rcText2 = { rcBtn.Width() - 3 * rcBtn.Height() / 2, rcBtn.top, rcBtn.right - rcBtn.Height() / 2, rcBtn.bottom };
                if (GetBtnCheck() == BST_CHECKED)
                {
                    clrFrom.SetFromCOLORREF(m_rgbBkGrnd);
                    clrTo.SetFromCOLORREF(m_rgbBkGrnd);
                    fsStyle = Gdiplus::FontStyleBold;

                    Gdiplus::Color gdipColor(255, 0, 0, 0);
                    gdipColor.SetFromCOLORREF(RGB(57, 151, 74));
                    Gdiplus::SolidBrush blackBrush(gdipColor);
                    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                    rcText2.DeflateRect(rcText2.Width() / 4, rcText2.Width() / 4);
                    graphics.FillEllipse(&blackBrush, rcText2.left, rcText2.top, rcText2.Width(), rcText2.Height());

                    clrCircleFrom = Color::White;
                    clrCircleTo = Color::White;
                    fsStyle = Gdiplus::FontStyleRegular;
                }
                else
                {
                    clrCircleFrom = Color::Black;
                    clrCircleTo = Color::Black;
                    clrFrom = Color::Black;
                    clrTo = Color::Black;
                    fsStyle = Gdiplus::FontStyleRegular;
                }
                Gdiplus::Font font(&FontFamily(fontName), 10, fsStyle);
                LinearGradientBrush brushCircle(Rect(0, 0, 100, 100), clrCircleFrom, clrCircleTo, LinearGradientModeHorizontal);

                RectF rcfCircle((REAL)rcText2.left, (REAL)rcText2.top, (REAL)rcText2.Width(), (REAL)rcText2.Height());
                Gdiplus::StringFormat strFmtCircle(0, 0);
                strFmtCircle.SetAlignment(StringAlignmentCenter);
                strFmtCircle.SetLineAlignment(StringAlignmentCenter);
                Status stCircle = graphics.DrawString(m_circleText, -1, &font, rcfCircle, &strFmtCircle, &brushCircle);

                if (lpDrawItemStruct->itemState & ODS_DISABLED) {
                    clrFrom = Color::Gray;
                    clrTo = Color::Gray;
                }
                LinearGradientBrush brush(Rect(0, 0, 100, 100), clrFrom, clrTo, LinearGradientModeHorizontal);

                RectF rcf((Gdiplus::REAL)rcBtn.left + iconHeight, (Gdiplus::REAL)(rcBtn.top),
                    (Gdiplus::REAL)rcBtn.Width() - rcBtn.Height(), (Gdiplus::REAL)rcBtn.Height());
                Gdiplus::StringFormat strFmt(0, 0);
                strFmt.SetAlignment(StringAlignmentNear);
                strFmt.SetLineAlignment(StringAlignmentCenter);
                Status st = graphics.DrawString(caption, -1, &font, rcf, &strFmt, &brush);

            }
            delete(pBitmap);
        }
        break;
        case NX_USE_COLOR_AND_TEXT_BORDER:
			if (lpDrawItemStruct->itemState & ODS_DISABLED)
			{
				HBRUSH nxhBrush = CreateSolidBrush(m_rgbDisabledBkGrnd);
				auto hOldBrush = SelectObject(hDc, nxhBrush);
				HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
				auto hOld = SelectObject(hDc, hPen);
				RoundRect(hDc, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom, 5, 5);
				SelectObject(hDc, hOld);
				SelectObject(hDc, hOldBrush);
				CString szBtnText;
				GetWindowText(szBtnText);
				SetBkMode(hDc, TRANSPARENT);
				::SetTextColor(hDc, m_rgbDisabledFrGrnd);

				DWORD dwHorizStyle = DT_CENTER;
				if (m_textAlign == BS_LEFT)
				{
					dwHorizStyle = DT_LEFT;
				}
				else if (m_textAlign == BS_RIGHT)
				{
					dwHorizStyle = DT_RIGHT;
				}

				DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, dwHorizStyle | DT_VCENTER | DT_SINGLELINE);
				DeleteObject(nxhBrush);
			}
			else
			{
				HBRUSH nxhBrush = CreateSolidBrush(m_rgbBkGrnd);
				FillRect(hDc, &rcBtn, nxhBrush);
				HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
				auto hOld = SelectObject(hDc, hPen);
				RoundRect(hDc, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom, 5, 5);
				SelectObject(hDc, hOld);
				CString szBtnText;
				GetWindowText(szBtnText);
				SetBkMode(hDc, TRANSPARENT);
				::SetTextColor(hDc, m_rgbFrGrnd);

				DWORD dwHorizStyle = DT_CENTER;
				if (m_textAlign == BS_LEFT)
				{
					dwHorizStyle = DT_LEFT;
				}
				else if (m_textAlign == BS_RIGHT)
				{
					dwHorizStyle = DT_RIGHT;
				}
				DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, dwHorizStyle | DT_VCENTER | DT_SINGLELINE);
				DeleteObject(nxhBrush);
				DeleteObject(hPen);
			}
			break;
        case NX_USE_COLOR_AND_TEXT:
            if (lpDrawItemStruct->itemState & ODS_DISABLED)
            {
                HBRUSH nxhBrush = CreateSolidBrush(m_rgbDisabledBkGrnd);
                FillRect(hDc, &rcBtn, nxhBrush);
                CString szBtnText;
                GetWindowText(szBtnText);
                SetBkMode(hDc, TRANSPARENT);
                ::SetTextColor(hDc, m_rgbDisabledFrGrnd);
                
                DWORD dwHorizStyle = DT_CENTER;
                if (m_textAlign == BS_LEFT)
                {
                    dwHorizStyle = DT_LEFT;
                }
                else if (m_textAlign == BS_RIGHT)
                {
                    dwHorizStyle = DT_RIGHT;
                }
                
                DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, dwHorizStyle | DT_VCENTER | DT_SINGLELINE);
                DeleteObject(nxhBrush);
            }
            else
            {
                HBRUSH nxhBrush = CreateSolidBrush(m_rgbBkGrnd);
                FillRect(hDc, &rcBtn, nxhBrush);
				CString szBtnText;
                GetWindowText(szBtnText);
                SetBkMode(hDc, TRANSPARENT);
                ::SetTextColor(hDc, m_rgbFrGrnd);

                DWORD dwHorizStyle = DT_CENTER;
                if (m_textAlign == BS_LEFT)
                {
                    dwHorizStyle = DT_LEFT;
                }
                else if (m_textAlign == BS_RIGHT)
                {
                    dwHorizStyle = DT_RIGHT;
                }
                DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, dwHorizStyle | DT_VCENTER | DT_SINGLELINE);
				DeleteObject(nxhBrush);
            }
            break;
        }
    }
    break;
    case ODA_FOCUS:
    {
		switch (m_imgFlag) {
		case NX_USE_COLOR_AND_TEXT_BORDER:
			{
 				HBRUSH nxhBrush = CreateSolidBrush(m_rgbBkGrnd);
				auto hOldBrush = SelectObject(hDc, nxhBrush);
				HPEN hPen = CreatePen(PS_SOLID, 1, RGB(60, 100, 255));
				auto hOld = SelectObject(hDc, hPen);
				RoundRect(hDc, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom, 5, 5);
				SelectObject(hDc, hOld);
				SelectObject(hDc, hOldBrush);
				CString szBtnText;
				SetBkMode(hDc, TRANSPARENT);
				GetWindowText(szBtnText);
				::SetTextColor(hDc, m_rgbFrGrnd);

				DWORD dwHorizStyle = DT_CENTER;
				if (m_textAlign == BS_LEFT)
				{
					dwHorizStyle = DT_LEFT;
				}
				else if (m_textAlign == BS_RIGHT)
				{
					dwHorizStyle = DT_RIGHT;
				}
				DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, dwHorizStyle | DT_VCENTER | DT_SINGLELINE);
				DeleteObject(nxhBrush);
				DeleteObject(hPen);
			}
			break;
		}
        if (lpDrawItemStruct->itemState & ODS_FOCUS)
        {

		}
        else
        {
            //TRACE(L"No Focus\n");
        }

    }
    break;


    }

	//dc.Detach();
}

BOOL CBtnClass::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    //SetCursor(LoadCursor(NULL,IDC_HAND));
    return TRUE;

}
void CBtnClass::SetCircleNumber(int circleNumber)
{
    CString circleText;
    circleText.Format(L"%d", circleNumber);
    SetCircleText(circleText);
    InvalidateRect(NULL);
}
