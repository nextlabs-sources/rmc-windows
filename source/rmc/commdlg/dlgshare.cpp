

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
#include "dlgshare.hpp"
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

CDlgShare::CDlgShare(const std::wstring & file, NXSharingData &sharingData, NXSharingData *pSharingDataOut) : CEmailAddrDlgBase(IDD_DIALOG_SHARE_KN),
    _hIcon(NULL),
    _hFontTitle0(NULL),
    _hFontTitle(NULL),
    _readonly(false),
	m_pSharingDataOut(pSharingDataOut)
{
	m_sharingData = sharingData;
    SetFile(file);
    for (int i = 0; i < TOTAL_PERMISSIONS; i++)
    {   //Clear all values
        _bSharePermissions[i] = false;
    }
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        auto status = Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
        status = status;
    }
    _bAssignRights = sharingData.bAssignedRights;
    _uiDesitedRights = 0;
    _bHasWatermark = sharingData.bWaterMark;
    _dlgType = 0;
	

}

CDlgShare::CDlgShare(const std::wstring& file, const int dlgID, NXSharingData &sharingData) : CEmailAddrDlgBase(dlgID),
_hIcon(NULL),
_hFontTitle0(NULL),
_hFontTitle(NULL),
_readonly(false)
{
    m_sharingData = sharingData;
    SetFile(file);
    for (int i = 0; i < TOTAL_PERMISSIONS; i++)
    {   //Clear all values
        _bSharePermissions[i] = false;
    }
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        auto status = Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
        status = status;
    }
    _bAssignRights = sharingData.bAssignedRights;
    _uiDesitedRights = 0;
    _bHasWatermark = sharingData.bWaterMark;
    _dlgType = 0;

}


CDlgShare::~CDlgShare()
{
    Gdiplus::GdiplusShutdown(m_gdiplusToken);
}



void CDlgShare::ShowAllPermissionButtons()
{   
    HWND hDlg = GetHwnd();

    for (int i=0; i < TOTAL_PERMISSIONS; i++)
    {
        ShowWindow(GetDlgItem(hDlg, _nPermissionControlIds[i]), _bShowPermissions ? SW_SHOW : SW_HIDE);
    }
}


typedef struct {
    int resID;
    int ctrlID;
    std::wstring defStr;
} FillDlgText;



HWND doCreateToolTip(HINSTANCE hInst, HWND hDlg, int iItemID, LPWSTR pszText)
{
    HWND hwndTip;
    HWND hwndTool;

    if (!iItemID || !hDlg || !pszText)
    {
        return FALSE;
    }

    // Get the window of the tool.
    hwndTool = GetDlgItem(hDlg, iItemID);
    RECT rcTool;
    GetWindowRectRelative(hDlg, hwndTool, &rcTool);

    //TODO: Create the tooltip.
    hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
        WS_POPUP | TTS_ALWAYSTIP | TTS_NOANIMATE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        hDlg, NULL,
        hInst, NULL);

    if (!hwndTool || !hwndTip)
    {
        return (HWND)NULL;
    }

    // Associate the tooltip with the tool.
    TOOLINFO toolInfo = { 0 };
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.hwnd = hDlg;
    toolInfo.uFlags = TTF_SUBCLASS;
    toolInfo.uId = 0;
    toolInfo.rect = rcTool;

    toolInfo.lpszText = pszText;
    SendMessageW(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

    return hwndTip;
}

bool IsTextTruncated(HWND hDlg, int CtrlID, wchar_t *ptr)
{
    HWND hwndTool = GetDlgItem(hDlg, CtrlID);
    HFONT hfont = GetWindowFont(hwndTool);
    HDC hdc = GetDC(hDlg);
    auto hOldFont = SelectObject(hdc, hfont);
    SIZE textExt;
    GetTextExtentPoint(hdc, ptr, lstrlen(ptr), &textExt);
    SelectObject(hdc, hOldFont); 
    ReleaseDC(hDlg, hdc);
    RECT rcTool;
    GetWindowRectRelative(hDlg, hwndTool, &rcTool);
    return (textExt.cx > RectWidth(rcTool));
}



