// NxButton.cpp : implementation file
//

#include "stdafx.h"
#include "NxButton.h"
#include "nxrmRMC.h"
//#include "gdiplus.h"

//using namespace Gdiplus;
// CNxButton

IMPLEMENT_DYNAMIC(CNxButton, CMFCButton)

CNxButton::CNxButton():m_textAlign(BS_CENTER),m_fontSize(8)
{

}

CNxButton::~CNxButton()
{
}


BEGIN_MESSAGE_MAP(CNxButton, CMFCButton)
END_MESSAGE_MAP()



// CNxButton message handlers




void CNxButton::OnDraw(CDC* pDC, const CRect& rect, UINT uiState)
{
    // TODO: Add your specialized code here and/or call the base class
    //HBRUSH nxhBrush = CreateSolidBrush(m_rgbBkGrnd);
    //FillRect(hDc, &rcBtn, nxhBrush);
    //CString szBtnText;
   // GetWindowText(szBtnText);
 /*   SetBkMode(hDc, TRANSPARENT);
    ::SetTextColor(hDc, m_rgbFrGrnd);

    DWORD dwHorizStyle = DT_CENTER;
    if (m_textAlign == BS_LEFT)
    {
        dwHorizStyle = DT_LEFT;
    }*/
//    pDC->DrawText(szBtnText, &rc, DT_VCENTER | DT_VCENTER | DT_SINGLELINE);
    //DrawText(szBtnText.GetBuffer(), &rect, DT_VCENTER | DT_VCENTER | DT_SINGLELINE);
    //DeleteObject(nxhBrush);
    return CMFCButton::OnDraw(pDC, rect, uiState);
}


//void CNxButton::OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState)
//{
//    // TODO: Add your specialized code here and/or call the base class
//
//    return CMFCButton::OnDrawBorder(pDC, rectClient, uiState);
//}


//void CNxButton::OnDrawFocusRect(CDC* pDC, const CRect& rectClient)
//{
//    // TODO: Add your specialized code here and/or call the base class
//
//    return CMFCButton::OnDrawFocusRect(pDC, rectClient);
//}


//void CNxButton::OnDrawText(CDC* pDC, const CRect& rect, const CString& strText, UINT uiDTFlags, UINT uiState)
//{
//    // TODO: Add your specialized code here and/or call the base class
//
//    return CMFCButton::OnDrawText(pDC, rect, strText, uiDTFlags, uiState);
//}


//void CNxButton::OnFillBackground(CDC* pDC, const CRect& rectClient)
//{
//    // TODO: Add your specialized code here and/or call the base class
//
//    return CMFCButton::OnFillBackground(pDC, rectClient);
//}


//void CNxButton::OnDrawParentBackground(CDC* pDC, CRect rectClient)
//{
//    // TODO: Add your specialized code here and/or call the base class
//
//    return CMFCButton::OnDrawParentBackground(pDC, rectClient);
//}

void CNxButton::SetColors(COLORREF rgbBkGrnd, COLORREF rgbFrGrnd, COLORREF rgbDisabledBkGrnd, COLORREF rgbDisabledFrGrnd)
{
    m_imgFlag = NX_USE_COLOR_AND_TEXT;

    m_rgbBkGrnd = rgbBkGrnd;
    m_rgbFrGrnd = rgbFrGrnd;
    m_rgbDisabledBkGrnd = rgbDisabledBkGrnd;
    m_rgbDisabledFrGrnd = rgbDisabledFrGrnd;
}

void CNxButton::SetColorsWithBorder(COLORREF rgbBkGrnd, COLORREF rgbFrGrnd, COLORREF rgbDisabledBkGrnd, COLORREF rgbDisabledFrGrnd, COLORREF rgbBorder)
{
    m_imgFlag = NX_USE_COLOR_AND_TEXT_BORDER;

    m_rgbBkGrnd = rgbBkGrnd;
    m_rgbFrGrnd = rgbFrGrnd;
    m_rgbDisabledBkGrnd = rgbDisabledBkGrnd;
    m_rgbDisabledFrGrnd = rgbDisabledFrGrnd;
    m_rgbBorder = rgbBorder;
}


