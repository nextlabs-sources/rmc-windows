#include "stdafx.h"
//#include <Windows.h>
#include <assert.h>
#include "nxrmRMC.h"
#include "resource.h"
//#include "EngineUtils.h"
//#include "servctrl.hpp"
#include "nx\common\rapidjson.h"
#include "nxrmc.h"
//#include <nudf/winutil.hpp>


#define RMS_PAGE_WIDTH      395
#define RMS_PAGE_HEIGHT     660
#define MAX_BROWSER_HEIGHT  750
void InitScrollBars(HWND hWnd, CRect &rc);

int CMainDlg::_closeID = IDOK;
LogonType CMainDlg::_logonType = NX_LOGON_SKYDRM;

CMainDlg::CMainDlg(HINSTANCE hInst, HWND hParent, UINT uDlgId, bool bIsWin10 /*false*/)
    : _hInst(hInst), _hParent(hParent), _hWnd(NULL), _uDlgId(uDlgId), _init_dlg_cx(0), _init_dlg_cy(0), _init_button_cx(0), _init_button_cy(0), _init_browser_cx(RMS_PAGE_WIDTH), _init_browser_cy(RMS_PAGE_HEIGHT), _hb_white(NULL), _hp_blue(NULL), _pCtrl(NULL)
{
    _bIsWin10 = bIsWin10;
}

CMainDlg::~CMainDlg()
{
    if (_pCtrl) {
        _pCtrl->DeActivate();
        delete _pCtrl;
        _pCtrl = NULL;
    }
    if (NULL != _hb_white) {
        ::DeleteObject(_hb_white);
        _hb_white = NULL;
    }
    if (NULL != _hp_blue) {
        ::DeleteObject(_hp_blue);
        _hp_blue = NULL;
    }
   
}

VOID CMainDlg::CenterWindow(HWND hWnd)
{
	auto plogon = CWnd::FromHandle(_hWnd);
	plogon->CenterWindow(CWnd::FromHandle(hWnd));
}
INT_PTR CMainDlg::LogOn(const std::wstring& authn_url, const std::wstring& succeed_url, const std::wstring &id)
{
    _authn_url = authn_url;    
    _logonType = NX_LOGON_SKYDRM;
	_clientid = id;
	HWND hwnd = CreateDialogParamW(_hInst, MAKEINTRESOURCEW(_uDlgId), _hParent, CMainDlg::InterDlgProc, (LPARAM)this);
    ShowWindow(hwnd, SW_SHOW);
    return hwnd != 0 ? 1 : 0;
}

INT_PTR CMainDlg::LogOnToProvider(const std::wstring & authn_url, const std::wstring & succeed_url, const std::wstring &cid, const std::wstring & uid, const std::wstring ticket)
{
	_authn_url = authn_url;
	_logonType = NX_LOGON_OAUTH;
	_success_url = succeed_url;
	_clientid = cid;
	_userid = uid;
	_ticket = ticket;

	return ::DialogBoxParamW(_hInst, MAKEINTRESOURCEW(_uDlgId), _hParent, CMainDlg::InterDlgProc, (LPARAM)this);
}

void CMainDlg::DestroyDialog(int closeID)
{
	_closeID = closeID;
	PostMessage(_hParent, WM_NX_LOGON_END, 0, 0);
	DestroyWindow(_hWnd);
}



void CMainDlg::StopBrowsing()
{
    _pCtrl->SetQuitFlag();
    if(!_bIsWin10)//for windows 7 & 8 call deactivate first.
        _pCtrl->DeActivate();
}

