// OwnerDrawStatic.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "OwnerDrawStatic.h"
#include "gdiplus.h"

using namespace Gdiplus;

// COwnerDrawStatic

const RmColorSchemeEntry& GetColorScheme(wchar_t c)
{
    if (!(c >= 0 && c <= 255))
    {
        return colorSchemeDefault;

    }
    if (isalpha(c)) {
        return colorSchemeAlpha[c - (c >= L'a' ? L'a' : L'A')];
    }
    else if (isdigit(c)) {
        return colorSchemeDigit[c - L'0'];
    }
    else {
        return colorSchemeDefault;
    }
}


IMPLEMENT_DYNAMIC(COwnerDrawStatic, CStatic)

COwnerDrawStatic::COwnerDrawStatic()
{

}

COwnerDrawStatic::~COwnerDrawStatic()
{
}


BEGIN_MESSAGE_MAP(COwnerDrawStatic, CStatic)
END_MESSAGE_MAP()



// COwnerDrawStatic message handlers




void COwnerDrawStatic::PreSubclassWindow()
{
    // TODO: Add your specialized code here and/or call the base class

    CStatic::PreSubclassWindow();

    ModifyStyle(0, SS_OWNERDRAW);

}


void COwnerDrawStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

    // TODO:  Add your code to draw the specified item
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    switch (m_drawType)
    {
    case NX_DRAWTYPE_TEXT_GRAY_LAST_BLACK:
        break;
    case NX_DRAWTYPE_ROUNDRECT:
    {
        CRect rcBtn = lpDrawItemStruct->rcItem;
        CPen pen;
        pen.CreatePen(PS_SOLID, 1, RGB(225, 225, 225));
        auto OldPen = pDC->SelectObject(&pen);
        CBrush *pbr = CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH));
        auto OldBr = pDC->SelectObject(pbr);
        pDC->RoundRect(rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom, rcBtn.Height(), rcBtn.Height());
        pDC->SelectObject(OldPen);
        pDC->SelectObject(OldBr);
        break;
    }
    case NX_DRAWTYPE_GREEN_BKGRND:
    {
        CBrush br;
        br.CreateSolidBrush(NX_NEXTLABS_STANDARD_GREEN);
        //auto pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH));
        pDC->FillRect(&lpDrawItemStruct->rcItem, &br);
        break;
    }
    case NX_DRAWTYPE_WHITE_BKGRND:
	{
		auto pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH));
		pDC->FillRect(&lpDrawItemStruct->rcItem, pBrush);
        break;
	}
    case NX_DRAWTYPE_RECT:
    {
        CRect rc;
        GetClientRect(&rc);
        CPen pen(PS_SOLID, 1, m_rgbBorder);
        CBrush br(m_rgbInterior);
        auto oldPen = pDC->SelectObject(&pen);
        auto oldBrush = pDC->SelectObject(&br);
        pDC->Rectangle(&rc);
        pDC->SelectObject(&oldPen);
        pDC->SelectObject(&oldBrush);
        break;
    }
    case NX_SCALE_IMAGE:
    {
        DrawScaledImage(lpDrawItemStruct);
        break;
    }
    case NX_STORAGE_CHART:
    {
        CRect rc;
        GetClientRect(&rc);
        auto width = rc.Width();
        auto protectedWidth = width * m_protectInMB / m_totalInMB;
        auto unprotectedWidth = width * m_unprotectedInMB / m_totalInMB;
        CPen pen(PS_SOLID, 1, m_rgbBorder);
        CBrush br(m_rgbFree);
        auto oldPen = pDC->SelectObject(&pen);
        auto oldBrush = pDC->SelectObject(&br);
        pDC->Rectangle(&rc);
        pDC->SelectObject(&oldPen);
        pDC->SelectObject(&oldBrush);
        CPen nullPen(PS_NULL, 0, COLORREF(0));
        CBrush brP(m_rgbUnprotected);
        oldPen = pDC->SelectObject(&nullPen);
        oldBrush = pDC->SelectObject(&brP);
        CRect rc1(rc.left, rc.top, rc.left + (int)unprotectedWidth, rc.bottom);
        pDC->Rectangle(&rc1);
        pDC->SelectObject(&oldPen);
        pDC->SelectObject(&oldBrush);

        CBrush brU(m_rgbProtected);
        oldPen = pDC->SelectObject(&nullPen);
        oldBrush = pDC->SelectObject(&brU);
        CRect rc2(rc1.right, rc1.top, rc1.right + (int)protectedWidth, rc1.bottom);
        pDC->Rectangle(&rc2);
        pDC->SelectObject(&oldPen);
        pDC->SelectObject(&oldBrush);

        break;
    }
    case NX_BULLET_TEXT:
    {
        CRect rc;
        GetClientRect(&rc);
        auto width = rc.Height();
        CRect rect = rc;
        rect.right = width;

        //auto protectedWidth = width * m_protectInMB / m_totalInMB;
        //auto unprotectedWidth = width * m_unprotectedInMB / m_totalInMB;

        rect.DeflateRect(width / 4, width / 4);
        CPen pen(PS_SOLID, 1, m_rgbBullet);
        CBrush br(m_rgbBullet);
        auto oldPen = pDC->SelectObject(&pen);
        auto oldBrush = pDC->SelectObject(&br);
        pDC->Rectangle(&rect);
        pDC->SelectObject(&oldPen);
        pDC->SelectObject(&oldBrush);
        CString wndText;
        GetWindowText(wndText);
        rc.left +=  width * 3/2;
        pDC->DrawText(wndText, rc, DT_LEFT | DT_SINGLELINE);

    }

    case NX_DRAWTYPE_USERS_IMAGE:
    {   const int NX_MAX_PICS = 5;

        CString wndText;

        int j = 0;
		//Graphics 
		Graphics graphics(pDC->GetSafeHdc());
		SolidBrush backBrush(Color(255, 239, 102, 69));
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        CRect rc = lpDrawItemStruct->rcItem;
        auto totalPics = NX_MAX_PICS;

        if (totalPics >= (int)m_totalPics)
        {
            for (auto x : m_initials)
            {
                auto colorScheme = GetColorScheme(x.GetAt(0));
                Color bkColor;
                bkColor.SetFromCOLORREF(colorScheme.bg);
                backBrush.SetColor(bkColor);
                CRect currRc(rc.left + j * (rc.Height() - 1), rc.top, rc.left + (j + 1) * (rc.Height() - 1), rc.bottom - 1);
                graphics.FillEllipse(&backBrush, currRc.left, currRc.top, currRc.Width(), currRc.Height());
                Gdiplus::Font myFont(L"Arial", 8);
                RectF layoutRect((REAL)currRc.left, (REAL)currRc.top, (REAL)currRc.Width(), (REAL)currRc.Height());
                StringFormat format;
                format.SetAlignment(StringAlignmentCenter);
                format.SetLineAlignment(StringAlignmentCenter);
                Color frColor;
                frColor.SetFromCOLORREF(colorScheme.fg);
                SolidBrush foreBrush(frColor);


                // Draw string.
                graphics.DrawString(
                    x,
                    x.GetLength(),
                    &myFont,
                    layoutRect,
                    &format,
                    &foreBrush);
                j++;
            }
        } 
        else
        {   
            for (int i=0;i<totalPics; i++)
            {
                auto x = m_initials[i];
                auto colorScheme = GetColorScheme(x.GetAt(0));
                Color bkColor;
                bkColor.SetFromCOLORREF(colorScheme.bg);
                backBrush.SetColor(bkColor);
                Color frColor;
                frColor.SetFromCOLORREF(colorScheme.fg);
                SolidBrush foreBrush(frColor);

                if (i == totalPics - 1)
                {
                    wstringstream txtOut;
                    txtOut << m_totalPics - totalPics + 1;
                    x = L"+";
                    x += txtOut.str().c_str();
                    colorScheme = GetColorScheme(x.GetAt(1));
                    bkColor.SetFromCOLORREF(colorScheme.bg);
                    frColor.SetFromCOLORREF(colorScheme.fg);

                    backBrush.SetColor(bkColor);
                    foreBrush.SetColor(frColor);
                }

                CRect currRc(rc.left + j * (rc.Height() - 1), rc.top, rc.left + (j + 1) * (rc.Height() - 1), rc.bottom - 1);
                graphics.FillEllipse(&backBrush, currRc.left, currRc.top, currRc.Width(), currRc.Height());
                Gdiplus::Font myFont(L"Arial", 8);
                RectF layoutRect((REAL)currRc.left, (REAL)currRc.top, (REAL)currRc.Width(), (REAL)currRc.Height());
                StringFormat format;
                format.SetAlignment(StringAlignmentCenter);
                format.SetLineAlignment(StringAlignmentCenter);

                // Draw string.
                graphics.DrawString(
                    x,
                    x.GetLength(),
                    &myFont,
                    layoutRect,
                    &format,
                    &foreBrush);
                j++;
            }


        }

        break;
    }

    default:
        break;
    }

}


void COwnerDrawStatic::DrawScaledImage(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

    //int nOldPos = (m_nImgIndex - 1) % NX_RMC_MAX_INTROIMAGES;
    auto pdc = CDC::FromHandle(lpDrawItemStruct->hDC);
    ASSERT(pdc != NULL);
    CDC memDC;
    memDC.CreateCompatibleDC(pdc);
    HBITMAP hbm = m_hBitmap; //GetBitmap();
    auto oldOgj = memDC.SelectObject(hbm);

    CRect rc = lpDrawItemStruct->rcItem;
    //m_introImage.GetWindowRect(&rc);

    BITMAP bm;
    GetObject(hbm, sizeof(BITMAP), &bm);
    pdc->SetStretchBltMode(HALFTONE);
    pdc->StretchBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    memDC.SelectObject(oldOgj);
}

