

#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <assert.h>

#include <list>
#include <assert.h>


#include "commonui.hpp"
#include "resource.h"
#include "dlgtemplate.hpp"
#include "dlgaddress.hpp"
#include "dlgshare.hpp"
#include <string>
#include <richedit.h>
#include <string.h>
#include "..\uiutilities\uiutilities.hpp"
#include "CGdiPlusBitmap.h"
#include "gdiplus.h"

LRESULT CALLBACK ButtonControlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);


using namespace uiUtilities;

CDlgEmailAddr::CDlgEmailAddr() : CDlgTemplate(IDD_DIALOG_SHARE), _hIcon(NULL), _hFontTitle(NULL), _readonly(false)
{
    
}

CDlgEmailAddr::CDlgEmailAddr(_In_ const std::wstring& address, _In_ CDlgTemplate *p) : CDlgTemplate(IDD_ADDRESS_DIALOG),
    _hIcon(NULL),
    _hFontTitle(NULL),
    _readonly(false)
{
    _bValidAddress = isValidEmailAddress(address.c_str());
    SetFile(address);
    _dlgShare = p;
}

CDlgEmailAddr::~CDlgEmailAddr()
{
}

void CDlgEmailAddr::OnDestroy()
{
    DeleteObject(_hValidBrush);
    DeleteObject(_hInvalidBrush);
    delete this;
}

void CDlgEmailAddr::ResizeDialogBox()
{
    HWND hDlg = GetHwnd();
    RECT rcDlg;
    GetWindowRect(hDlg, &rcDlg);

}

BOOL CDlgEmailAddr::OnInitialize()
{
    HWND hDlg = GetHwnd();
    RECT rcDlg;
    _hValidBrush = CreateSolidBrush(RGB(224,224,224));
    _hInvalidBrush = CreateSolidBrush(RGB(255, 0, 0));

    GetWindowRect(hDlg, &rcDlg);
    MapWindowPoints(GetDesktopWindow(), _dlgShare->GetHwnd(), (LPPOINT)&rcDlg, 2);
    
    HWND hwndStatic = GetDlgItem(hDlg, IDC_STATIC_ADRESS);
    SetWindowText(hwndStatic, _address.c_str());
    HDC hdc = GetDC(hwndStatic);
    SIZE sz;
    RECT rcStatic;
    RECT rcClose;
    HFONT hfont = GetWindowFont(hwndStatic);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hfont);
    GetTextExtentPoint(hdc, _address.c_str(), (int)_address.length(), &sz);
    SelectObject(hdc, hOldFont);
    GetWindowRect(GetDlgItem(hDlg, IDC_STATIC_ADRESS), &rcStatic);
    MapWindowPoints(GetDesktopWindow(), hDlg, (LPPOINT)&rcStatic, 2);
    GetWindowRect(GetDlgItem(hDlg, ID_CLOSE), &rcClose);
    MapWindowPoints(GetDesktopWindow(), hDlg, (LPPOINT)&rcClose, 2);
    if (_bValidAddress)
    { //We have icon here
        rcStatic.right = rcStatic.left + sz.cx;
    }
    else
    { // We Remove icon here
        ShowWindow(GetDlgItem(hDlg, IDC_PERSON), SW_HIDE);
        rcStatic.left = 0;
        rcStatic.right = sz.cx;
    }
    MoveWindow(hwndStatic, rcStatic.left, rcStatic.top, RectWidth(rcStatic), RectHeight(rcStatic), TRUE);
    int closeWidth = RectWidth(rcClose);
    rcClose.left = rcStatic.right+closeWidth / 2;
    rcClose.right = rcClose.left + closeWidth;
    MoveWindow(GetDlgItem(hDlg, ID_CLOSE), rcClose.left, rcClose.top, RectWidth(rcClose), RectHeight(rcClose), TRUE);

    _rcLocation.right = _rcLocation.left + rcClose.right + closeWidth * 3 / 2;
    _rcLocation.bottom = _rcLocation.top + RectHeight(rcDlg);

    MoveWindow(hDlg, _rcLocation.left, _rcLocation.top, RectWidth(_rcLocation), RectHeight(_rcLocation), TRUE);

    ReleaseDC(hwndStatic,hdc);
    ShowWindow(hDlg, SW_SHOW);

    auto ret = SetWindowSubclass(GetDlgItem(hDlg, ID_CLOSE), ButtonControlProc, 11, (CONST DWORD_PTR)this);
    _bMouseOverButton = FALSE;

    return FALSE;
}

