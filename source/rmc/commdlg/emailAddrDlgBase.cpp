

#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <assert.h>

#include <list>
#include <cwchar>

#include "commonui.hpp"
#include "resource.h"
#include "dlgtemplate.hpp"
#include "emailAddrDlgBase.hpp"
#include <string>
#include <richedit.h>
#include "dlgaddress.hpp"
#include "..\uiutilities\uiutilities.hpp"
#include <Shlwapi.h>
#include "commondlg.hpp"

using namespace uiUtilities;
using namespace std;


//static ULONG_PTR m_gdiplusToken;


//wchar_t _enterPrompt[100] = L"Add more people..."; //This string will be loaded 


LRESULT CALLBACK EditControlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);


CEmailAddrDlgBase::CEmailAddrDlgBase(int nDlgID, bool bEmptyIsValid) : CDlgTemplate(nDlgID),
    _hIcon(NULL),
    _hFontTitle0(NULL),
    _hFontTitle(NULL),
    _readonly(false),
    _bEmptyIsValid(bEmptyIsValid)
{
}


CEmailAddrDlgBase::~CEmailAddrDlgBase()
{
}


//OnInitialize function is called from WM_INITDIALOG message
//Initialize the dialog state according to given data
BOOL CEmailAddrDlgBase::OnInitialize()
{
    HWND hDlg = GetHwnd();

	m_bPasteFlag = false;
	m_bRet = false;
	_addressList.empty();


	_bAddressSpaceFull = false;
	_bScroll = false;
	m_nScrollPos = 0;

	::GetClientRect(hDlg, &m_rcOriginalRect); //Initialize the m_rcOriginalRect for scroll bar

	RECT rcDesktoop;
	::GetClientRect(GetDesktopWindow(), &rcDesktoop);
	m_nCurHeight = RectHeight(rcDesktoop) * 8 / 10;  //Scroll start at 80% of desktop
	SetScrollbarInfo();

	SetFocus(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES));
	SendMessage(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), EM_SETEVENTMASK, 0, 0xffffffff | ENM_CLIPFORMAT);
	auto hEditWnd = GetDlgItem(hDlg, IDC_EDIT_ADDRESSES);

	//Subclassing edit control, so that we can change standard edit behavior
	auto ret = SetWindowSubclass(hEditWnd, EditControlProc, 10, (CONST DWORD_PTR)this);

	RECT rcEdit;
	GetWindowRectRelative(hDlg, hEditWnd, &rcEdit);
	_ptInitEditPos = { rcEdit.left,rcEdit.top };
	_initEditSize = { RectWidth(rcEdit), RectHeight(rcEdit) };
	_minimalAddrWidth = 5 * RectHeight(rcEdit);
	_currentY = _ptInitEditPos.y; //This is scroll coordinate
	_addressHeight = 0;



     return FALSE;
}

bool CEmailAddrDlgBase::IsAllAddressValid()
{
    bool bEnableFlag = true;
    if ((_addressList.size() == 0) && !_bEmptyIsValid)
    {
        bEnableFlag = false;
    }
    for (auto x : _addressList)
    {
        if (!x->IsValidAddress())
        {
            bEnableFlag = false;
        }
    }
    return bEnableFlag;
}


