// FileRepoPage.cpp : implementation file
//

#include "stdafx.h"
//#include "RMCMainTabView.h"
#include "FlatDlgBase.h"
#include "afxdialogex.h"


// CFlatDlgBase dialog

IMPLEMENT_DYNAMIC(CFlatDlgBase, CDialogEx)

CFlatDlgBase::CFlatDlgBase(int nID, CWnd* pParent /*=NULL*/)
	: CDialogEx(nID, pParent)
	,m_rgbBkgrnd(RGB(255,255,255))
{
	m_bkBrush.CreateSolidBrush(m_rgbBkgrnd);
}

CFlatDlgBase::~CFlatDlgBase()
{
}

void CFlatDlgBase::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFlatDlgBase, CDialogEx)
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CFlatDlgBase message handlers


void CFlatDlgBase::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CDialogEx::OnPaint() for painting messages
    CRect rect;
    CBrush br;
    //br.FromHandle
    GetClientRect(&rect);
    dc.FillRect(&rect, &m_bkBrush);

}

void CFlatDlgBase::AddFontSize(int nPercent, CWnd &ctrlWnd, bool bBold, bool bItalic)
{
    auto pfont = GetFont();
    LOGFONT lf;
    pfont->GetLogFont(&lf);
    lf.lfHeight += lf.lfHeight * nPercent / 100;
    if (bBold)
    {
        lf.lfWeight = FW_BOLD;
    }
    lf.lfItalic = bItalic;

    lf.lfQuality = CLEARTYPE_QUALITY;
    auto hBoldFont = CreateFontIndirect(&lf);
    auto pBold = CFont::FromHandle(hBoldFont);
    ctrlWnd.SetFont(pBold);
}



HBRUSH CFlatDlgBase::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here
    pDC->SetBkColor(m_rgbBkgrnd);

    // TODO:  Return a different brush if the default is not desired
    return (HBRUSH)m_bkBrush.GetSafeHandle();
}

void CFlatDlgBase::SetBackColor(COLORREF rgbBkgrnd)
{
	m_rgbBkgrnd = rgbBkgrnd;
	m_bkBrush.DeleteObject();
	m_bkBrush.CreateSolidBrush(m_rgbBkgrnd);
}

BOOL CFlatDlgBase::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    CRect rect;
    CBrush br;
    //br.FromHandle
    GetClientRect(&rect);
    pDC->FillRect(&rect, &m_bkBrush);

    return /*CDialogEx::OnEraseBkgnd(pDC)*/TRUE;
}


void CFlatDlgBase::MoveWindowPos(CWnd * pwnd, int right, int bottom, BOOL bKeepLeft)
{
    RECT rc;
    pwnd->GetWindowRect(&rc);
    ScreenToClient(&rc);
    if (right != 0) {
        if (!bKeepLeft) rc.left = right - (rc.right - rc.left);
        rc.right = right;
    }
    if (bottom != 0)
    {
        if (!bKeepLeft) rc.top = bottom - (rc.bottom - rc.top);
        rc.bottom = bottom;
    }
    pwnd->MoveWindow(&rc, TRUE);
}

void CFlatDlgBase::ScaleCtrlWnd(CWnd * pwnd, CRect &rc, float fFactor)
{
    CRect scaledRc = rc;
    scaledRc.left = (int)(scaledRc.left * fFactor);
    scaledRc.top = (int)(scaledRc.top * fFactor);
    scaledRc.right = (int)(scaledRc.right * fFactor);
    scaledRc.bottom = (int)(scaledRc.bottom * fFactor);
    if (scaledRc.top == scaledRc.bottom)
    {
        scaledRc.bottom++;
    }

    pwnd->MoveWindow(&scaledRc, TRUE);
}

void CFlatDlgBase::SetChildRect(CWnd *pWnd, vector<CtrlSizeData> &ctrlData)
{
    ASSERT(pWnd != NULL);
    CRect rc;
    CtrlSizeData ctrlSize;
    pWnd->GetWindowRect(&rc);
    ScreenToClient(&rc);
    ctrlSize.rc = rc;
    ctrlSize.pCtrl = pWnd;
    ctrlData.push_back(ctrlSize);
}