#define IDS_SHAREDLG_ADD_PEOPLE  0
#define IDS_SHAREDLG_PROTECT  0
#define IDS_SHAREDLG_PROTECT_U  0
#define IDS_SHAREDLG_PROTECTTITLE  0
#define IDS_SHAREDLG_ADDRESSWRONG  0
#define IDS_SHAREDLG_ERROR  0
#define IDS_SHAREDLG_ADDRESSWRONG  0
#define IDS_SHAREDLG_ERROR  0

//OnInitialize function is called from WM_INITDIALOG message
//Initialize the dialog state according to given data
BOOL CDlgShare::OnInitialize()
{
    HWND hDlg = GetHwnd();
	CEmailAddrDlgBase::OnInitialize();

    SYSTEMTIME st;

    GetSystemTime(&st);

    auto ptr = PathFindFileName(_file.c_str());


    SetDlgItemText(hDlg, IDC_STATIC_DOCUMENT_NAME, ptr);

    Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_VIEW), BST_CHECKED);
    EnableWindow(GetDlgItem(hDlg, IDC_CHECK_VIEW), FALSE);
    Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_WATERMARK), BST_UNCHECKED);
    
    //Initializing the date control with the default value
    _bSharePermissions[_View] = true;
    int defaultExpration = 1;
    st.wYear += defaultExpration;
    _sysTime = st;
  

    //Input focus to be set, ready for user to input
    if (m_sharingData.dlgType == DLGTYPE_SHARING)
    {

        //Set Add people text in edit control
        _bClean = true;
        SetEditTextColor(GetSysColor(COLOR_GRAYTEXT));
        wstring promptStr = LoadStringResource(IDS_SHAREDLG_ADD_PEOPLE, IDS_ADD_PEOPLE);
        wcscpy_s(_enterPrompt, 100, promptStr.c_str());		//Prepare for comparison later
        SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, promptStr.c_str());

        SendDlgItemMessage(hDlg, IDC_EDIT_ADDRESSES, EM_CANPASTE, CF_TEXT, 0L);

    }

    // initial scroll position

    //Check and disalbe the button according to given data
    if (_bAssignRights)
    {
        Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_VIEW), m_sharingData.bView ? BST_CHECKED : BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_EDIT), m_sharingData.bEdit ? BST_CHECKED : BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_PRINT), m_sharingData.bPrint ? BST_CHECKED : BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_SHARE), m_sharingData.bShare ? BST_CHECKED : BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_DOWNLOAD), m_sharingData.bDownload ? BST_CHECKED : BST_UNCHECKED);
        if (m_sharingData.dlgType == DLGTYPE_SHARING)
        {
            EnableWindow(GetDlgItem(hDlg, IDC_CHECK_EDIT), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_CHECK_PRINT), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_CHECK_SHARE), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_CHECK_DOWNLOAD), FALSE);
        }
    }

    //Check and disable the button according to given data
    if (_bAssignRights)
    {
        Button_SetCheck(GetDlgItem(hDlg, IDC_CHECK_WATERMARK), _bHasWatermark ? BST_CHECKED : BST_UNCHECKED);
        if (m_sharingData.dlgType == DLGTYPE_SHARING)
        {
            EnableWindow(GetDlgItem(hDlg, IDC_CHECK_WATERMARK), FALSE);
        }
    }


//     HFONT hfont = GetWindowFont(GetDlgItem(hDlg, IDC_STATIC_CONTENT));
//     LOGFONT lf;
//     GetObject(hfont, sizeof(LOGFONT), &lf);
//     lf.lfWeight = FW_BOLD;
//     lf.lfQuality = CLEARTYPE_QUALITY;
//     _hBoldFont = CreateFontIndirect(&lf);
//     SetWindowFont(GetDlgItem(hDlg, IDC_STATIC_CONTENT), _hBoldFont, TRUE);
//     SetWindowFont(GetDlgItem(hDlg, IDC_STATIC_COLLABORATION), _hBoldFont, TRUE);
//     SetWindowFont(GetDlgItem(hDlg, IDC_STATIC_EFFECT), _hBoldFont, TRUE);
// 
//     if(IsTextTruncated(hDlg, IDC_STATIC_DOCUMENT_NAME, ptr))
//     {
//         doCreateToolTip(_hInstance, hDlg, IDC_STATIC_DOCUMENT_NAME, ptr);
//     }

    if (m_sharingData.dlgType == DLGTYPE_PROTECT || m_sharingData.dlgType == DLGTYPE_PROTECT_PROJ)
    {//Protect type here
        RECT rcDlg;
        RECT rcAnd;
        ::GetWindowRect(hDlg, &rcDlg);
        GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_OK), &rcAnd);
        int ncyTitle = GetSystemMetrics(SM_CYCAPTION);