//ResizeWindowToFit add or reduce the height according to address block change
//If parameter addHeight is positive, the dialog is enlarged in height
//If parameter addHeight is negative, the dialog is reduced in height
bool CEmailAddrDlgBase::ResizeWindowToFit(int addHeight)
{
    HWND hDlg = GetHwnd();
    RECT rcDlg;
    GetWindowRectRelative(GetDesktopWindow(), hDlg, &rcDlg);
    auto nPos = GetScrollPos(hDlg, SB_VERT);

    //Make the static to be 'addHeight' bigger
    RECT rcStatic;
    GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_STATIC_ADDRESSES), &rcStatic);
    int nCurrHeight = RectHeight(m_rcOriginalRect);
    rcStatic.bottom += addHeight;
    MoveWindow(GetDlgItem(hDlg, IDC_STATIC_ADDRESSES), rcStatic.left, rcStatic.top, RectWidth(rcStatic), RectHeight(rcStatic), TRUE);
	int nAddrBottom = rcStatic.bottom;
	GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_OK), &rcStatic);
	rcStatic.bottom += addHeight;
	rcStatic.top += addHeight;
	MoveWindow(GetDlgItem(hDlg, IDC_OK), rcStatic.left, rcStatic.top, RectWidth(rcStatic), RectHeight(rcStatic), TRUE);
	GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDCANCEL), &rcStatic);
	rcStatic.bottom += addHeight;
	rcStatic.top += addHeight;
	MoveWindow(GetDlgItem(hDlg, IDCANCEL), rcStatic.left, rcStatic.top, RectWidth(rcStatic), RectHeight(rcStatic), TRUE);
	//temporary add move button code here, this should be redesigned in future.
	GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_REMOVEALLUSERS), &rcStatic);
	rcStatic.bottom += addHeight;
	rcStatic.top += addHeight;
	MoveWindow(GetDlgItem(hDlg, IDC_REMOVEALLUSERS), rcStatic.left, rcStatic.top, RectWidth(rcStatic), RectHeight(rcStatic), TRUE);
	m_rcOriginalRect.bottom += addHeight;
	rcDlg.bottom += addHeight;
    RECT rcClient;
    GetClientRect(hDlg, &rcClient);
    RECT rcDesktoop;
    GetClientRect(GetDesktopWindow(), &rcDesktoop);
    int nMaxHeight = RectHeight(rcDesktoop) * 8 / 10;
     if (RectHeight(m_rcOriginalRect) >= nMaxHeight) //Here we need to have title bar and windows taskbar to be shown.
    {
        if (nCurrHeight < nMaxHeight)
        {
            m_nCurHeight = nCurrHeight;
        }
        SCROLLINFO si = SetScrollbarInfo();

        int nMaxPos = RectHeight(m_rcOriginalRect) - m_nCurHeight;

        int nDelta = nMaxPos - m_nScrollPos;
        m_nScrollPos = nMaxPos;
        SetScrollPos(hDlg, SB_VERT, m_nScrollPos, TRUE);
        ScrollWindow(hDlg, 0, -nDelta, NULL, NULL);


        return false;
    }
    else if (_bScroll)
    {//This is situation that we are shorter than max height, yet we need to scroll back the content
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        GetScrollInfo(hDlg, SB_VERT, &si);
        int nDelta = 0 - m_nScrollPos;
        m_nScrollPos = 0;
        SetScrollPos(hDlg, SB_VERT, m_nScrollPos, TRUE);
        ScrollWindow(hDlg, 0, -nDelta, NULL, NULL);

        SetScrollbarInfo();

        return false;

    }
    else
    {
        //We do not do it to get scrollbar
        SetScrollbarInfo();     //This shoud remove
        MoveWindow(hDlg, rcDlg.left, rcDlg.top, RectWidth(rcDlg), RectHeight(rcDlg), TRUE);
        CenterWindowVertically(GetDesktopWindow(), hDlg);
       
        GetClientRect(hDlg, &m_rcOriginalRect);
        InvalidateRect(hDlg, NULL, true);
        return true;
    }
}


bool CEmailAddrDlgBase::FindEmailInThelist(const wchar_t *pszAddr)
{
    for (auto addr : _addressList)
    {
        if (addr->IsTheSameAddress(pszAddr))
        {
            return true;
        }
    }
    return false;
}

//function CreateAddressWindow() creates one new address window in the address area
//There are many situations when create a address window, this will be in the inline comments
void CEmailAddrDlgBase::CreateAddressWindow()
{
    if (_bClean)
    {
        return;
    }
    HWND hDlg = GetHwnd();
    wchar_t emailAddress[MAX_PATH];
    GetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, emailAddress, MAX_PATH);
    CreateAddressWindow(emailAddress);
}