void CDlgEmailAddr::WindowScreenLocation(RECT *addrWndLocation, int nPos)
{

    *addrWndLocation = _rcLocation;
    ScrollToScreen(*addrWndLocation, nPos);
}

void CDlgEmailAddr::WindowScrollLocation(RECT *addrWndLocation)
{
    *addrWndLocation = _rcLocation;
}

void CDlgEmailAddr::ResizeAddressToFit()
{ //This is the wrong place to position the window 
    HWND hDlg = GetHwnd();
    RECT rcDlg;

    GetWindowRect(hDlg, &rcDlg);
    MapWindowPoints(GetDesktopWindow(), _dlgShare->GetHwnd(), (LPPOINT)&rcDlg, 2);

    HWND hwndStatic = GetDlgItem(hDlg, IDC_STATIC_ADRESS);
    SetWindowText(hwndStatic, _address.c_str());
    //SIZE sz;
    RECT rcStatic;
    RECT rcClose;
    RECT rcIcon;
    GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_STATIC_ADRESS), &rcStatic);
    GetWindowRectRelative(hDlg, GetDlgItem(hDlg, ID_CLOSE), &rcClose);
    GetWindowRectRelative(hDlg,GetDlgItem(hDlg, IDC_STATIC_PERSON), &rcIcon);
    int closeWidth = RectWidth(rcClose);

    _rcLocation.right = _nMaxRight - 3;
    auto nTotalWidth = RectWidth(_rcLocation);
    int nStaticWidth = nTotalWidth - closeWidth / 8 - 2 * closeWidth - (rcStatic.left - rcIcon.left);
    if (_bValidAddress)
    { //We have icon here
        rcStatic.right = rcStatic.left + nStaticWidth;
    }
    else
    { // We Remove icon here
        ShowWindow(GetDlgItem(hDlg, IDC_STATIC_PERSON), SW_HIDE);
        rcStatic.left = 0;
        rcStatic.right = nStaticWidth;
    }
    MoveWindow(hwndStatic, rcStatic.left, rcStatic.top, RectWidth(rcStatic), RectHeight(rcStatic), TRUE);
    rcClose.left = rcStatic.right + closeWidth / 8;
    rcClose.right = rcClose.left + closeWidth;
    MoveWindow(GetDlgItem(hDlg, ID_CLOSE), rcClose.left, rcClose.top, RectWidth(rcClose), RectHeight(rcClose), TRUE);

    _rcLocation.right = _rcLocation.left + rcClose.right + 4 * closeWidth;
    _rcLocation.bottom = _rcLocation.top + RectHeight(rcDlg);

    MoveWindow(hDlg, _rcLocation.left, _rcLocation.top, RectWidth(_rcLocation), RectHeight(_rcLocation), TRUE);

    ShowWindow(hDlg, SW_SHOW);


}
void CDlgEmailAddr::MoveWindowLocation(POINT pt, int nPos)
{
    HWND hDlg = GetHwnd();
    {
        auto width = RectWidth(_rcLocation);
        auto height = RectHeight(_rcLocation);
        MoveWindow(hDlg, pt.x, pt.y, width, height, TRUE);
        _rcLocation = { pt.x, pt.y, pt.x + width, pt.y + height };
        ScreenToScroll(_rcLocation, nPos);
    }
}

void CDlgEmailAddr::ResizeWindow(int nWidth,  int nHeight)
{
    HWND hDlg = GetHwnd();
    {
        auto width = RectWidth(_rcLocation);
        auto height = RectHeight(_rcLocation);
        MoveWindow(hDlg, _rcLocation.left, _rcLocation.top, nWidth, nHeight, TRUE);
        _rcLocation = { _rcLocation.left, _rcLocation.top, _rcLocation.left + nWidth, _rcLocation.top + nHeight };


        HWND hwndStatic = GetDlgItem(hDlg, IDC_STATIC_ADRESS);
        RECT rcStatic;
        RECT rcClose;
        RECT rcIcon;
        GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_STATIC_ADRESS), &rcStatic);
        GetWindowRectRelative(hDlg, GetDlgItem(hDlg, ID_CLOSE), &rcClose);
        GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_STATIC_PERSON), &rcIcon);

        int closeWidth = RectWidth(rcClose);

        int nStaticWidth;
        if (_bValidAddress)
        { //We have icon here
            nStaticWidth = nWidth - 2 * closeWidth -rcStatic.left;
        }
        else
        { // We Remove icon here
            //ShowWindow(GetDlgItem(hDlg, IDC_PERSON), SW_HIDE);
            rcStatic.left = 0;
            nStaticWidth = nWidth - 2 * closeWidth;
        }
        rcStatic.right = rcStatic.left + nStaticWidth;

        MoveWindow(hwndStatic, rcStatic.left, rcStatic.top, RectWidth(rcStatic), RectHeight(rcStatic), TRUE);

        rcClose.left = rcStatic.right ;
        rcClose.right = rcClose.left + closeWidth;
        MoveWindow(GetDlgItem(hDlg, ID_CLOSE), rcClose.left, rcClose.top, RectWidth(rcClose), RectHeight(rcClose), TRUE);
        InvalidateRect(hDlg, NULL, TRUE);
    }
}