//        MoveWindow(hDlg, rcDlg.left, rcDlg.top, RectWidth(rcDlg), rcAnd.top + ncyTitle, TRUE);
        wstring protectStr = LoadStringResource(IDS_SHAREDLG_PROTECT, IDS_PROTECT);
        SetDlgItemText(hDlg, IDC_OK, protectStr.c_str());
        protectStr = LoadStringResource(IDS_SHAREDLG_PROTECTTITLE, IDS_PROTECT);
		protectStr += L" - ";
		protectStr += ptr;
		SetWindowText(hDlg, protectStr.c_str());
        ShowWindow(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES),SW_HIDE);//Disable the share when protect
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC_ADDRESSES), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC_SHAREWITH), SW_HIDE);
		SetFocus(GetDlgItem(hDlg, IDC_CHECK_SHARE));
        if (m_sharingData.dlgType == DLGTYPE_PROTECT_PROJ)
        {
            EnableWindow(GetDlgItem(hDlg, IDC_CHECK_SHARE), FALSE);
        }

    } 
    else
    {
        //AT this time following statement is always disable the share button
        EnableWindow(GetDlgItem(hDlg, IDC_OK), IsAllAddressValid());
		wchar_t szWinTitle[100];
		GetWindowText(hDlg, szWinTitle, 99);
		wstring protectStr = szWinTitle;
		protectStr += L" - ";
		protectStr += ptr;
		SetWindowText(hDlg, protectStr.c_str());
    }


    CenterWindow(GetParent(hDlg), hDlg);
    if (_dlgId == IDD_DIALOG_FILE_PROPERTIES )
    {
		SendDlgItemMessage(hDlg, IDC_FILE_ICON, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)m_sharingData.hFileIcon);
		SetDlgItemText(hDlg, IDC_STATIC_DATETIME, m_sharingData.dtString.c_str());
		SetDlgItemText(hDlg, IDC_STATIC_FILESIZE, m_sharingData.sizeString.c_str());

		ShowWindow(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), SW_HIDE);
		for (auto x : m_sharingData.peopleArr)
		{
			CreateAddressWindow(x.c_str());
		}
    }
	else if(_dlgId == IDD_DIALOG_SHARE_KN)
	{
		if (m_sharingData.dlgType == DLGTYPE_SHARING)
		{
			for (auto x : m_sharingData.peopleArr)
			{
				CreateAddressWindow(x.c_str());
			}
		}
	}
	if (_dlgId != IDD_DIALOG_FILE_PROPERTIES && m_pSharingDataOut)
	{
		m_pSharingDataOut->hWnd = hDlg;
	}
    return FALSE;
}