void CEmailAddrDlgBase::CreateAddressWindow(const wchar_t *emailAddress)
{
    HWND hDlg = GetHwnd();
    auto nPos = GetScrollPos(hDlg, SB_VERT);

    if (lstrlen(emailAddress))
    {
        if (FindEmailInThelist(emailAddress))
        {
            SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, L"");
            return;
        }
        //Create an email address window here
        CDlgEmailAddr *paddr = new CDlgEmailAddr(emailAddress, this);
        if (paddr)
        {
            RECT rcEdit;
            _addressList.push_back(paddr);
            GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), &rcEdit);
            RECT rcStatic;
            GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_STATIC_ADDRESSES), &rcStatic);
            POINT pt;
            POINT ptMax = { rcStatic.right, rcStatic.bottom };

            pt.x = rcEdit.left;
            pt.y = rcEdit.top - (_addressHeight - _initEditSize.cy) / 2;

            //Allocate the address window at the position, we first calculated
            paddr->SetDisplayLocation(&pt, &ptMax, nPos);
            paddr->DoModaless(GetHwnd());

            RECT rcLocation;
            paddr->WindowScreenLocation(&rcLocation, nPos);
            if (!_addressHeight)
            {//We need height to calculate y correctly, so we do it for the initial position here
                _addressHeight = RectHeight(rcLocation);
                pt.y = rcEdit.top - (_addressHeight - _initEditSize.cy) / 2;
                paddr->SetDisplayLocation(&pt, &ptMax, nPos);
                paddr->WindowScreenLocation(&rcLocation, nPos);
            }

            int addressTop = ScreenToScroll(rcEdit.top - (_addressHeight - _initEditSize.cy) / 2, nPos);

            if (rcLocation.right + ADDRESS_MARGIN > rcStatic.right)
            {// Here is the situation that this window will be bigger than our container width
                if (rcLocation.left == _ptInitEditPos.x)
                {//This means that we are too big to fit to one line. Will need to resize to fit
                    paddr->ResizeWindow(RectWidth(rcStatic) - 20, RectHeight(rcLocation));
                    //Allocate the space for edit to next line
                    if (rcLocation.bottom + _addressHeight > rcStatic.bottom)
                    {//Remove the newly created Window
                     //Here we need to enlarge static window and move all windows below and Enlarge the whole dialog.
                        ResizeWindowToFit(_addressHeight);
                        nPos = GetScrollPos(hDlg, SB_VERT);
                    }
                    SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, L"");
                    paddr->WindowScreenLocation(&rcLocation, m_nScrollPos);
                    int currentY = rcLocation.bottom + (RectHeight(rcLocation) - RectHeight(rcEdit)) / 2;
                    MoveWindow(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), _ptInitEditPos.x, currentY, _initEditSize.cx, _initEditSize.cy, TRUE);
                    _currentY = ScreenToScroll(currentY, nPos);
                }
                else
                {//Normal sitiation, control is in one line, we need to allocate the space for next line for edit
                    if (rcLocation.bottom + _addressHeight > rcStatic.bottom)
                    {//Remove the newly created Window
                     //DeleteLastAddress();
                     //Here we need to enlarge static window and move all windows below and Enlarge the whole dialog.
                        ResizeWindowToFit(_addressHeight);
                        nPos = GetScrollPos(hDlg, SB_VERT);
                    }
                    //else
                    {//Move the address window into next line
                        paddr->WindowScreenLocation(&rcLocation, m_nScrollPos);
                        POINT ptNewPos = { _ptInitEditPos.x, rcLocation.top + _addressHeight };
                        paddr->MoveWindowLocation(ptNewPos, nPos);
                        //Move edit window next
                        paddr->WindowScreenLocation(&rcLocation, m_nScrollPos);
                        SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, L"");
                        int currentY = rcLocation.top + (RectHeight(rcLocation) - RectHeight(rcEdit)) / 2;
                        int editWidth = rcStatic.right - rcLocation.right - ADDRESS_MARGIN;
                        MoveWindow(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), rcLocation.right, currentY, editWidth, _initEditSize.cy, TRUE);
                        _currentY = ScreenToScroll(currentY, nPos);
                    }
                }
            }
            else
            { //We are in the situation the newly created address window fit inside the line
                rcEdit.left = rcLocation.right;
                if (RectWidth(rcEdit) < _minimalAddrWidth)
                {//This is the situation that the edit control is too small we need to move to next line
                    RECT rcStatic;
                    GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_STATIC_ADDRESSES), &rcStatic);
                    rcEdit.left = _ptInitEditPos.x;
                    OffsetRect(&rcEdit, 0, _addressHeight);
                    RECT rcScrollEdit = rcEdit;
                    ScreenToScroll(rcScrollEdit, nPos);

                    if (rcEdit.bottom + _initEditSize.cy / 4 > rcStatic.bottom)
                    {
                        ResizeWindowToFit(_addressHeight);
                        nPos = GetScrollPos(hDlg, SB_VERT);
                    }
                    {
                        ScrollToScreen(rcScrollEdit, m_nScrollPos);
                        int currentY = rcScrollEdit.top;
                        SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, L"");
                        int editWidth = rcStatic.right - _ptInitEditPos.x - ADDRESS_MARGIN; //Here we are in a new line
                        MoveWindow(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), rcEdit.left, currentY, editWidth, _initEditSize.cy, TRUE);
                        _currentY = ScreenToScroll(currentY, nPos);
                    }
                }
                else
                { //This is the situation that we only need to move the edit at the end of newly created address window

                    SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, L"");
                    int editTop = rcLocation.top + (RectHeight(rcLocation) - RectHeight(rcEdit)) / 2;
                    int editWidth = rcStatic.right - rcLocation.right - ADDRESS_MARGIN;
                    MoveWindow(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), rcEdit.left, editTop, editWidth, _initEditSize.cy, TRUE);
                }
                POINT ptNewPos = { rcLocation.left,  ScrollToScreen(addressTop, m_nScrollPos) };
                paddr->MoveWindowLocation(ptNewPos, nPos);
            }
        }
    }
    EnableWindow(GetDlgItem(hDlg, IDC_OK), IsAllAddressValid());
	EnableWindow(GetDlgItem(hDlg, IDC_REMOVEALLUSERS), _addressList.size() > 0);
}


BOOL CEmailAddrDlgBase::DeleteLastAddress()
{
 
    if(_addressList.size())
    {
        auto pAddr = _addressList.back();
        pAddr->OnCommand(0, ID_CLOSE, 0);
		return TRUE;
    }
	return FALSE;
}