BOOL CDlgEmailAddr::IsTheSameAddress(std::wstring inputAddr)
{
    
    return !_wcsicmp(_address.c_str(), inputAddr.c_str());
}

BOOL CDlgEmailAddr::OnNotify(_In_ LPNMHDR lpnmhdr)
{
    HWND hDlg = GetHwnd();
    if (lpnmhdr->code == NM_CLICK)
    {
    }

    return CDlgTemplate::OnNotify(lpnmhdr);
}

BOOL CDlgEmailAddr::OnCommand(WORD notify, WORD id, HWND hwnd)
{
    HWND hDlg = GetHwnd();
    switch (id)
    {
    case ID_CLOSE:
        ((CEmailAddrDlgBase*)_dlgShare)->RemoveAddress(this);
        ((CEmailAddrDlgBase*)_dlgShare)->ReArrangeAddresses();
        ((CEmailAddrDlgBase*)_dlgShare)->SetEditFocus();
        CDlgTemplate::OnOk();
        break;
    }
    return CDlgTemplate::OnCommand(notify, id, hwnd);
}

using namespace Gdiplus;

BOOL CDlgEmailAddr::OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct)
{
    HDC              hDc = lpDrawItemStruct->hDC;
    RECT             rcBtn = lpDrawItemStruct->rcItem;

    switch (lpDrawItemStruct->CtlID)
    {
    case ID_CLOSE:
        {
            CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
            bool bBmpLoaded = false;
            Color clr = _bValidAddress ? Color(224, 224, 224) : Color(255, 255, 255);
            int hoverID = IDB_CLOSE_HOVER;
            int normalID = IDB_CLOSE_NORMAL;
            if (TRUE == _bMouseOverButton )
            {
                bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(hoverID), L"PNG", _hInstance);
            }
            else {
                bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(normalID), L"PNG", _hInstance);
            }
            if (bBmpLoaded)
            {
                Gdiplus::Graphics graphics(hDc);
                Rect rect (rcBtn.left, rcBtn.top, RectWidth(rcBtn), RectHeight(rcBtn)) ;
                SolidBrush bkBrush(clr);
                graphics.FillRectangle(&bkBrush, rect);
                graphics.SetSmoothingMode(SmoothingModeHighQuality);
                int iconWidth = RectWidth(rcBtn);
                int iconHeight = RectHeight(rcBtn);
                Rect rc(rcBtn.left, (iconHeight - iconWidth) / 2, iconWidth, iconWidth);
                graphics.DrawImage(*pBitmap, (const Rect&)rc);
            }
            delete(pBitmap);
        }
    break;
    case IDC_PERSON:
    {
        CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
        bool bBmpLoaded = false;
        Color clr = Color(224, 224, 224);
        bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_PERSON), L"PNG", _hInstance);
        if (bBmpLoaded)
        {
            Gdiplus::Graphics graphics(hDc);
            Rect rect(rcBtn.left, rcBtn.top, RectWidth(rcBtn), RectHeight(rcBtn));
            SolidBrush bkBrush(clr);
            graphics.FillRectangle(&bkBrush, rect);
            graphics.SetSmoothingMode(SmoothingModeHighQuality);
            int iconWidth = RectWidth(rcBtn);
            int iconHeight = RectHeight(rcBtn);
            Rect rc(rcBtn.left, (iconHeight - iconWidth) / 2, iconWidth, iconWidth);
            graphics.DrawImage(*pBitmap, (const Rect&)rc);
        }
        delete(pBitmap);
    }
    break;
    }

    return FALSE;
}