void CNxButton::SetColorsCircle(COLORREF rgbBkGrnd, COLORREF rgbFrGrnd, COLORREF rgbDisabledBkGrnd, COLORREF rgbDisabledFrGrnd, int font_size)
{
    m_imgFlag = NX_USE_COLOR_CIRCLE_TEXT;

    m_rgbBkGrnd = rgbBkGrnd;
    m_rgbFrGrnd = rgbFrGrnd;
    m_rgbDisabledBkGrnd = rgbDisabledBkGrnd;
    m_rgbDisabledFrGrnd = rgbDisabledFrGrnd;
    m_fontSize = font_size;
}


void CNxButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

    // TODO:  Add your code to draw the specified item
    auto hDc = lpDrawItemStruct->hDC;
    CRect rcBtn = lpDrawItemStruct->rcItem;
    switch (m_imgFlag)
    {
    case NX_USE_COLOR_AND_TEXT_BORDER:
    {
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
                dwHorizStyle = BS_RIGHT;
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
                dwHorizStyle = BS_RIGHT;
            }
            DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, dwHorizStyle | DT_VCENTER | DT_SINGLELINE);
            DeleteObject(nxhBrush);
        }

        
        auto pDC = CDC::FromHandle(hDc);
        CPen Pen(PS_SOLID, 1, m_rgbBorder);
        CRect rc = lpDrawItemStruct->rcItem;
        auto oldPen =pDC->SelectObject(&Pen);
        pDC->MoveTo(rc.left, rc.top);
        pDC->LineTo(rc.right - 1, rc.top);
        pDC->LineTo(rc.right - 1, rc.bottom - 1);
        pDC->LineTo(rc.left, rc.bottom - 1);
        pDC->LineTo(rc.left, rc.top);
        pDC->SelectObject(&oldPen);

        break;
    }
    case NX_USE_COLOR_AND_TEXT:
    {
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
                dwHorizStyle = BS_RIGHT;
            }

            DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, dwHorizStyle | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
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
                dwHorizStyle = BS_RIGHT;
            }
            DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, dwHorizStyle | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            DeleteObject(nxhBrush);
        }
        break;
    }
    case NX_USE_COLOR_CIRCLE_TEXT:
        {/*GetRValue, GetBValue, GetGValue*/
            //HBRUSH nxhBrush = CreateSolidBrush(m_rgbBkGrnd);
            Gdiplus::Graphics graphics(hDc);
            //gdiplus::Color rgb;
            Gdiplus::Color gdipColor(255, 0, 0, 0);
            gdipColor.SetFromCOLORREF(m_rgbBkGrnd);
            Gdiplus::SolidBrush blackBrush(gdipColor);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            rcBtn.DeflateRect(1, 1);
            graphics.FillEllipse(&blackBrush, rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height());

            Gdiplus::Font myFont(L"Arial", (Gdiplus::REAL)m_fontSize);
            Gdiplus::RectF layoutRect((Gdiplus::REAL)rcBtn.left, (Gdiplus::REAL)rcBtn.top, 
                (Gdiplus::REAL)rcBtn.Width(), (Gdiplus::REAL)rcBtn.Height());
            Gdiplus::StringFormat format;
            format.SetAlignment(Gdiplus::StringAlignmentCenter);
            format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            gdipColor.SetFromCOLORREF(m_rgbFrGrnd);
            Gdiplus::SolidBrush foreBrush(gdipColor);
            CString szBtnText;
            GetWindowText(szBtnText);

            // Draw string.
            graphics.DrawString(
                szBtnText,
                szBtnText.GetLength(),
                &myFont,
                layoutRect,
                &format,
                &foreBrush);



        }
        break;
    }
}