//ReArrangeAddresses() function is called, when user removed one address window by backspace or clicked 
//
void CEmailAddrDlgBase::ReArrangeAddresses()
{
    HWND hDlg = GetHwnd();
    RECT rcEdit;
    auto nPos = GetScrollPos(hDlg, SB_VERT);

    GetWindowRect(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), &rcEdit);
    MapWindowPoints(GetDesktopWindow(), hDlg, (LPPOINT)&rcEdit, 2);
    //Redraw All Address Windows here.
    
    RECT rcStatic;

    GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_STATIC_ADDRESSES), &rcStatic);

    if(_addressList.size())
    {

        POINT ptStart = { _ptInitEditPos.x, ScrollToScreen(_ptInitEditPos.y - (_addressHeight - _initEditSize.cy) / 2, nPos) };
        _currentY = ScreenToScroll(_ptInitEditPos.y, nPos) ;
        RECT wndRect;
        for (auto addr : _addressList)
        {//reposition all windows here, if it in the same locatoin we do not move the widow
            assert(addr != NULL);
            //addr->WindowScreenLocation(&wndRect);
            addr->WindowScreenLocation(&wndRect, nPos);
            //ScrollToScreen(wndRect, nPos);
            if (wndRect.left == ptStart.x && wndRect.top == ptStart.y)
            { //We do not need to change the window, it located a the same place 

            }
            else if (wndRect.left != ptStart.x && wndRect.top == ptStart.y)
            {//We need to move window here
                addr->MoveWindowLocation(ptStart, nPos);

            }
            else if (wndRect.top != ptStart.y)
            {//We need to Test if it fit the space or not, 
                if (ptStart.x + RectWidth(wndRect) < rcStatic.right)
                {
                    addr->MoveWindowLocation(ptStart, nPos);
                } 
                else if (wndRect.top >= ptStart.y + _addressHeight)
                { //
                    ptStart.y += _addressHeight;
                    auto currentY = ptStart.y + (_addressHeight - _initEditSize.cy) / 2;
                    _currentY = ScreenToScroll(currentY, nPos);
                    ptStart.x = _ptInitEditPos.x;
                    addr->MoveWindowLocation(ptStart, nPos);
                }
            }
            addr->WindowScreenLocation(&wndRect, nPos);
            //ScrollToScreen(wndRect, nPos);
            ptStart = { wndRect.right, wndRect.top };
        }
        auto paddr = _addressList.back();
        paddr->WindowScreenLocation(&wndRect, nPos);

        if (rcStatic.right - wndRect.right < _minimalAddrWidth)
        { //This is the situation that we do not have enough space at the end
            auto currentY = wndRect.bottom + (_addressHeight - _initEditSize.cy) / 2;
            MoveWindow(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), _ptInitEditPos.x, currentY,
                rcStatic.right - _ptInitEditPos.x - ADDRESS_MARGIN, RectHeight(rcEdit), TRUE);
            _currentY = ScreenToScroll(currentY,nPos);
        } 
        else
        { 
            auto currentY = wndRect.top + (_addressHeight - _initEditSize.cy) / 2;
            MoveWindow(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), wndRect.right, currentY,
                rcStatic.right - wndRect.right - ADDRESS_MARGIN, RectHeight(rcEdit), TRUE);
            _currentY = ScreenToScroll(currentY, nPos);

        }
    }
    else
    { //this is at First situation

        MoveWindow(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), _ptInitEditPos.x, ScrollToScreen(_ptInitEditPos.y,nPos), _initEditSize.cx, _initEditSize.cy, TRUE);
    }
    GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), &rcEdit);
    if (rcEdit.bottom + 2 * _addressHeight < rcStatic.bottom)
    {//This will take care of very long email that used almost a line space.
        ResizeWindowToFit(-(2 * _addressHeight));
    }
    else if (rcEdit.bottom + _addressHeight < rcStatic.bottom)
    {
        ResizeWindowToFit(-_addressHeight);
    } 
    EnableWindow(GetDlgItem(hDlg, IDC_OK), IsAllAddressValid());
	EnableWindow(GetDlgItem(hDlg, IDC_REMOVEALLUSERS), _addressList.size() > 0);
}

void CEmailAddrDlgBase::SetEditFocus()
{
    HWND hDlg = GetHwnd();
    SetFocus(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES));
}


LRESULT CEmailAddrDlgBase::OnSubclassEditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = GetHwnd();
    CHARRANGE cr = { 0, 0 };
    LRESULT ret = FALSE;
    switch (uMsg)
    {
    case WM_SETFOCUS:
        if (_bClean)
        {
            Edit_SetSel(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), 0, 0);
        }
        break;
    case WM_KILLFOCUS:
        if (!_bClean)
        {
            //CreateAddressWindow();
        }
        break;
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        if (_bClean)
        {
            SetFocus(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES));
            return TRUE;
        }
        break;

    case WM_CHAR:
        switch (wParam) {
        case L';':
        case L' ':
            CreateAddressWindow();
            return TRUE;
        case VK_RETURN:
            if (IsWindowEnabled(GetDlgItem(hDlg, IDC_OK)))
            {
                m_bRet = true;
            }
            CreateAddressWindow();
            SetEditFocus();
            return TRUE;

        case VK_TAB:
            CreateAddressWindow();
            break;
        case VK_BACK:
            break;
        default:
            if (_bAddressSpaceFull)
            {
                return TRUE;
            }
            break;
        }
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_BACK:
            SendMessage(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), EM_EXGETSEL, 0, (LPARAM)(CHARRANGE*)&cr);
            if (cr.cpMin == cr.cpMax && cr.cpMin == 0)
            {
                wchar_t emailAddress[MAX_PATH];
                GetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, emailAddress, MAX_PATH);
                if (lstrcmp(emailAddress, _enterPrompt) == 0)
                { //When back space not chages the string, we can use it to remove previous address window
                    DeleteLastAddress();
                    return TRUE;
                }
            }
            break;
        case 0x56:	//V
            if ((GetKeyState(VK_LCONTROL) & 0x8000) || (GetKeyState(VK_RCONTROL) & 0x8000))
            {
                m_bPasteFlag = true;
            }

            break;

        case VK_INSERT:
            if ((GetKeyState(VK_LSHIFT) & 0x8000) || (GetKeyState(VK_LSHIFT) & 0x8000))
            {
                m_bPasteFlag = true;
            }
            break;
        }

    default:
        break;
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