bool CDlgShare::GetDataIntoVector()
{
	HWND hDlg = GetHwnd();
	_resultString.clear();
	WCHAR formatStr[] = L"%s;";
	WCHAR addressTagName[] = L"email_address";

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



//NXSharingData &CDlgShare::GetResult()
int CDlgShare::GetResult(const wchar_t ** pResult)
{
	int retVal = (int)_resultString.size();
	*pResult = _resultString.data();
	return retVal;
}

int CDlgShare::GetJsonResult(const wchar_t ** pResult)
{
    int retVal = (int)_jsonString.length();
    *pResult = _jsonString.c_str();
    return retVal+1; //include machine 0 here
}




BOOL CDlgShare::OnNotify(_In_ LPNMHDR lpnmhdr)
{
    HWND hDlg = GetHwnd();
 
    if (lpnmhdr->idFrom == IDC_EDIT_ADDRESSES)
    {
        if (lpnmhdr->code == EN_SELCHANGE)
        {

        }
        //wchar_t str[256];
        //wsprintf(str, L"lpnmhdr->code: %x\r\n", lpnmhdr->code);
        //TRACE(str);
    }
    if (lpnmhdr->code == DTN_DATETIMECHANGE)
    {
        DateTime_GetSystemtime(GetDlgItem(hDlg, IDC_DATETIMEPICKER_EXPIRARION), &_sysTime);
        ShowExpirationDateText();
    }

    return CDlgTemplate::OnNotify(lpnmhdr);
}
void CDlgShare::ShowExpirationDateText()
{
    HWND hDlg = GetHwnd();
    std::wstring dateString = L"";
    {//Show days from access
        wchar_t szExpireDate[100];
        GetDateFormat(LOCALE_NAME_USER_DEFAULT, DATE_AUTOLAYOUT, &_sysTime, NULL, szExpireDate, 100);
        dateString = szExpireDate;
    }
    SetDlgItemText(hDlg, IDC_STATIC_EXPIRATION, dateString.c_str());
}

//void CDlgShare::RepositionEditControl()
//{
//    HWND hDlg = GetHwnd();
//    SIZE sz;
//    RECT rcEdit;
//    auto nPos = GetScrollPos(hDlg, SB_VERT);
//    if (_addressList.size())
//    {
//        auto paddr = _addressList.back();
//        auto hEdit = GetDlgItem(hDlg, IDC_EDIT_ADDRESSES);
//        auto hdc = GetDC(hDlg);
//        HFONT hfont = GetWindowFont(hDlg);
//        HFONT hOldFont = (HFONT)SelectObject(hdc, hfont);
//        RECT rcStatic;
//        GetWindowRectRelative(hDlg, GetDlgItem(hDlg, IDC_STATIC_ADDRESSES), &rcStatic);
//        GetTextExtentPoint(hdc, _addressStr.c_str(), (int)_addressStr.length(), &sz);
//        SelectObject(hdc, hOldFont);
//        GetWindowRectRelative(hDlg, hEdit, &rcEdit);
//        ScreenToScroll(rcEdit, nPos);
//        if (sz.cx >= RectWidth(rcEdit) && rcEdit.left > _ptInitEditPos.x)
//        {
//            rcEdit.left = _ptInitEditPos.x;
//            int offsetY = _addressHeight;
//            ResizeWindowToFit(_addressHeight);
//            {
//                OffsetRect(&rcEdit, 0, offsetY);
//                ScrollToScreen(rcEdit, m_nScrollPos);
//                auto currentY = rcEdit.top;
//                MoveWindow(hEdit, rcEdit.left, currentY, RectWidth(rcEdit), RectHeight(rcEdit), TRUE);
//                _currentY = ScreenToScroll(currentY, nPos);
//            }
//        }
//        else if (sz.cx >= RectWidth(rcEdit) && rcEdit.left == _ptInitEditPos.x)
//        {//We do not change anything here, the whole line is one address
//
//        }
//        else
//        {
//            assert(paddr);
//            RECT lastLocation;
//            paddr->WindowScreenLocation(&lastLocation, nPos);
//            if (lastLocation.right + sz.cx + _minimalAddrWidth < rcStatic.right && ScrollToScreen(_currentY,nPos) > lastLocation.bottom)
//            {//Here we move the edit window back
//                int width = rcStatic.right - lastLocation.right -ADDRESS_MARGIN;
//                int height = RectHeight(rcEdit);
//                int currentY = lastLocation.top + (_addressHeight - _initEditSize.cy) / 2;
//                MoveWindow(hEdit, lastLocation.right, currentY, width, height, TRUE);
//                _currentY = ScreenToScroll(currentY, nPos);
//                ResizeWindowToFit(-_addressHeight);
//            }
//        }
//        ReleaseDC(hEdit, hdc);
//    }
//    
//}

//void CDlgShare::SetEditTextColor(COLORREF rgbText)
//{
//    CHARFORMAT2 cf;
//    HWND hDlg = GetHwnd();
//    cf.cbSize = sizeof(CHARFORMAT2);
//    cf.dwMask = CFM_COLOR;
//    auto ret = ::SendMessage(::GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
//    cf.dwMask = CFM_COLOR;
//    cf.dwEffects = 0;
//    cf.crTextColor = rgbText;
//    ret = ::SendMessage(::GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
//
//}

//void CDlgShare::SetEditClean(HWND hDlg)
//{
//    SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, _enterPrompt);
//    SetEditTextColor(RGB(128, 128, 128));
//    _bClean = true;
//}
//
//int CDlgShare::ParsingPasteString(wchar_t *szAddrStr)
//{
//    auto delemeter = L",;\n\r ";
//    wchar_t *ptr;
//    auto pTok = wcstok_s(szAddrStr, delemeter,&ptr);
//    do
//    {
//        CreateAddressWindow(pTok);
//    } while ((pTok = wcstok_s(NULL, delemeter, &ptr)) != NULL);
//    return 0;
//}


BOOL CDlgShare::OnCommand(WORD notify, WORD id, HWND hwnd)
{
    HWND hDlg = GetHwnd();
    //const int Max_Past_Len = 16 * MAX_PATH;
    //wchar_t szAddrStr[Max_Past_Len];

    switch (id)
    {
    case IDC_EDIT_ADDRESSES:
		CEmailAddrDlgBase::OnCommand(notify, id, hwnd);
        //switch (notify)
        //{
        //case EN_UPDATE:
        //    GetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, szAddrStr, Max_Past_Len);
        //    if (_bClean)
        //    {
        //        _addressStr = L"";
        //    }
        //    else
        //    {
        //        _addressStr = szAddrStr;
        //    }
        //    RepositionEditControl();
        //    //Following 
        //    if (_addressStr.length() > 0 )
        //    {
        //        if (_addressList.size() == 0)
        //        {
        //            EnableWindow(GetDlgItem(hDlg, IDC_OK), true);
        //        } 
        //        else
        //        {

        //            EnableWindow(GetDlgItem(hDlg, IDC_OK), IsAllAddressValid());
        //        }
        //    } 
        //    else
        //    {//If the string length == 0

        //        EnableWindow(GetDlgItem(hDlg, IDC_OK), IsAllAddressValid());
        //    }

        //    break;
        //case EN_CLIPFORMAT:
        //    //GetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, szAddrStr, Max_Past_Len);
        //    //MessageBox(hDlg, szAddrStr, L"paste", MB_OK);
        //    break;
        //case EN_CHANGE:
        //    GetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, szAddrStr, Max_Past_Len);
        //    if (m_bPasteFlag)
        //    {
        //        if (_bClean)
        //        {
        //            int nEnd = lstrlen(szAddrStr) - lstrlen(_enterPrompt);
        //            szAddrStr[nEnd] = L'\0';
        //        }
        //        ParsingPasteString(szAddrStr);
        //        SetEditClean(hDlg);
        //        m_bPasteFlag = false;
        //    }
        //    else
        //    {
        //        auto a = ES_AUTOHSCROLL;
        //        if (_bClean && (lstrlen(szAddrStr) >= lstrlen(_enterPrompt) + 1))
        //        {
        //            //_bInit = true;
        //            int nEnd = lstrlen(szAddrStr) - lstrlen(_enterPrompt);
        //            szAddrStr[nEnd] = L'\0';
        //            _bClean = false;
        //            SetEditTextColor(RGB(0, 0, 0));
        //            SetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, szAddrStr);
        //            Edit_SetSel(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), -1, -1);

        //        }
        //        else
        //        {
        //            if (lstrlen(szAddrStr) == 0)
        //            {
        //                SetEditClean(hDlg);
        //            }
        //            else
        //            {
        //                //LRESULT lMask = SendMessage(GetDlgItem(hDlg, IDC_EDIT_ADDRESSES), EM_SETEVENTMASK, 0, 0xffffffff | ENM_CLIPFORMAT);
        //                //lMask = lMask;
        //            }
        //        }
        //    }
        //    break;
        //}
        //
        //break;
    case IDC_CHECK_VIEW:
        _bSharePermissions[_View] = Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_VIEW)) == BST_CHECKED;
        break;
    case IDC_CHECK_EDIT:
        _bSharePermissions[_Edit] = Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_EDIT)) == BST_CHECKED;
    case IDC_CHECK_PRINT:
        _bSharePermissions[_Print] = Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_PRINT)) == BST_CHECKED;
        break;
    case IDC_OK:
        OnOk();
        break;
    //case IDC_EXPIRATION:
    //    _bShowExpiration = !_bShowExpiration;
    //    CollapseExpiration();
    //    break;

    }
    return CDlgTemplate::OnCommand(notify, id, hwnd);
}