void CMainDlg::InitDlgSize()
{
	CRect rc;
	GetClientRect(_hParent, &rc);
    const int screen_cx = (_logonType == NX_LOGON_SKYDRM ? rc.Width() : GetSystemMetrics(SM_CXMAXIMIZED));
    const int screen_cy = (_logonType == NX_LOGON_SKYDRM ? rc.Height() : GetSystemMetrics(SM_CYMAXIMIZED));
    RECT rect = { 0, 0, 0, 0 };

    // Get dialog window size
    GetWindowRect(_hWnd, &rect);
    const int dlg_width = rect.right - rect.left;
    const int dlg_height = rect.bottom - rect.top;
    // Get dialog client size
    memset(&rect, 0, sizeof(rect));
    GetClientRect(_hWnd, &rect);
    const int dlg_client_width = rect.right - rect.left;
    const int dlg_client_height = rect.bottom - rect.top;
    // calculate the margin
    const int dlg_margin_width = dlg_width - dlg_client_width;
    const int dlg_margin_height = dlg_height - dlg_client_height;
    // Get button size
    memset(&rect, 0, sizeof(rect));
    GetWindowRect(GetDlgItem(_hWnd, IDC_BUTTON_WEBLOGON), &rect);
    *((int*)(&_init_button_cx)) = rect.right - rect.left;
    *((int*)(&_init_button_cy)) = rect.bottom - rect.top;

    // calculate new dialog size
    const int new_dlg_width = _init_browser_cx + dlg_margin_width;
    const int new_dlg_height = 5 + _init_browser_cy + 5 + _init_button_cy + 5 + dlg_margin_height;
    const int new_dlg_x = (screen_cx > new_dlg_width) ? ((screen_cx - new_dlg_width) / 2) : 0;
    const int new_dlg_y = (screen_cy > new_dlg_height) ? ((screen_cy - new_dlg_height) / 2) : 0;
    *((int*)(&_init_dlg_cx)) = new_dlg_width;
    *((int*)(&_init_dlg_cy)) = new_dlg_height;

    MoveWindow(GetDlgItem(_hWnd, IDC_BUTTON_WEBLOGON), 10, 5 + _init_browser_cy + 5, _init_button_cx, _init_button_cy, TRUE);
    MoveWindow(GetDlgItem(_hWnd, IDC_BROWSER_FRAME), 0, 5, _init_browser_cx, _init_browser_cy, TRUE);
    SetWindowPos(_hWnd, HWND_TOP, new_dlg_x, new_dlg_y, new_dlg_width, new_dlg_height, 0);
    CRect rcScroll(new_dlg_x, new_dlg_y, new_dlg_x + new_dlg_width, new_dlg_y + new_dlg_height);
    InitScrollBars(theApp.m_mainhWnd, rcScroll);

}

BOOL CMainDlg::OnInitDialog(HWND hDlg)
{
    CoInitialize(NULL);
	_closeID = IDOK;
    wchar_t szTitle[MAX_PATH];
    GetWindowText(hDlg, szTitle, MAX_PATH);
    g_APPUI.szLogonTitle = szTitle,

    _hWnd = hDlg;
    _color_blue = RGB(6, 69, 173);
    _hb_white = ::CreateSolidBrush(RGB(255, 255, 255));
    _hp_blue = ::CreatePen(PS_SOLID, 1, _color_blue);
    ShowWindow(GetDlgItem(_hWnd, IDC_BUTTON_WEBLOGON), SW_HIDE);

    _back_button.Attach(GetDlgItem(_hWnd, IDC_BUTTON_WEBLOGON));

    HWND hCtrl = GetDlgItem(hDlg, IDC_BROWSER_FRAME);
    assert(NULL != hCtrl);
    if(_logonType == NX_LOGON_SKYDRM)
    {
        _pCtrl = new CLogonControl(hCtrl);
        if (NULL == _pCtrl)
        {
            return FALSE;
        }
    } 
    else
    {
        _pCtrl = new CLogonControl(hCtrl, _success_url.c_str());
        if (NULL == _pCtrl)
        {
            return FALSE;
        }

    }

    InitDlgSize();

    if (_pCtrl) {
		if (_logonType == NX_LOGON_SKYDRM) {
			_pCtrl->SetURL(_authn_url.c_str(), _clientid.c_str());
		}
		else {
			_pCtrl->SetURL(_authn_url.c_str(), _clientid.c_str(), _userid.c_str(), _ticket.c_str());
		}
    }

    return TRUE;
}
void CMainDlg::OnDestroy()
{
    if (_pCtrl) {
        _pCtrl->DeActivate();
        delete _pCtrl;
        _pCtrl = NULL;
    }

    CoFreeUnusedLibraries();
    SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
    CoUninitialize();

    if (_logonType == NX_LOGON_SKYDRM)
    {
        theApp.m_plogonDlg = NULL;
        delete this;
    }
}

void CMainDlg::OnDrawItem(UINT uCtrlId, DRAWITEMSTRUCT* pDis)
{
    switch (uCtrlId)
    {
    case IDC_BUTTON_WEBLOGON:
        OnDrawBackButton(pDis);
        break;
    default:
        ::DefWindowProc(_hWnd, WM_DRAWITEM, (WPARAM)uCtrlId, (LPARAM)pDis);
    }
}

void CMainDlg::OnEraseBkgnd(HDC hDC)
{
    RECT rc = { 0, 0, 0, 0 };
    GetClientRect(_hWnd, &rc);
    FillRect(hDC, &rc, _hb_white);
}