//----------------------BEGIN Subclass to capture tabs and enter for setfocus------ ---------------
LRESULT CALLBACK EditControlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    auto pDlg = reinterpret_cast<CEmailAddrDlgBase*>(dwRefData);
    if (pDlg)
    {
        return pDlg->OnSubclassEditProc(hwnd, uMsg, wParam, lParam);
    }
    return FALSE;
}




bool CEmailAddrDlgBase::GetDataIntoVector()
{
	HWND hDlg = GetHwnd();
	_resultString.clear();
	WCHAR formatStr[] = L"%s;";
	WCHAR addressTagName[] = L"email_address";
	WCHAR addrInEdit[MAX_PATH];
	wchar_t msg[MAX_PATH];
	bool bFoundEmail = false;

	GetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, addrInEdit, MAX_PATH);
	if (!IsAllAddressValid())
	{
		wstring errmsgFmt = LoadStringResource(0, IDS_REMOVE_INVALID_EMAIL_ADDR)/*L"Please remove invalid email address."*/;
		swprintf_s(msg, MAX_PATH, errmsgFmt.c_str(), addrInEdit);
		wstring errTitle = LoadStringResource(0, IDS_ERROR)/*L"Error"*/;
		MessageBox(hDlg, msg, errTitle.c_str(), MB_OK);
		return false;
	}
	for (auto paddr : _addressList)
	{
		auto currentSize = _resultString.size();
		currentSize = currentSize == 0 ? 0 : currentSize - 1;
		auto emailAddress = paddr->GetAddress();
		size_t addSize = emailAddress.length() + 2;
		_resultString.resize(currentSize + addSize);
		auto pData = _resultString.data();
		swprintf_s((pData + currentSize), addSize, formatStr, emailAddress.c_str());
	}

	_addressList.clear();
	return true;
}



//NXSharingData &CEmailAddrDlgBase::GetResult()
int CEmailAddrDlgBase::GetResult(const wchar_t ** pResult)
{
	int retVal = (int)_resultString.size();
	*pResult = _resultString.data();
	return retVal;
}


BOOL CEmailAddrDlgBase::OnNotify(_In_ LPNMHDR lpnmhdr)
{
    HWND hDlg = GetHwnd();
 
    if (lpnmhdr->idFrom == IDC_EDIT_ADDRESSES)
    {
        if (lpnmhdr->code == EN_SELCHANGE)
        {

        }
    }
 
    return CDlgTemplate::OnNotify(lpnmhdr);
}

void CEmailAddrDlgBase::RepositionEditControl()
{
    HWND hDlg = GetHwnd();
    SIZE sz;
    RECT rcEdit;
    auto nPos = GetScrollPos(hDlg, SB_VERT);
    if (_addressList.size())
    {
        auto paddr = _addressList.back();
        auto hEdit = GetDlgItem(hDlg, IDC_EDIT_ADDRESSES);
        auto hdc = GetDC(hDlg);
        HFONT hfont = GetWindowFont(hDlg);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hfont);
        RECT rcStatic;
        GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_STATIC_ADDRESSES), &rcStatic);
        GetTextExtentPoint(hdc, _addressStr.c_str(), (int)_addressStr.length(), &sz);
        SelectObject(hdc, hOldFont);
        GetWindowRectRelative(hDlg, hEdit, &rcEdit);
        ScreenToScroll(rcEdit, nPos);
        if (sz.cx >= RectWidth(rcEdit) && rcEdit.left > _ptInitEditPos.x)
        {
            rcEdit.left = _ptInitEditPos.x;
            int offsetY = _addressHeight;
            ResizeWindowToFit(_addressHeight);
            {
                OffsetRect(&rcEdit, 0, offsetY);
                ScrollToScreen(rcEdit, m_nScrollPos);
                auto currentY = rcEdit.top;
                MoveWindow(hEdit, rcEdit.left, currentY, RectWidth(rcEdit), RectHeight(rcEdit), TRUE);
                _currentY = ScreenToScroll(currentY, nPos);
            }
        }
        else if (sz.cx >= RectWidth(rcEdit) && rcEdit.left == _ptInitEditPos.x)
        {//We do not change anything here, the whole line is one address

        }
        else
        {
            assert(paddr);
            RECT lastLocation;
            paddr->WindowScreenLocation(&lastLocation, nPos);
            if (lastLocation.right + sz.cx + _minimalAddrWidth < rcStatic.right && ScrollToScreen(_currentY,nPos) > lastLocation.bottom)
            {//Here we move the edit window back
                int width = rcStatic.right - lastLocation.right -ADDRESS_MARGIN;
                int height = RectHeight(rcEdit);
                int currentY = lastLocation.top + (_addressHeight - _initEditSize.cy) / 2;
                MoveWindow(hEdit, lastLocation.right, currentY, width, height, TRUE);
                _currentY = ScreenToScroll(currentY, nPos);
                ResizeWindowToFit(-_addressHeight);
            }
        }
        ReleaseDC(hEdit, hdc);
    }
    
}

