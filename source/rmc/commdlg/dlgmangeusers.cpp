

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
#include "dlgmanageusers.hpp"
#include <string>
#include <richedit.h>
#include "dlgaddress.hpp"
#include "..\uiutilities\uiutilities.hpp"
#include "gdiplus.h"
#include "CGdiPlusBitmap.h"
#include <Shlwapi.h>
#include "commondlg.hpp"

using namespace uiUtilities;
using namespace Gdiplus;
using namespace std;


static ULONG_PTR m_gdiplusToken;



LRESULT CALLBACK EditControlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);



CDlgManageUsers::CDlgManageUsers(const std::wstring & file, const std::vector<std::wstring> &peopleArr) : CEmailAddrDlgBase(IDD_MANAGEUSERS, true)
{
	_file = file;
	for (auto x : peopleArr) {
		_peopleArr.push_back(x.c_str());
	}
}

CDlgManageUsers::~CDlgManageUsers()
{
}



//OnInitialize function is called from WM_INITDIALOG message
//Initialize the dialog state according to given data
BOOL CDlgManageUsers::OnInitialize()
{
    HWND hDlg = GetHwnd();

	CEmailAddrDlgBase::OnInitialize();
    
    CenterWindow(GetParent(hDlg), hDlg);

	SetWindowText(GetDlgItem(hDlg, IDC_STATIC_FILENAME), _file.c_str());
	HFONT hfont = GetWindowFont(GetDlgItem(hDlg, IDC_STATIC_FILENAME));
	LOGFONT lf;
	GetObject(hfont, sizeof(LOGFONT), &lf);
	lf.lfWeight = FW_BOLD;
	lf.lfQuality = CLEARTYPE_QUALITY;
	_hBoldFont = CreateFontIndirect(&lf);
	SetWindowFont(GetDlgItem(hDlg, IDC_STATIC_FILENAME), _hBoldFont, TRUE);
	
	SetEditClean(hDlg);
	for (auto x : _peopleArr)
	{
		CreateAddressWindow(x.c_str());
	}

    return FALSE;
}


bool CDlgManageUsers::GetDataIntoVector()
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
		wstring errmsgFmt = LoadStringResource(0, IDS_REMOVE_INVALID_EMAIL_ADDR)/*L"Please remove wrong email address"*/;
		swprintf_s(msg, MAX_PATH, errmsgFmt.c_str(), addrInEdit);
		wstring errTitle = LoadStringResource(0, IDS_ERROR)/*L"Error"*/;
		MessageBox(hDlg, msg, errTitle.c_str(), MB_OK);
		return false;
	}

	for (auto paddr : _addressList)
	{
		auto currentSize = _resultString.size();
		currentSize = currentSize == 0 > 0 ? 0 : currentSize - 1;
		auto emailAddress = paddr->GetAddress();
		size_t addSize = emailAddress.length() + 2;
		_resultString.resize(currentSize + addSize);
		auto pData = _resultString.data();
		swprintf_s((pData + currentSize), addSize, formatStr, emailAddress.c_str());
	}

	_addressList.clear();
	return true;
}



//NXSharingData &CDlgManageUsers::GetResult()
int CDlgManageUsers::GetResult(const wchar_t ** pResult)
{
	int retVal = (int)_resultString.size();
	*pResult = _resultString.data();
	return retVal;
}

//int CDlgManageUsers::GetJsonResult(const wchar_t ** pResult)
//{
//    int retVal = (int)_jsonString.length();
//    *pResult = _jsonString.c_str();
//    return retVal+1; //include machine 0 here
//}




BOOL CDlgManageUsers::OnNotify(_In_ LPNMHDR lpnmhdr)
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



BOOL CDlgManageUsers::OnCommand(WORD notify, WORD id, HWND hwnd)
{
    HWND hDlg = GetHwnd();

    switch (id)
    {
    case IDC_EDIT_ADDRESSES:
		CEmailAddrDlgBase::OnCommand(notify, id, hwnd);
		break;
    case IDC_OK:
		{
			OnOk();
			break;
		}
	case IDC_REMOVEALLUSERS:
		while (CEmailAddrDlgBase::DeleteLastAddress()) {
			//delete all until it is empty.
		}
		break;
    }
    return CDlgTemplate::OnCommand(notify, id, hwnd);
}


INT_PTR CDlgManageUsers::OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
    }

    return CDlgTemplate::OnRestWMMessage(uMsg, wParam, lParam);
}

BOOL CDlgManageUsers::OnLButtonDown(WPARAM wParam, int x, int y)
{
	return CEmailAddrDlgBase::OnLButtonDown(wParam, x, y);
}

BOOL CDlgManageUsers::OnPaint()
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

BOOL CDlgManageUsers::OnSize(WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}


void CDlgManageUsers::OnDestroy()
{
}

BOOL CDlgManageUsers::OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct)
{
	return CEmailAddrDlgBase::OnOwnerDraw(lpDrawItemStruct);
}


void CDlgManageUsers::OnOk()
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
//bool CDlgManageUsers::GetSharedData()