void CFlatDlgBase::ScaleControlHorizontally(int index, int cx, const vector<CtrlSizeData> &ctrlData)
{
    int x = ctrlData[index].rc.left * cx / ctrlData[0].rc.Width();
    int w = ctrlData[index].rc.Width() * cx / ctrlData[0].rc.Width();
    ctrlData[index].pCtrl->SetWindowPos(this, x, ctrlData[index].rc.top, w, ctrlData[index].rc.Height(), SWP_NOZORDER);
}

void CFlatDlgBase::ScaleRightHorizontally(int index, int cx, const vector<CtrlSizeData> &ctrlData)
{
    int x = ctrlData[index].rc.right * cx / ctrlData[0].rc.Width() - ctrlData[index].rc.Width();
    int w = ctrlData[index].rc.Width();
    ctrlData[index].pCtrl->SetWindowPos(this, x, ctrlData[index].rc.top, w, ctrlData[index].rc.Height(), SWP_NOZORDER);
}

void CFlatDlgBase::ScaleLeftHorizontally(int index, int cx, const vector<CtrlSizeData> &ctrlData)
{
    int x = ctrlData[index].rc.left * cx / ctrlData[0].rc.Width();
    int w = ctrlData[index].rc.Width();
    ctrlData[index].pCtrl->SetWindowPos(this, x, ctrlData[index].rc.top, w, ctrlData[index].rc.Height(), SWP_NOZORDER);
}

void CFlatDlgBase::MoveRightHorizontally(int index, int cx, const vector<CtrlSizeData> &ctrlData)
{
    ctrlData[index].pCtrl->SetWindowPos(this, cx - ctrlData[0].rc.right + ctrlData[index].rc.left, ctrlData[index].rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CFlatDlgBase::ShrinkRightHorizontally(int index, int cx, const vector<CtrlSizeData> &ctrlData)
{

    ctrlData[index].pCtrl->SetWindowPos(this, 0, 0, ctrlData[index].rc.Width() + (cx - ctrlData[0].rc.right), ctrlData[index].rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
}


bool CFlatDlgBase::IsTextTruncated(CWnd *pCtrl, int percent)
{
    auto pfont = GetFont();
    auto  pdc = pCtrl->GetDC();
    CFont &font = GetFontFromSize(percent, *pCtrl, false, false);
    auto hOldFont = pdc->SelectObject(font.GetSafeHandle());
    CRect rc;
    pCtrl->GetWindowRect(&rc);
    CString ptr;
    pCtrl->GetWindowText(ptr);
    auto size = pdc->GetTextExtent(ptr);
    pdc->SelectObject(hOldFont);
    pCtrl->ReleaseDC(pdc);
    font.DeleteObject();
    return (size.cx > rc.Width());
}

int CFlatDlgBase::GetTextWidth(CWnd *pCtrl, int percent)
{
	auto pfont = GetFont();
	auto  pdc = pCtrl->GetDC();
	CFont &font = GetFontFromSize(percent, *pCtrl, false, false);
	auto hOldFont = pdc->SelectObject(font.GetSafeHandle());
	CString ptr;
	pCtrl->GetWindowText(ptr);
	auto size = pdc->GetTextExtent(ptr);
	pdc->SelectObject(hOldFont);
	pCtrl->ReleaseDC(pdc);
	font.DeleteObject();
	return (size.cx);
}



CFont &CFlatDlgBase::GetFontFromSize(int nPercent, CWnd &ctrlWnd, bool bBold, bool bItalic)
{
    auto pfont = GetFont();
    LOGFONT lf;
    pfont->GetLogFont(&lf);
    lf.lfHeight += lf.lfHeight * nPercent / 100;
    if (bBold)
    {
        lf.lfWeight = FW_BOLD;
    }
    lf.lfItalic = bItalic;

    lf.lfQuality = CLEARTYPE_QUALITY;
    static CFont resultFont;
    resultFont.CreateFontIndirectW(&lf);
    return resultFont;
}

bool CFlatDlgBase::AddTooTipWhenTruncated(CWnd *pParentWnd, CWnd *pWnd, CToolTipCtrl *pTip, CString tipText, int nToolID)
{
    CRect rc;
    pWnd->GetWindowRect(&rc);
    pParentWnd->ScreenToClient(&rc);
    pTip->DelTool(pParentWnd, nToolID);
    pTip->AddTool(pParentWnd, tipText, &rc, nToolID);
    if (IsTextTruncated(pWnd, 50))
    {
        pTip->Activate(TRUE);
        return true;
    }
    else
    {
        pTip->Activate(FALSE);
        return false;
    }
}