void CMainDlg::OnDrawBackButton(DRAWITEMSTRUCT* pDis)
{
    RECT rcText = { 0, 0, 0, 0 };
    SetBkMode(pDis->hDC, TRANSPARENT);
    SetTextColor(pDis->hDC, _color_blue);
    SetBkColor(pDis->hDC, RGB(255, 255, 255));
    FillRect(pDis->hDC, &pDis->rcItem, _hb_white);
    DrawTextW(pDis->hDC, theApp.LoadString(IDS_SIGN_IN_DIFF_ACCOUNT)/*L"Sign in with a different account"*/, -1, &rcText, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_CALCRECT);
    DrawTextW(pDis->hDC, theApp.LoadString(IDS_SIGN_IN_DIFF_ACCOUNT)/*L"Sign in with a different account"*/, -1, &pDis->rcItem, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

    const int width = rcText.right - rcText.left;
    HPEN hOldPen = (HPEN)::SelectObject(pDis->hDC, _hp_blue);
    MoveToEx(pDis->hDC, pDis->rcItem.left, pDis->rcItem.bottom - 3, NULL);
    LineTo(pDis->hDC, pDis->rcItem.left + width, pDis->rcItem.bottom - 3);
    ::SelectObject(pDis->hDC, hOldPen);
}

void CMainDlg::OnClickWebLogon()
{
    _pCtrl->Goto(_authn_url.c_str());
}

void CMainDlg::OnAdjustPageSize(int cx, int cy)
{
    RECT rect = { 0, 0, 0, 0 };

    return;

    // Don't change width now
    UNREFERENCED_PARAMETER(cx);

    // Get screen size
	CRect rc;
	GetClientRect(_hParent, &rc);
    const int screen_cx = (_logonType == NX_LOGON_SKYDRM ? rc.Width() : GetSystemMetrics(SM_CXMAXIMIZED));
    const int screen_cy = (_logonType == NX_LOGON_SKYDRM ? rc.Height() : GetSystemMetrics(SM_CYMAXIMIZED));


    // Avoid exceeding maximum browser size
    if (cy > MAX_BROWSER_HEIGHT) {
        cy = MAX_BROWSER_HEIGHT;
    }

    // Calculate browser size
    GetWindowRect(GetDlgItem(_hWnd, IDC_BROWSER_FRAME), &rect);
    const int browser_height = rect.bottom - rect.top;
    const int browser_width = rect.right - rect.left;

    // No height change
    if (cy < get_init_browser_cy()) {
        cy = get_init_browser_cy();
    }
    if (browser_height == cy) {
        return;
    }

    // Calculate diff
    const int diff_cy = cy - browser_height;
    
    // Get dlg size
    memset(&rect, 0, sizeof(rect));
    GetWindowRect(_hWnd, &rect);
    const int dlg_width = rect.right - rect.left;
    const int dlg_height = rect.bottom - rect.top + diff_cy;
    const int dlg_x = (screen_cx > dlg_width) ? ((screen_cx - dlg_width) / 2) : 0;
    const int dlg_y = (screen_cy > dlg_height) ? ((screen_cy - dlg_height) / 2) : 0;
    MoveWindow(GetDlgItem(_hWnd, IDC_BUTTON_WEBLOGON), 10, 5 + cy + 5, _init_button_cx, _init_button_cy, TRUE);
    MoveWindow(GetDlgItem(_hWnd, IDC_BROWSER_FRAME), 0, 5, _init_browser_cx, cy, TRUE);
    SetWindowPos(_hWnd, HWND_TOP, dlg_x, dlg_y, dlg_width, dlg_height, 0);
}

std::wstring CMainDlg::get_server_from_url(LPCWSTR wzUrl)
{
    std::wstring s(wzUrl);
    size_t pos = s.find_first_of(L':');
    if (pos == std::wstring::npos) {
        return std::wstring();
    }
    s = s.substr(pos + 3);
    pos = s.find_first_of(L":/");
    if (pos != std::wstring::npos) {
        s = s.substr(0, pos);
    }
    return s;
}

LRESULT CALLBACK CMainDlg::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    LPMSG lpMsg = (LPMSG)lParam;

    CMainDlg* dlgptr = NULL;

    if (nCode >= 0 && PM_REMOVE == wParam)
    {
        // keyboard message only
        if ((lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST) && g_APPUI.hHostingWnd)
        {
            dlgptr = (CMainDlg*)::GetWindowLongPtrW(g_APPUI.hHostingWnd, GWLP_USERDATA);

            if (dlgptr)
            {
                if (S_OK == dlgptr->_pCtrl->TranslateAcceleratorIO(lpMsg))
                {
                    //
                    // TranslateAcceleratorIO return S_OK means this message is belongs to control
                    // we set message to WM_NULL (message hook can't return "don't processing") to prevent dialog box to swallow this message
                    //
                    lpMsg->message = WM_NULL;
                    lpMsg->lParam = 0;
                    lpMsg->wParam = 0;
                }
            }
        }
    }

    return CallNextHookEx(NULL /* per MSDN, this parameter is ignored*/, nCode, wParam, lParam);
}

