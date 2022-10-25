

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
#include "dlgproject.hpp"
#include <string>
#include <richedit.h>
#include "dlgaddress.hpp"
#include "..\uiutilities\uiutilities.hpp"
#include "gdiplus.h"
#include "CGdiPlusBitmap.h"
#include <Shlwapi.h>
#include "commondlg.hpp"
#include <set>

using namespace uiUtilities;
using namespace Gdiplus;
using namespace std;


static ULONG_PTR m_gdiplusToken;



LRESULT CALLBACK EditControlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);



CDlgProject::CDlgProject(NXProjData & projIn, int DlgID, bool bEmptyIsValid) : CEmailAddrDlgBase(DlgID, bEmptyIsValid), _projData(projIn)
{
}

CDlgProject::~CDlgProject()
{
}



//OnInitialize function is called from WM_INITDIALOG message
//Initialize the dialog state according to given data
BOOL CDlgProject::OnInitialize()
{
    HWND hDlg = GetHwnd();

	CEmailAddrDlgBase::OnInitialize();

    lstrcpy(_enterPrompt, LoadStringResource(0, IDS_ADD_MORE_MEMBER).c_str()/*L"Add more member..."*/);
	//wstring promptStr = LoadStringResource(0, IDS_ADD_PEOPLE);
	//wcscpy_s(_enterPrompt, 100, promptStr.c_str());		//Prepare for comparison later

	SetEditClean(hDlg);
    if (_projData.projFlag == NX_PROJ_CREATE)
    {
        
        SendDlgItemMessage(hDlg, IDC_EDIT_PROJNAME, EM_SETLIMITTEXT, 50, 0L);
        SendDlgItemMessage(hDlg, IDC_EDIT_DESC, EM_SETLIMITTEXT, 250, 0L);
		SetFocus(GetDlgItem(hDlg, IDC_EDIT_PROJNAME));
    }

    return FALSE;
}


bool CDlgProject::GetDataIntoVector()
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



//NXSharingData &CDlgProject::GetResult()
int CDlgProject::GetResult(const wchar_t ** pResult)
{
	int retVal = (int)_resultString.size();
	*pResult = _resultString.data();
	return retVal;
}

//int CDlgProject::GetJsonResult(const wchar_t ** pResult)
//{
//    int retVal = (int)_jsonString.length();
//    *pResult = _jsonString.c_str();
//    return retVal+1; //include machine 0 here
//}




BOOL CDlgProject::OnNotify(_In_ LPNMHDR lpnmhdr)
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

bool IsAllBlank(wchar_t szProjDesc[])
{
    for (int i = 0; i < lstrlen(szProjDesc); i++)
    {
        if (szProjDesc[i] != ' ')
            return false;
    }

    return true;
}

bool IsInvalidCharExist(wchar_t szProjName[], wchar_t &invalidChar)
{   
    std::set<wchar_t> validChars = { L'\'',L'"',L'#', L'-', L',',L' ', L'_'};

    for (int i = 0; i < lstrlen(szProjName); i++)
    {
        if (isalnum(szProjName[i]))
        {
            continue;
        }
        auto result = validChars.find(szProjName[i]);
        if (result != validChars.end()) {
            continue;
        } 
        else
        {
            invalidChar = szProjName[i];
            return true;
        }
    }
    return false;
}


BOOL CDlgProject::OnCommand(WORD notify, WORD id, HWND hwnd)
{
    HWND hDlg = GetHwnd();

    switch (id)
    {
    case IDC_EDIT_ADDRESSES:
		CEmailAddrDlgBase::OnCommand(notify, id, hwnd);
		break;
    case IDC_OK:
		{
			wchar_t szProjName[NX_PROJ_NAME_LEN];
			wchar_t szProjDesc[NX_PROJ_DESC_LEN];
			GetDlgItemText(hDlg, IDC_EDIT_PROJNAME, szProjName, NX_PROJ_NAME_LEN);
			GetDlgItemText(hDlg, IDC_EDIT_DESC, szProjDesc, NX_PROJ_DESC_LEN);
            if (_projData.projFlag == NX_PROJ_CREATE)
            {
                if (!lstrlen(szProjName))
                {
                    MessageBox(hDlg, LoadStringResource(0, IDS_ENTER_PROJ_NAME).c_str()/*L"Please enter the project name"*/, 
                        LoadStringResource(0, IDS_PRODUCT_NAME).c_str()/*L"SkyDRM"*/, MB_OK | MB_ICONWARNING);
                    break;
                }
                if (!lstrlen(szProjDesc))
                {
                    MessageBox(hDlg, LoadStringResource(0, IDS_ENTER_PROJ_DESC).c_str()/*L"Please enter the project description"*/, 
                        LoadStringResource(0, IDS_PRODUCT_NAME).c_str()/*L"SkyDRM"*/, MB_OK | MB_ICONWARNING);
                    break;
                }
                if (IsAllBlank(szProjDesc))
                {
                    MessageBox(hDlg, LoadStringResource(0, IDS_DESC_CANNOT_BE_EMPTY).c_str()/*L"The project description field cannot be left empty."*/, 
                        LoadStringResource(0, IDS_PRODUCT_NAME).c_str()/*L"SkyDRM"*/, MB_OK | MB_ICONWARNING);
                    break;
                }
                if (IsAllBlank(szProjName))
                {
                    MessageBox(hDlg, LoadStringResource(0, IDS_PROJ_NAME_CANNOT_BE_EMPTY).c_str()/*L"The project name field cannot be left empty."*/, 
                        LoadStringResource(0, IDS_PRODUCT_NAME).c_str()/*L"SkyDRM"*/, MB_OK | MB_ICONWARNING);
                    break;
                }

                wchar_t invalidChar;
                if(IsInvalidCharExist(szProjName,invalidChar))
                {
                    wstring outMsg = LoadStringResource(0, IDS_CHARACTER_PREFIX)/*L"Character '"*/;
                    outMsg.append(1,invalidChar);
                    outMsg += LoadStringResource(0, IDS_IS_NOT_ALLOWED_IN_PROJ_NAME)/*L"' is not allowed in project name"*/;
                    MessageBox(hDlg, outMsg.c_str(), LoadStringResource(0, IDS_PRODUCT_NAME).c_str()/*L"SkyDRM"*/, MB_OK | MB_ICONWARNING);
                    break;
                }
                lstrcpy(_projData.szProjName, szProjName);
                lstrcpy(_projData.szProjDesc, szProjDesc);
            }

			OnOk();
			break;
		}
    }
    return CDlgTemplate::OnCommand(notify, id, hwnd);
}


INT_PTR CDlgProject::OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgProject::OnLButtonDown(WPARAM wParam, int x, int y)
{
	return CEmailAddrDlgBase::OnLButtonDown(wParam, x, y);
}

BOOL CDlgProject::OnPaint()
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

BOOL CDlgProject::OnSize(WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}


void CDlgProject::OnDestroy()
{
}

BOOL CDlgProject::OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct)
{
	return CEmailAddrDlgBase::OnOwnerDraw(lpDrawItemStruct);
}


void CDlgProject::OnOk()
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
//bool CDlgProject::GetSharedData()