void CEmailAddrDlgBase::SetEditTextColor(COLORREF rgbText)
{
    CHARFORMAT2 cf;
    HWND hDlg = GetHwnd();
    cf.cbSize = sizeof(CHARFORMAT2);
    cf.dwMask = CFM_COLOR;
    auto ret = ::SendMessage(::GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    cf.dwMask = CFM_COLOR;
    cf.dwEffects = 0;
    cf.crTextColor = rgbText;
    ret = ::SendMessage(::GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);

}

void CEmailAddrDlgBase::SetEditClean(HWND hDlg)
{
    SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, _enterPrompt);
    SetEditTextColor(RGB(128, 128, 128));
    _bClean = true;
}

int CEmailAddrDlgBase::ParsingPasteString(wchar_t *szAddrStr)
{
    auto delemeter = L",;\n\r ";
    wchar_t *ptr;
    auto pTok = wcstok_s(szAddrStr, delemeter,&ptr);
    do
    {
        CreateAddressWindow(pTok);
    } while ((pTok = wcstok_s(NULL, delemeter, &ptr)) != NULL);
    return 0;
}


BOOL CEmailAddrDlgBase::OnCommand(WORD notify, WORD id, HWND hwnd)
{
    HWND hDlg = GetHwnd();
    const int Max_Past_Len = 16 * MAX_PATH;
    wchar_t szAddrStr[Max_Past_Len];

    switch (id)
    {
    case IDC_EDIT_ADDRESSES:
        switch (notify)
        {
        case EN_UPDATE:
            GetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, szAddrStr, Max_Past_Len);
            if (_bClean)
            {
                _addressStr = L"";
            }
            else
            {
                _addressStr = szAddrStr;
            }
            RepositionEditControl();
            //Following 
            if (_addressStr.length() > 0 )
            {
                if (_addressList.size() == 0)
                {
                    EnableWindow(GetDlgItem(hDlg, IDC_OK), true);
                } 
                else
                {

                    EnableWindow(GetDlgItem(hDlg, IDC_OK), IsAllAddressValid());
					EnableWindow(GetDlgItem(hDlg, IDC_REMOVEALLUSERS), true);
				}
            } 
            else
            {//If the string length == 0

                EnableWindow(GetDlgItem(hDlg, IDC_OK), IsAllAddressValid());
				EnableWindow(GetDlgItem(hDlg, IDC_REMOVEALLUSERS), _addressList.size() > 0);
			}

            break;
        case EN_CLIPFORMAT:
            break;
        case EN_CHANGE:
            GetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, szAddrStr, Max_Past_Len);
            if (m_bPasteFlag)
            {
                if (_bClean)
                {
                    int nEnd = lstrlen(szAddrStr) - lstrlen(_enterPrompt);
                    szAddrStr[nEnd] = L'\0';
                }
                ParsingPasteString(szAddrStr);
                SetEditClean(hDlg);
                m_bPasteFlag = false;
            }
            else
            {
                auto a = ES_AUTOHSCROLL;
                if (_bClean && (lstrlen(szAddrStr) >= lstrlen(_enterPrompt) + 1))
                {
                    //_bInit = true;
                    int nEnd = lstrlen(szAddrStr) - lstrlen(_enterPrompt);
                    szAddrStr[nEnd] = L'\0';
                    _bClean = false;
                    SetEditTextColor(RGB(0, 0, 0));
                    SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, szAddrStr);
                    Edit_SetSel(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), -1, -1);

                }
                else
                {
                    if (lstrlen(szAddrStr) == 0)
                    {
                        SetEditClean(hDlg);
                    }
                    else
                    {
                        //LRESULT lMask = SendMessage(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), EM_SETEVENTMASK, 0, 0xffffffff | ENM_CLIPFORMAT);
                        //lMask = lMask;
                    }
                }
            }
            break;
        }
        
        break;
     //case IDC_OK:
     //   OnOk();
        break;

    }
    return CDlgTemplate::OnCommand(notify, id, hwnd);
}