//BOOL CDlgShare::OnVScroll(WPARAM wParam, LPARAM lParam) 
//{ 
//    HWND hDlg = GetHwnd();
//    int nDelta;
//    int nMaxPos = RectHeight(m_rcOriginalRect) - m_nCurHeight;
//    int nSBCode = LOWORD(wParam);
//    int nPos = HIWORD(wParam);
//
//    switch (nSBCode)
//    {
//    case SB_LINEDOWN:
//        if (m_nScrollPos >= nMaxPos)
//            return FALSE;
//
//        nDelta = min(max(nMaxPos / 20, 5), nMaxPos - m_nScrollPos);
//        break;
//
//    case SB_LINEUP:
//        if (m_nScrollPos <= 0)
//            return FALSE;
//        nDelta = -min(max(nMaxPos / 20, 5), m_nScrollPos);
//        break;
//    case SB_PAGEDOWN:
//        if (m_nScrollPos >= nMaxPos)
//            return FALSE;
//        nDelta = min(max(nMaxPos / 10, 5), nMaxPos - m_nScrollPos);
//        break;
//    case SB_THUMBTRACK:
//    case SB_THUMBPOSITION:
//        nDelta = (int)nPos - m_nScrollPos;
//        break;
//
//    case SB_PAGEUP:
//        if (m_nScrollPos <= 0)
//            return FALSE;
//        nDelta = -min(max(nMaxPos / 10, 5), m_nScrollPos);
//        break;
//
//    default:
//        return FALSE;
//    }
//    m_nScrollPos += nDelta;
//    SetScrollPos(hDlg, SB_VERT, m_nScrollPos, TRUE);
//    ScrollWindow(hDlg, 0, -nDelta, NULL, NULL);
//    return FALSE; 
//}