INT_PTR CALLBACK CMainDlg::InterDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CMainDlg* dlgptr = NULL;
	
    if (WM_INITDIALOG == uMsg) {
        dlgptr = (CMainDlg*)lParam;
        ::SetWindowLongPtrW(hDlg, GWLP_USERDATA, lParam);
        g_APPUI.hHostingWnd = hDlg;

        dlgptr->_hHook = SetWindowsHookEx(WH_GETMESSAGE,
            GetMsgProc,
            NULL,
            GetCurrentThreadId());

        return dlgptr->OnInitDialog(hDlg);
    }
    
    dlgptr = (CMainDlg*)::GetWindowLongPtrW(hDlg, GWLP_USERDATA);
    if (NULL == dlgptr) {
        return 0;
    }

    switch (uMsg)
    {
    case WM_CLOSE:
        if (_logonType == NX_LOGON_SKYDRM)
        {
            dlgptr->DestroyDialog(IDCANCEL);
        }
        else
        {
            ::EndDialog(hDlg, IDCANCEL);
        }
        break;

    case WM_DESTROY:
        g_APPUI.hHostingWnd = NULL;

        UnhookWindowsHookEx(dlgptr->_hHook);
        dlgptr->_hHook = NULL;

        dlgptr->OnDestroy();
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_WEBLOGON:
            dlgptr->OnClickWebLogon();
            break;
        case IDOK:
            if (_logonType == NX_LOGON_SKYDRM)
            {
                dlgptr->DestroyDialog(IDOK);
            }
            break;
		case IDCLOSE:
			EndDialog(hDlg, IDOK);
			break;
        default:
            break;
        }
        return TRUE;

    case WM_SIZE:
        dlgptr->_pCtrl->OnSize();
        break;

    case WM_DRAWITEM:
        dlgptr->OnDrawItem((UINT)wParam, (DRAWITEMSTRUCT*)lParam);
        return TRUE;

    case WM_MOUSEMOVE:
        dlgptr->_back_button.OnMouseLeave();
        break;

    case WM_ERASEBKGND:
        dlgptr->OnEraseBkgnd((HDC)wParam);
        return TRUE;

    case WM_WEBPAGE_SET_HEIGHT:
        dlgptr->OnAdjustPageSize((int)HIWORD(wParam), (int)LOWORD(wParam));
        return TRUE;

    case WM_WEBPAGE_URL_SUCCEEDED:
        if (_logonType == NX_LOGON_SKYDRM)
        {
            if (lParam)
            {
                char *pResponse = (char*)lParam;
                string responseStr = pResponse;
                do {
                    NX::rapidjson::JsonDocument response;
                    int err_code = 0;
                    size_t err_pos = 0;
                    if (!response.LoadJsonString(responseStr, &err_code, &err_pos)) {
                        //res = RESULT(ERROR_INVALID_DATA);
                        break;
                    }

                    NX::rapidjson::JsonValue* root = response.GetRoot();
                    if (!(NULL != root && root->IsObject())) {
                        //res = RESULT(ERROR_INVALID_DATA);
                        break;
                    }

					if (NULL == root->AsObject()->at(L"statusCode") || NULL == root->AsObject()->at(L"message")) {
						break;
					}
                    try {

                        int statusCode = root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
                        auto message = root->AsObject()->at(L"message")->AsString();
                        auto msgStr = message->GetString();

                        //std::wstring errmsg;

                        if (statusCode == 200 && msgStr == L"Authorized")
                        {

                            //NX::rapidjson::JsonValue* extrObject = root->AsObject()->at(L"extra")->AsObject();

                            NX::rapidjson::JsonStringWriter<char> writer;
                            auto s = writer.Write(root);
                            auto res = theApp.m_pSession->LoginFromResult(s);
                            if (!res)
                            {
                                dlgptr->StopBrowsing();
                                MessageBox(hDlg, theApp.LoadString(IDS_LOGIN_FROM_RESULT_FAILED)/*L"Web login passed, but LoginFromResult failed"*/, theApp.LoadString(IDS_ERROR)/*L"Fatal error"*/, MB_OK);
                                ::PostMessage(hDlg, WM_CLOSE, 0, 0);
                            }
                            else
                            {
                                dlgptr->StopBrowsing();
                                ::PostMessage(hDlg, WM_COMMAND, IDOK, 0);
                            }
                        }
                    }
                    catch (const std::exception& e) {
                        UNREFERENCED_PARAMETER(e);
                    }
                } while (FALSE);

                free(pResponse);

            }
            else
            {
                dlgptr->StopBrowsing();
                ::PostMessage(hDlg, WM_CLOSE, 0, 0);
            }

            return TRUE;
        }
        else //NX_LOGON_OAUTH
        {
            if (lParam)
            {
				char *pResponse = (char*)lParam;
				string responseStr = pResponse;
				do {
					NX::rapidjson::JsonDocument response;
					int err_code = 0;
					size_t err_pos = 0;
					if (!response.LoadJsonString(responseStr, &err_code, &err_pos)) {
						break;
					}

					NX::rapidjson::JsonValue* root = response.GetRoot();
					if (!(NULL != root && root->IsObject())) {
						break;
					}

					if (root->AsObject()->at(L"statusCode") == NULL || root->AsObject()->at(L"message") == NULL) {
						break;//invalid JSon return
					}
					try {

						int statusCode = root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
						auto message = root->AsObject()->at(L"message")->AsString();
						auto msgStr = message->GetString();

						dlgptr->StopBrowsing();
						if (statusCode == 200){
							::PostMessage(hDlg, WM_COMMAND, IDCLOSE, 0);
						}
						else {
							wstringstream txtOut;
							txtOut << theApp.LoadStringW(IDS_FAILED_ADD_REPO).GetBuffer()/*L"Failed to add a repository."*/ << msgStr;
							theApp.ShowTrayMsg(txtOut.str().c_str());
							::PostMessage(hDlg, WM_CLOSE, 0, 0);
						}
					}
					catch (const std::exception& e) {
						UNREFERENCED_PARAMETER(e);
					}
				} while (FALSE);

				free(pResponse);
            }
			else
			{
				dlgptr->StopBrowsing();
				::PostMessage(hDlg, WM_CLOSE, 0, 0);
			}
            return TRUE;
        }
    }

    return 0;
}