BOOL CEmailAddrDlgBase::OnVScroll(WPARAM wParam, LPARAM lParam) 
{ 
    HWND hDlg = GetHwnd();
    int nDelta;
    int nMaxPos = RectHeight(m_rcOriginalRect) - m_nCurHeight;
    int nSBCode = LOWORD(wParam);
    int nPos = HIWORD(wParam);

    switch (nSBCode)
    {
    case SB_LINEDOWN:
        if (m_nScrollPos >= nMaxPos)
            return FALSE;

        nDelta = min(max(nMaxPos / 20, 5), nMaxPos - m_nScrollPos);
        break;

    case SB_LINEUP:
        if (m_nScrollPos <= 0)
            return FALSE;
        nDelta = -min(max(nMaxPos / 20, 5), m_nScrollPos);
        break;
    case SB_PAGEDOWN:
        if (m_nScrollPos >= nMaxPos)
            return FALSE;
        nDelta = min(max(nMaxPos / 10, 5), nMaxPos - m_nScrollPos);
        break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        nDelta = (int)nPos - m_nScrollPos;
        break;

    case SB_PAGEUP:
        if (m_nScrollPos <= 0)
            return FALSE;
        nDelta = -min(max(nMaxPos / 10, 5), m_nScrollPos);
        break;

    default:
        return FALSE;
    }
    m_nScrollPos += nDelta;
    SetScrollPos(hDlg, SB_VERT, m_nScrollPos, TRUE);
    ScrollWindow(hDlg, 0, -nDelta, NULL, NULL);
    return FALSE; 
}

BOOL CEmailAddrDlgBase::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = GetHwnd();
    int nMaxPos = RectHeight(m_rcOriginalRect) - m_nCurHeight;
    short zDelta = HIWORD(wParam);
    if (zDelta<0)
    {
        if (m_nScrollPos < nMaxPos)
        {
            zDelta = min(max(nMaxPos / 20, 5), nMaxPos - m_nScrollPos);

            m_nScrollPos += zDelta;
            SetScrollPos(hDlg, SB_VERT, m_nScrollPos, TRUE);
            ScrollWindow(hDlg, 0, -zDelta, NULL, NULL);
        }
    }
    else
    {
        if (m_nScrollPos > 0)
        {
            zDelta = -min(max(nMaxPos / 20, 5), m_nScrollPos);

            m_nScrollPos += zDelta;
            SetScrollPos(hDlg, SB_VERT, m_nScrollPos, TRUE);
            ScrollWindow(hDlg, 0, -zDelta, NULL, NULL);
        }
    }

    return FALSE;
}

INT_PTR CEmailAddrDlgBase::OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = GetHwnd();
    switch (uMsg)
    {
    case WM_CTLCOLORSTATIC:
        {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        return (INT_PTR)(HBRUSH)GetStockObject(WHITE_BRUSH);
        }
    //case WM_GETMINMAXINFO:
    //    break;
    }

    return CDlgTemplate::OnRestWMMessage(uMsg, wParam, lParam);
}

BOOL CEmailAddrDlgBase::OnLButtonDown(WPARAM wParam, int x, int y)
{
    HWND hDlg = GetHwnd();
    RECT rcStatic;
    GetWindowRect(GetDlgItem(hDlg, IDC_STATIC_ADDRESSES), &rcStatic);
    MapWindowPoints(GetDesktopWindow(), hDlg, (LPPOINT)&rcStatic, 2);
    POINT pt = { x, y };
    if(PtInRect(&rcStatic, pt))
    {
        SetFocus(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES));
    }
    return CDlgTemplate::OnLButtonDown(wParam, x, y);
}

BOOL CEmailAddrDlgBase::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc;
    HWND hDlg = GetHwnd();
    RECT rect;

    hdc = BeginPaint(hDlg, &ps);
    GetClientRect(hDlg, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

    EndPaint(hDlg, &ps);
    return 0;
}