void CDlgEmailAddr::OnCancel()
{
}

void CDlgEmailAddr::SetDisplayLocation(LPPOINT pPt, LPPOINT pPtMax, int nScrollPos )
{
    assert(pPt != NULL);
    int nWidth = RectWidth(_rcLocation);
    int nHeight = RectHeight(_rcLocation);
    _rcLocation.left = pPt->x;
    _rcLocation.top = ScreenToScroll(pPt->y, nScrollPos);
    _rcLocation.right = _rcLocation.left + nWidth;
    _rcLocation.bottom = _rcLocation.top + nHeight;
    _nMaxRight = pPtMax->x;
    _nScrollPos = nScrollPos;
}

INT_PTR CDlgEmailAddr::OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = GetHwnd();
    switch (uMsg)
    {
    case WM_CTLCOLORSTATIC:
        if (lParam == (LPARAM)GetDlgItem(hDlg, IDC_STATIC_ADRESS))
        {
            HDC hdc = (HDC)wParam;
            HBRUSH hretBrush = (_bValidAddress) ? _hValidBrush : _hInvalidBrush;
            SetBkColor(hdc, (_bValidAddress) ? RGB(224, 224, 224) : RGB(255,255,255));
            return (INT_PTR)hretBrush;
        }
    }
    return CDlgTemplate::OnRestWMMessage(uMsg, wParam, lParam);
}

BOOL CDlgEmailAddr::OnEraseBkGnd(HDC hDC)
{
    return FALSE;
}

BOOL CDlgEmailAddr::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc;
    HWND hDlg = GetHwnd();
    RECT rect;
    HBRUSH hbrush;

    RECT childRect;
    hdc = BeginPaint(hDlg, &ps);
    GetWindowRectRelative(hDlg, GetDlgItem(hDlg, ID_CLOSE), &childRect);
    GetClientRect(hDlg, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

    if (_bValidAddress)
    {
        hbrush = CreateSolidBrush(RGB(224, 224, 224));
        rect.right = childRect.right + RectWidth(childRect) / 2;
        rect.bottom = rect.bottom - RectHeight(rect) / 10;
        FillRect(hdc, &rect, hbrush);
        DeleteObject(hbrush);
    }
    EndPaint(hDlg, &ps);
    return 0;
}

BOOL CDlgEmailAddr::OnSize(WPARAM wParam, LPARAM lParam)
{
    return CDlgTemplate::OnSize(wParam, lParam);;
}




void CDlgEmailAddr::OnOk()
{
}



LRESULT CDlgEmailAddr::OnSubclassButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = GetHwnd();
    //CHARRANGE cr = { 0, 0 };
    LRESULT ret = FALSE;
    RECT     rect;

    TRACKMOUSEEVENT tme;

    tme.cbSize = sizeof(tme);
    tme.hwndTrack = hwnd;
    tme.dwFlags = TME_LEAVE | TME_HOVER;
    tme.dwHoverTime = 1;

    int iControlID = GetWindowLongW(hwnd, GWL_ID);
    switch (uMsg)
    {
        case WM_LBUTTONDBLCLK:
            //PostMessage(hWnd, WM_LBUTTONDOWN, wParam, lParam);
            break;

        case WM_MOUSEMOVE:
        {
            TrackMouseEvent(&tme);  //"generate" WM_MOUSEHOVER and WM_MOUSELEAVE Message
                                    // return 0;
        }
        break;

        case WM_MOUSEHOVER:
        {
            if (!_bMouseOverButton)
            {
                _bMouseOverButton = TRUE;
                GetClientRect(hwnd, &rect);
                InvalidateRect(hwnd, &rect, TRUE);
            }

            return 0;
        }// case WM_MOUSEHOVER

        case WM_MOUSELEAVE:
        {
            if (_bMouseOverButton)
            {
                _bMouseOverButton = FALSE;
                GetClientRect(hwnd, &rect);
                InvalidateRect(hwnd, &rect, TRUE);
            }
            break;
        }
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

//----------------------BEGIN Subclass to capture tabs and enter for setfocus------ ---------------
LRESULT CALLBACK ButtonControlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    auto pDlg = reinterpret_cast<CDlgEmailAddr*>(dwRefData);
    if (pDlg)
    {
        return pDlg->OnSubclassButtonProc(hwnd, uMsg, wParam, lParam);
    }
    return FALSE;
}