CLinkButton::CLinkButton() : _h(NULL), _prev_wndproc(NULL), _prev_userdata(NULL), _is_hand(false)
{
}

CLinkButton::~CLinkButton()
{
    Detach();
}

void CLinkButton::Attach(HWND h)
{
    _prev_wndproc = (WNDPROC)::SetWindowLongPtrW(h, GWLP_WNDPROC, (LONG_PTR)CLinkButton::WndProc);
    assert(NULL != _prev_wndproc);
    _prev_userdata = ::SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)this);
    _h = h;
}

void CLinkButton::Detach()
{
    if (NULL != _h) {
        ::SetWindowLongPtrW(_h, GWLP_WNDPROC, (LONG_PTR)_prev_wndproc);
        ::SetWindowLongPtrW(_h, GWLP_USERDATA, (LONG_PTR)_prev_userdata);
        _prev_wndproc = NULL;
        _prev_userdata = NULL;
        _h = NULL;
    }
}

void CLinkButton::OnMouseMove()
{
    static HCURSOR hc_hand = ::LoadCursorW(NULL, IDC_HAND);
    if (!_is_hand) {
        ::SetCursor(hc_hand);
        _is_hand = true;
    }
}

void CLinkButton::OnMouseLeave()
{
    static HCURSOR hc_arrow = ::LoadCursorW(NULL, IDC_ARROW);
    if (_is_hand) {
        ::SetCursor(hc_arrow);
        _is_hand = false;
    }
}

LRESULT CALLBACK CLinkButton::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CLinkButton* pThis = (CLinkButton*)::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    if (pThis == NULL) {
        return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
    if (pThis->_prev_wndproc == NULL) {
    }

    switch (uMsg)
    {
    case WM_MOUSEMOVE:
        pThis->OnMouseMove();
        break;
    case WM_MOUSEHOVER:
        pThis->OnMouseMove();
        break;
    case WM_MOUSELEAVE:
        pThis->OnMouseLeave();
        break;
    default:
        break;
    }
    return (pThis->_prev_wndproc == NULL) ? ::DefWindowProcW(hWnd, uMsg, wParam, lParam) : pThis->_prev_wndproc(hWnd, uMsg, wParam, lParam);
}