BOOL CEmailAddrDlgBase::OnSize(WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

SCROLLINFO CEmailAddrDlgBase::SetScrollbarInfo()
{
    HWND hDlg = GetHwnd();
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    si.nMin = 0;
    si.nMax = RectHeight(m_rcOriginalRect);
    si.nPage = m_nCurHeight;
    si.nPos = 0;
    SetScrollInfo(hDlg, SB_VERT, &si, TRUE);

    _bScroll = (si.nMax > (int)si.nPage);
    return si;
}

void CEmailAddrDlgBase::OnDestroy()
{
    DeleteObject(_hBoldFont);
}

BOOL CEmailAddrDlgBase::OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct)
{
    assert(lpDrawItemStruct != 0);
    DRAWITEMSTRUCT *pDraw = lpDrawItemStruct;

    HDC              hDc = pDraw->hDC;
    RECT             rcBtn = pDraw->rcItem;

    switch (pDraw->CtlID)
    {
        case IDC_OK:
        {
            switch (pDraw->itemAction)
            {
            case ODA_DRAWENTIRE:
            {
                if (pDraw->itemState & ODS_DISABLED)
                {
                    HBRUSH nxhBrush = CreateSolidBrush(RGB(204, 204, 204));
                    FillRect(hDc, &rcBtn, nxhBrush);
                    wchar_t szBtnText[100];
                    GetWindowText(pDraw->hwndItem, szBtnText, 100);
                    SetBkMode(hDc, TRANSPARENT);
                    SetTextColor(hDc, RGB(127, 127, 127));
                    DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    DeleteObject(nxhBrush);
                }
                else
                {
                    HBRUSH nxhBrush = CreateSolidBrush(RGB(57, 151, 74));
                    FillRect(hDc, &rcBtn, nxhBrush);
                    wchar_t szBtnText[100];
                    GetWindowText(pDraw->hwndItem, szBtnText, 100);
                    SetBkMode(hDc, TRANSPARENT);
                    SetTextColor(hDc, RGB(255, 255, 255));
                    DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    DeleteObject(nxhBrush);

                }

            }
            break;
            case ODA_FOCUS:
                    {
                        if (pDraw->itemState & ODS_FOCUS)
                        {
                            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 128, 255));
                            HPEN oldPen = (HPEN)SelectObject(hDc, hPen);
                            HBRUSH oldBrush = (HBRUSH)SelectObject(hDc, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
                            Rectangle(hDc, rcBtn.left+1, rcBtn.top+1, rcBtn.right, rcBtn.bottom);
                            SelectObject(hDc, oldPen);
                            InflateRect(&rcBtn, -2, -2);
                            DrawFocusRect(hDc, &rcBtn);
                            SelectObject(hDc, oldBrush);
                            DeleteObject(hPen);
                        }
                        else
                        {
                        }

                    }
                    break;
                case ODA_SELECT:
                    break;
            }
        }
        break;
		case IDC_REMOVEALLUSERS:
		{
			switch (pDraw->itemAction)
			{
			case ODA_DRAWENTIRE:
			{
				if (pDraw->itemState & ODS_DISABLED)
				{
					HBRUSH nxhBrush = CreateSolidBrush(RGB(204, 204, 204));
					FillRect(hDc, &rcBtn, nxhBrush);
					wchar_t szBtnText[100];
					GetWindowText(pDraw->hwndItem, szBtnText, 100);
					SetBkMode(hDc, TRANSPARENT);
					SetTextColor(hDc, RGB(127, 127, 127));
					DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					DeleteObject(nxhBrush);
				}
				else
				{
					HBRUSH nxhBrush = CreateSolidBrush(RGB(240, 240, 240));
					FillRect(hDc, &rcBtn, nxhBrush);
					wchar_t szBtnText[100];
					GetWindowText(pDraw->hwndItem, szBtnText, 100);
					SetBkMode(hDc, TRANSPARENT);
					SetTextColor(hDc, RGB(235, 87, 87));
					DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					DeleteObject(nxhBrush);

				}
				HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
				HPEN oldPen = (HPEN)SelectObject(hDc, hPen);
				HBRUSH oldBrush = (HBRUSH)SelectObject(hDc, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
				Rectangle(hDc, rcBtn.left, rcBtn.top, rcBtn.right, rcBtn.bottom);
				SelectObject(hDc, oldPen);
				SelectObject(hDc, oldBrush);
				DeleteObject(hPen);
			}
			break;
			case ODA_FOCUS:
			{
				if (pDraw->itemState & ODS_FOCUS)
				{
					HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 128, 255));
					HPEN oldPen = (HPEN)SelectObject(hDc, hPen);
					HBRUSH oldBrush = (HBRUSH)SelectObject(hDc, (HBRUSH)GetStockObject(HOLLOW_BRUSH));
					Rectangle(hDc, rcBtn.left + 1, rcBtn.top + 1, rcBtn.right, rcBtn.bottom);
					SelectObject(hDc, oldPen);
					InflateRect(&rcBtn, -2, -2);
					DrawFocusRect(hDc, &rcBtn);
					SelectObject(hDc, oldBrush);
					DeleteObject(hPen);
				}
				else
				{
				}

			}
			break;
			case ODA_SELECT:
				break;
			}
		}
		break;         default:
            break;
    }
    return FALSE;
}

BOOL CEmailAddrDlgBase::OnMeasureItem(MEASUREITEMSTRUCT* lpMeasureItemStruct)
{

    return FALSE;
}

void CEmailAddrDlgBase::OnComboBoxSelChanged()
{
}

void CEmailAddrDlgBase::OnOk()
{
    //HWND hwndCtrl = GetFocus();
    HWND hDlg = GetHwnd();
    //Before We close window We need to figure out data to pass
	if (!_bClean)
	{
		CreateAddressWindow();
	}
	if (GetDataIntoVector())
	{
		CDlgTemplate::OnOk();
	}
}

void CEmailAddrDlgBase::RemoveAddress(CDlgEmailAddr* p)
{
    _addressList.remove(p);
}