//BOOL CDlgShare::OnMouseWheel(WPARAM wParam, LPARAM lParam)
//{
//    HWND hDlg = GetHwnd();
//    int nMaxPos = RectHeight(m_rcOriginalRect) - m_nCurHeight;
//    short zDelta = HIWORD(wParam);
//    if (zDelta<0)
//    {
//        if (m_nScrollPos < nMaxPos)
//        {
//            zDelta = min(max(nMaxPos / 20, 5), nMaxPos - m_nScrollPos);
//
//            m_nScrollPos += zDelta;
//            SetScrollPos(hDlg, SB_VERT, m_nScrollPos, TRUE);
//            ScrollWindow(hDlg, 0, -zDelta, NULL, NULL);
//        }
//    }
//    else
//    {
//        if (m_nScrollPos > 0)
//        {
//            zDelta = -min(max(nMaxPos / 20, 5), m_nScrollPos);
//
//            m_nScrollPos += zDelta;
//            SetScrollPos(hDlg, SB_VERT, m_nScrollPos, TRUE);
//            ScrollWindow(hDlg, 0, -zDelta, NULL, NULL);
//        }
//    }
//
//    return FALSE;
//}

INT_PTR CDlgShare::OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

BOOL CDlgShare::OnLButtonDown(WPARAM wParam, int x, int y)
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

BOOL CDlgShare::OnPaint()
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

BOOL CDlgShare::OnSize(WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

//SCROLLINFO CDlgShare::SetScrollbarInfo()
//{
//    HWND hDlg = GetHwnd();
//    SCROLLINFO si;
//    si.cbSize = sizeof(SCROLLINFO);
//    si.fMask = SIF_ALL;
//    si.nMin = 0;
//    si.nMax = RectHeight(m_rcOriginalRect);
//    si.nPage = m_nCurHeight;
//    si.nPos = 0;
//    SetScrollInfo(hDlg, SB_VERT, &si, TRUE);
//
//    _bScroll = (si.nMax > (int)si.nPage);
//    return si;
//}

void CDlgShare::OnDestroy()
{
    DeleteObject(_hBoldFont);
}

BOOL CDlgShare::OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct)
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
        case IDC_EXPIRATION:
            switch (pDraw->itemAction)
            {
            case ODA_DRAWENTIRE:
            {
                {   //background is white
                    HBRUSH nxhBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
                    FillRect(hDc, &rcBtn, nxhBrush);


                    CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
                    bool bBmpLoaded = false;

                    if (_bShowExpiration )
                    {
                        bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_COLLAPSE), L"PNG", _hInstance);
                    }
                    else {
                        bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_COLLAPSE_OPEN), L"PNG", _hInstance);
                    }
                    if (bBmpLoaded)
                    {
                        Gdiplus::Graphics graphics(hDc);
                        Rect rect(rcBtn.left, rcBtn.top, RectWidth(rcBtn), RectHeight(rcBtn));
                        graphics.SetSmoothingMode(SmoothingModeHighQuality);
                        int iconHeight = RectHeight(rcBtn);
                        int iconWidth = iconHeight;
                        Rect rc(rcBtn.left, rcBtn.top, iconWidth, iconWidth);
                        rc.Inflate(Point(-2, -2));
                        graphics.DrawImage(*pBitmap, (const Rect&)rc);
                    }
                    delete(pBitmap);

                    wchar_t szBtnText[100];
                    GetWindowText(pDraw->hwndItem, szBtnText, 100);
                    SetBkMode(hDc, TRANSPARENT);

                    rcBtn.left += RectHeight(rcBtn) * 5 / 4;
                    DrawText(hDc, szBtnText, lstrlen(szBtnText), &rcBtn, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

                }

            }
            break;
            case ODA_FOCUS:
            {
                if (pDraw->itemState & ODS_FOCUS)
                {
                    InflateRect(&rcBtn, -3, -3);
                    DrawFocusRect(hDc, &rcBtn);
                    //TRACE(L"Focus\n");
                }
                else
                {
                    //TRACE(L"No Focus\n");
                }

            }
            break;
            case ODA_SELECT:
                break;
            }
            break;
        default:
            break;
    }
    return FALSE;
}

//BOOL CDlgShare::OnMeasureItem(MEASUREITEMSTRUCT* lpMeasureItemStruct)
//{
//
//    return FALSE;
//}

void CDlgShare::OnComboBoxSelChanged()
{
}

void CDlgShare::OnOk()
{
    //HWND hwndCtrl = GetFocus();
    HWND hDlg = GetHwnd();
    //Before We close window We need to figure out data to pass
    if (!_bClean)
    {
        CreateAddressWindow();
    }
	bool bCanFinish = GetSharedData();
	if (bCanFinish)
	{
		CDlgTemplate::OnOk();
	}
}
bool CDlgShare::GetSharedData()
{
    HWND hDlg = GetHwnd();
    _resultString.clear(); 
    WCHAR expireDateTagName[] = L"expire_date";
    WCHAR expireDaysTagName[] = L"expire_days";
    WCHAR expireMonthsTagName[] = L"expire_months";
    WCHAR expireYearsTagName[] = L"expire_years";
    WCHAR formatStr[] = L"%s=%s";
    WCHAR addressTagName[] = L"email_address";
    WCHAR addrInEdit[MAX_PATH];
    wchar_t msg[MAX_PATH];
    bool bFoundEmail = false;
    //Here we get addresses into the vector
    GetDlgItemText(hDlg, IDC_EDIT_ADDRESSES, addrInEdit, MAX_PATH);

	if (m_sharingData.dlgType == DLGTYPE_SHARING)
	{
		if (!IsAllAddressValid())
		{
			swprintf_s(msg, MAX_PATH, LoadStringResource(0, IDS_REMOVE_INVALID_EMAIL_ADDR).c_str()/*L"Please remove invalide email address"*/);
			wstring errTitle = LoadStringResource(IDS_SHAREDLG_ERROR, IDS_ERROR);
			MessageBox(hDlg, msg, errTitle.c_str(), MB_OK);
			return false;
		}
		GetDataIntoVector();
	}
  
    {
        //WCHAR permissionTagName[] = L"watermark";

        //auto currentSize = _resultString.size();
		m_pSharingDataOut->bView = (Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_VIEW)) == BST_CHECKED);
		m_pSharingDataOut->bEdit = (Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_EDIT)) == BST_CHECKED);
		m_pSharingDataOut->bPrint = (Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_PRINT)) == BST_CHECKED);
		m_pSharingDataOut->bShare = (Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_SHARE)) == BST_CHECKED);
		m_pSharingDataOut->bDownload = (Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_DOWNLOAD)) == BST_CHECKED);

		m_pSharingDataOut->bWaterMark = (Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_WATERMARK)) == BST_CHECKED);
    }
    return true;
}


//void CDlgShare::RemoveAddress(CDlgEmailAddr* p)
//{
//    _addressList.remove(p);
//}

