
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <winioctl.h>
#include <WinCred.h>
#include <Objbase.h>
#include <Dwmapi.h>

#include <sstream>
#include <map>
#include <vector>

#include <nudf\shared\enginectl.h>
#include <nudf\eh.hpp>
#include <nudf\shared\logdef.h>
#include <nudf\string.hpp>
#include <nudf\conversion.hpp>
#include <nudf\json.hpp>
#include <nxrmres.h>
#include <nudf\resutil.hpp>
#include <nudf\crypto.hpp>
#include <servctrl.hpp>

#include "resource.h"

#include "nxrmtray.h"
#include "engineutils.h"
#include "main_popup.h"
#include "rotateicon.h"
#include "DPIScaling.h"
#include "gdiplus.h"
#include "CGdiPlusBitmap.h"
#include "weblogondlg.hpp"

#include "pipe_server.hpp"
//#include <vld.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _AMD64_

    extern BOOL install_hook_x86(
        PVOID original_call,
        PVOID *trampoline_call,
        PVOID new_call);


#define install_hook		install_hook_x86

#else

    extern BOOL install_hook_x64(
        PVOID original_call,
        PVOID *trampoline_call,
        PVOID new_call);

#define install_hook		install_hook_x64

#endif

#ifdef __cplusplus
}
#endif

namespace TrimMemory {
#define NXRM_MEMTUNING_MAX_WORKINGSET             (1024*1024)
#define NXRM_MEMTUNING_MIN_WORKINGSET             (500*1024)
#define NXRM_MEMTUNING_TIMER  					  555		//Make this ID odd enough that other IDs will not be conflict with this one
#define NXRM_MEMTUNING_TIMESLOT					  30000		//Make memtrim 30 seconds

    void __stdcall TrimWorkingSet(void)
    {
        CoFreeUnusedLibraries();

        SetProcessWorkingSetSize(GetCurrentProcess(), 
            NXRM_MEMTUNING_MIN_WORKINGSET,
            NXRM_MEMTUNING_MAX_WORKINGSET);
    }
}
bool OpenEventViewerWindow(HINSTANCE hinst, HINSTANCE hResource, HWND hParentWnd);

using namespace TrimMemory;
using namespace Gdiplus;
using namespace RotateImage;

// Enable Visual Style: supported by ComCtrl32.dll version 6 and later, see the configure follow
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma endregion


// *************************************************************************
//
//  Define Global Variables
//
// *************************************************************************
using namespace NXSERV;

nxrm::tray::serv_status g_serv_status;
pipe_server             g_ipc_server;

CNXDPI    g_DPI;

#ifndef RESFILENAME
#define RESFILENAME     L"nxrmres.dll"
#endif
#ifndef RESFILENAME2
#define RESFILENAME2    L"res\\nxrmres.dll"
#endif

#define PIPE_LISTEN_PORT                L"RMCTrayPort"

#define MAX_LOADSTRING 100

#define COLOR_FONT_NORMAL                 (RGB(255, 255, 255))
#define COLOR_FONT_BLACK				  (RGB(0, 0, 0))	
#define COLOR_STATUS_OK                   (RGB(57, 151, 74))
#define COLOR_POLICY_OK                   (RGB(27, 116, 64))
#define COLOR_UPDATE_OK                   (RGB(19, 83, 45))
#define COLOR_REFRESH_OK_NORMAL           (RGB(24, 104, 47))
#define COLOR_REFRESH_OK_HOVER            (RGB(29, 150, 43))

#define COLOR_STATUS_BREAK                (RGB(181, 181, 181))
#define COLOR_POLICY_BREAK                (RGB(161, 161, 161))
#define COLOR_UPDATE_BREAK                (RGB(141, 141, 141))
#define COLOR_REFRESH_BREAK_NORMAL        (RGB(131, 131, 131))
#define COLOR_REFRESH_BREAK_HOVER         (RGB(151, 151, 151))

#define COLOR_SETTING_BK                  (RGB(255, 255, 255))
#define COLOR_SETTING_TEXT                (RGB(131, 131, 131))
#define COLOR_SETTING_HOVER               (RGB(181, 181, 181))
#define COLOR_SETTING_NORMAL              (RGB(161, 161, 161))


// Global Variables:
WCHAR szTitle[MAX_LOADSTRING];					// The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

APPUI        g_APPUI;                           // Global Variables (most are handles of control )
APPSTATE     g_APPState;                        // Used to initial the program.(about UI or create UI)
                
static BOOL  g_bMouseOverButton;                // Mark if the mouse is over the button
static int   g_iMouseOverButtonID;              // Button's ID of the mouse is hover
static int   g_iPopupStatus = POPUP_SHOW;       // Pop up status
static long  g_lPopupIncrement = 0;             // Used to move main windows
static int   g_expandedHeight = 0;				// Use this height to restore window height
LONG_PTR     g_BtnClosePreProc;                 // Store return value of SetWindowsLongPtr
LONG_PTR     g_BtnRefreshPreProc;
LONG_PTR     g_BtnSettingPreProc;

//Following are defined for icon rotation
enum RotateState
{
    NX_ICON_RT_STATE_STOPPED,
    NX_ICON_RT_STATE_ROTATING,
    NX_ICON_RT_STATE_STOPPING
};

RotateState		 g_updateIconRotate = NX_ICON_RT_STATE_STOPPED;		//Indcate that update icon rotate state

nxrm::tray::logon_ui_flag g_logon_ui_flag;

static UINT  WM_TASKBARCREATED = 0;

static int		 g_iAngle = 0;

static BOOL __stdcall nxrmtrayInternetReadFile(
    _In_ HINTERNET hFile,
    _Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID lpBuffer,
    _In_ DWORD dwNumberOfBytesToRead,
    _Out_ LPDWORD lpdwNumberOfBytesRead
);

static BOOL WINAPI nxrmtrayCreateProcessW(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
	);

static std::map<std::wstring, std::wstring> get_environment_variables(const wchar_t* sys_envs);
static std::vector<wchar_t> create_environment_block(const std::map<std::wstring, std::wstring>& variables);

// Forward declarations of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    OwnerDrawBNProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
BOOL                InitMainDlg(HWND hMainDlg);
bool                InitTrayIcon(HWND hWnd, HINSTANCE hInstance);
void                InitTrayIconMenu();
void                doShowTrayMsg(_In_ LPCWSTR wzTitle, _In_ LPCWSTR wzInfo);
void                doDrawBtnItem(HWND hWnd, LPARAM lParam, COLORREF bkColor, COLORREF txtColor, UINT uFormat, LONG lMargin);
HWND                doCreateToolTip(HWND hDlg, int iItemID, LPWSTR pszText);
void                doMoveMainDialog(void);
void                onShowingOrHidingPopup(void);
void                onBtnRefreshClicked(void);
void                onBtnSettingClicked(void);
void                QuitMain();
void				StopRotatingUpdateIndicator();
BOOL                IsDwmCompositionEnabled();
VOID                UpdateDwmCompositionStatus();

// Prepare Resource
HMODULE             LoadResourceModule();
VOID                UpdateTrayMenu(bool debug_on, bool external_authn, const std::wstring& logon_user);
VOID                UpdateServerStatus(const nxrm::tray::serv_status& st);
VOID                ProtectOfficeAddinKeys();
HRESULT             TranslateAcceleratorWebLogin(MSG *lpMsg);


// *************************************************************************
//
//  WinMain
//
// *************************************************************************
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
    
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    //MessageBox(NULL, L"ERROR", L"", MB_OK);

    MSG    msg;
    HACCEL hAccelTable;
    INITCOMMONCONTROLSEX icex;

    static ULONG_PTR m_gdiplusToken;

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);


    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NXRMUI, szWindowClass, MAX_LOADSTRING);
    
    std::wstring ws_port_seed;

    INT     nArgs = 0;
    LPCWSTR pwzCmdline = GetCommandLineW();
    LPWSTR* pwzArgs = CommandLineToArgvW(pwzCmdline, &nArgs);
    if(nArgs >= 1) {
        const WCHAR* pExeExt = wcsrchr(pwzArgs[0], L'.');
        if(NULL == pExeExt) pExeExt = pwzArgs[0];
        if(0 == _wcsicmp(pExeExt, L".exe")) {
            if(nArgs > 1) {
                ws_port_seed = pwzArgs[1];
            }
        }
        else {
            ws_port_seed = pwzArgs[0];
        }
    }

    
    // generate port
    LARGE_INTEGER li_seed = { 0, 0 };
    //li_seed.QuadPart = (LONGLONG)NX::crypto::ToCrc64(0, ws_port_seed.c_str(), (ULONG)(ws_port_seed.length() * sizeof(wchar_t)));
    //swprintf_s(nudf::string::tempstr<wchar_t>(gwsEngPipeName, 128), 128, L"NXRM_SERV_IPC_%08X%08X", li_seed.HighPart, li_seed.LowPart);
    //nxrm::tray::serv_control::set_serv_ctl_port(gwsEngPipeName);

    // Guarantee single instance
    nxrm::tray::session_guard   sguard;
    if (sguard.check_existence()) {
        HWND hWnTarget = FindWindowW(NULL, szTitle);
        if (NULL != hWnTarget) {
            ShowWindow(hWnTarget, SW_SHOWNORMAL);
            UpdateWindow(hWnTarget);
            SetForegroundWindow(hWnTarget);
        }
        return 0;
    }

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, SW_HIDE)) {
        return FALSE;
    }
    SetWindowTextW(g_APPUI.hMainDlg, szTitle);

    WM_TASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated");

    // Main message loop:
    hAccelTable = NULL;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            if(g_APPUI.hHtmlNotifyDlg == 0 || ! IsDialogMessage(g_APPUI.hHtmlNotifyDlg, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    Gdiplus::GdiplusShutdown(m_gdiplusToken);

    return (int) msg.wParam;
}

//Return the correct color according to whether is connected or mouse hover or not
COLORREF GetRefreshBtnBkColor()
{
    COLORREF bkColor;
    if (g_APPState.bConnected) {

        if (g_bMouseOverButton) {
            bkColor = COLOR_REFRESH_OK_HOVER;
        }
        else 
        {
            bkColor = COLOR_REFRESH_OK_NORMAL;
        }
    }
    else
    {
        if (g_bMouseOverButton) {
            bkColor = COLOR_REFRESH_BREAK_HOVER;
        }
        else {
            bkColor = COLOR_REFRESH_BREAK_NORMAL;
        }
    }
    return bkColor;

}

void DisplayIconInAngle(HWND hWnd, int iAngle)
{
    HWND hbtnWnd = GetDlgItem(hWnd, IDC_MAIN_POLICY_REFRESH);
    HDC hDc = GetDC(hbtnWnd);
    DrawRotatedIconFromPng(hDc, iAngle, IDB_REFRESH, GetRefreshBtnBkColor());
    ReleaseDC(hbtnWnd, hDc);
}
// *************************************************************************
//
//  Local Routines
//
// *************************************************************************

//This function tried to find login window in the process of the nxrmtray.exe
//All strings will be localized when language changes, we do not use text as identifier
//We'll use class name only as identifier at this time.
HWND  FindPasswordWindow()
{
    HWND hChildWnd = NULL;
    WCHAR szClass[] = L"#32770";
    do
    {
        hChildWnd = FindWindowEx(NULL, hChildWnd, szClass, g_APPUI.szLogonTitle.c_str());
        DWORD dwCurrThreadID = GetCurrentThreadId();
        DWORD dwWndProcID = 0;

        if (GetWindowThreadProcessId(hChildWnd, NULL) == dwCurrThreadID)
        {
            return hChildWnd;
        }

    } while (hChildWnd != NULL);

    return NULL;
}

std::wstring GetNotificationValue(std::wstring jsonStr, std::wstring & value)
{
#define CONTENT_CONNECTED	L"connected"
#define CONTENT_POLICY_ID	L"policyId"
#define	CONTENT_MESSAGE		L"message"
    try {
        NX::json_value json_msg = NX::json_value::parse(jsonStr);
        if (json_msg.as_object().has_field(CONTENT_CONNECTED))
        {
            value = json_msg.as_object()[CONTENT_CONNECTED].as_string();
            return CONTENT_CONNECTED;
        }
        if (json_msg.as_object().has_field(CONTENT_POLICY_ID))
        {
            value = json_msg.as_object()[CONTENT_POLICY_ID].as_string();
            return CONTENT_POLICY_ID;
        }
        if (json_msg.as_object().has_field(CONTENT_MESSAGE))
        {
            value = json_msg.as_object()[CONTENT_MESSAGE].as_string();
            return CONTENT_MESSAGE;
        }
    }
    catch (const std::exception& e) {
        UNREFERENCED_PARAMETER(e);
    }
    return L"";
}


void PopupNotificationMessage(std::wstring jsonStr)
{
    try {
        NX::json_value json_msg = NX::json_value::parse(jsonStr);
        std::wstring msg = json_msg[L"message"].as_string();
        const std::wstring notify_title = json_msg[L"title"].as_string();
        doShowTrayMsg(msg.c_str(), notify_title.c_str());
    }
    catch (const std::exception& e) {
        UNREFERENCED_PARAMETER(e);
    }
}

void OnLogonRequired()
{
    if (!g_logon_ui_flag.on()) {
		if (!g_APPState.bLoggedOn) {
			//only popup the login window when there is no user logged in
			std::wstring msg = NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)IDS_NOTIFY_LOGON_REQUIRED, 1024, g_APPState.dwLangId);
			doShowTrayMsg(msg.data());
			EngineOpenLogIn();
		}
    }
    else
    {
        HWND hPswdWnd = FindPasswordWindow();
        BringWindowToTop(hPswdWnd);
        SetForegroundWindow(hPswdWnd);
    }
}



//  Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPDRAWITEMSTRUCT pdis = NULL;
    int              iItemID;
    HBRUSH           hBrush;
    HDC              hDc;
    RECT             rcClient;

        if (message == WM_TASKBARCREATED)
    {
        InitTrayIcon(hWnd, g_APPUI.hInst);
        return TRUE;
    }

    switch (message)
    {
    case WM_CREATE:
        g_APPState.bInitialized = FALSE;
        if(!EngineGetStatus()) {
            QuitMain();
        }


        SetTimer(hWnd, (UINT_PTR)KEEPALIVE_TIMER, (UINT)KEEPALIVE_ELAPSE_SHORT, NULL);
        SetTimer(hWnd, (UINT_PTR)KEYPROTECT_TIMER, (UINT)KEYPROTECT_ELAPSE, NULL);
        //Reduce Memory Usage timer
        SetTimer(hWnd, NXRM_MEMTUNING_TIMER, NXRM_MEMTUNING_TIMESLOT, NULL);

        ProtectOfficeAddinKeys();

        {
            int nDPI = g_DPI.GetCurrentDPI();
            g_DPI.SetAwareness(PROCESS_SYSTEM_DPI_AWARE);
            g_DPI.SetScale(nDPI);
        }

        return TRUE;
    case WM_NCHITTEST:
        POINT pt;   
        pt.x = GET_X_LPARAM(lParam);   
        pt.y = GET_Y_LPARAM(lParam);    // The coordinate is relative to the upper-left corner of the screen.
        ScreenToClient(hWnd,&pt);       // Change to relative to the upper-left corner of the Client.
        RECT rcTitle;
        nxrmUtil::GetWindowRectRelative(hWnd, GetDlgItem(hWnd, IDC_MAIN_WINDOW_TITLE), &rcTitle);
        if(rcTitle.bottom > pt.y) {
            return HTCAPTION;
        }
        break;

    case WM_DWMCOMPOSITIONCHANGED:
        UpdateDwmCompositionStatus();
        break;
        
        // Process this message to change Main dialog's background color
    case WM_ERASEBKGND: 
        hDc = (HDC) wParam; 
        hBrush = CreateSolidBrush((g_APPState.bConnected ? COLOR_STATUS_OK : COLOR_STATUS_BREAK));
        GetClientRect(hWnd, &rcClient); 
        SetMapMode(hDc, MM_ANISOTROPIC); 
        SetWindowExtEx(hDc, 100, 100, NULL); 
        SetViewportExtEx(hDc, rcClient.right, rcClient.bottom, NULL); 
        FillRect(hDc, &rcClient, hBrush); 
        DeleteObject(hBrush);
        return TRUE; 

    case WM_COMMAND:
        iItemID = LOWORD(wParam);
        switch (iItemID)
        {
        case IDC_MAIN_BTN_CLOSE:
            if(POPUP_SHOWING == g_iPopupStatus || POPUP_HIDING == g_iPopupStatus) {
                return TRUE;
            }
            PostMessage(hWnd, WM_CLOSE, NULL, NULL);
            return TRUE;

        case IDC_MAIN_POLICY_REFRESH:
            if(POPUP_SHOWING == g_iPopupStatus || POPUP_HIDING == g_iPopupStatus || g_updateIconRotate != NX_ICON_RT_STATE_STOPPED) 
            {
                return TRUE;
            }

            g_iAngle  = 0; //Set start Angle
            SetTimer(hWnd, (UINT_PTR)ROTATE_REFRESH_TIMER, (UINT)ROTATE_REFRESH_TIME, NULL);
            g_updateIconRotate = NX_ICON_RT_STATE_ROTATING;

            onBtnRefreshClicked();
            return TRUE;

        case IDC_MAIN_SETTING:
            onBtnSettingClicked();
            return TRUE;

        default:
            break;
        }
        return TRUE;
    case WM_TIMER:
    {
        switch ((UINT_PTR)wParam)
        {
        case NXRM_MEMTUNING_TIMER:
            TrimWorkingSet();
            break;
        case KEEPALIVE_TIMER:
        {
            if (g_APPState.bInitialized) {
                if (!EngineConnected())
                    QuitMain();
            }
            else {
                EngineGetStatus();//refresh status until synced with service
            }
        }
        break;
        case KEYPROTECT_TIMER:
        {
            // Remove special keys
            ProtectOfficeAddinKeys();
        }
        break;
        case SHOW_TRAY_ICON_TIMER:
        {
            // show icon if succeed, we remove the timer
            if (InitTrayIcon(hWnd, g_APPUI.hInst))
            {
                KillTimer(hWnd, SHOW_TRAY_ICON_TIMER);
            }
        }
        break;
        case ROTATE_REFRESH_TIMER:
        {
            g_iAngle  += ROTATE_ANGLE_UNIT;
            if ((g_iAngle % 360) == 0)
            {
                if (g_updateIconRotate == NX_ICON_RT_STATE_STOPPING ||
                    g_iAngle / ROTATE_ANGLE_UNIT * ROTATE_REFRESH_TIME >= 7000) //If we exceeded 7 seconds we stop spinning the icon
                {
                    StopRotatingUpdateIndicator();
                }
            }
            DisplayIconInAngle(hWnd, g_iAngle);
        }
        break;
        }
    break;
    }

    case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
            int              iButtonID = lpDIS->CtlID;

            switch (iButtonID)
            {
            case IDC_MAIN_BTN_CLOSE:
                // The third Parameter is Back ground color
                doDrawBtnItem(hWnd, lParam, g_APPState.bConnected ? COLOR_UPDATE_OK : COLOR_POLICY_BREAK, COLOR_FONT_NORMAL, DT_RIGHT, 0);
                break;

            case IDC_MAIN_STATE_ICON:
                doDrawBtnItem(hWnd, lParam, g_APPState.bConnected ? COLOR_STATUS_OK : COLOR_STATUS_BREAK, COLOR_FONT_NORMAL, DT_RIGHT, 0);
                break;

            case IDC_MAIN_STATE_STR_CONNECT:
                doDrawBtnItem(hWnd, lParam, g_APPState.bConnected ? COLOR_STATUS_OK : COLOR_STATUS_BREAK, COLOR_FONT_NORMAL, DT_LEFT, 0);
                break;

            case IDC_MAIN_POLICY_LABEL:
                doDrawBtnItem(hWnd, lParam, g_APPState.bConnected ? COLOR_POLICY_OK : COLOR_POLICY_BREAK, COLOR_FONT_NORMAL, DT_RIGHT, 5);
                break;

            case IDC_MAIN_POLICY_VERSION:
                doDrawBtnItem(hWnd, lParam, g_APPState.bConnected ? COLOR_POLICY_OK : COLOR_POLICY_BREAK, COLOR_FONT_NORMAL, DT_LEFT, 5);
                break;

            case IDC_MAIN_LAST_UPDATE_LABEL:
                doDrawBtnItem(hWnd, lParam, g_APPState.bConnected ? COLOR_UPDATE_OK : COLOR_UPDATE_BREAK, COLOR_FONT_NORMAL, DT_LEFT, 10);
                break;

            case IDC_MAIN_LAST_UPDATE_TIME:
                doDrawBtnItem(hWnd, lParam, g_APPState.bConnected ? COLOR_UPDATE_OK : COLOR_UPDATE_BREAK, COLOR_FONT_NORMAL, DT_RIGHT, 10);
                break;

			case IDC_MAIN_WINDOW_TITLE:
				doDrawBtnItem(hWnd, lParam, g_APPState.bConnected ? COLOR_UPDATE_OK : COLOR_POLICY_BREAK, COLOR_FONT_NORMAL, DT_LEFT, 10);
				break;

            case IDC_MAIN_POLICY_REFRESH:
                if (g_updateIconRotate == NX_ICON_RT_STATE_STOPPED)
                {
                    if (g_APPState.bConnected) {

                        if ((g_bMouseOverButton && IDC_MAIN_POLICY_REFRESH == g_iMouseOverButtonID)) {
                            doDrawBtnItem(hWnd, lParam, COLOR_REFRESH_OK_HOVER, COLOR_FONT_NORMAL, DT_RIGHT, 0);
                        }
                        else {
                            doDrawBtnItem(hWnd, lParam, COLOR_REFRESH_OK_NORMAL, COLOR_FONT_NORMAL, DT_RIGHT, 0);
                        }
                    }
                    else
                    {
                        if ((g_bMouseOverButton && IDC_MAIN_POLICY_REFRESH == g_iMouseOverButtonID)) {
                            doDrawBtnItem(hWnd, lParam, COLOR_REFRESH_BREAK_HOVER, COLOR_FONT_NORMAL, DT_RIGHT, 0);
                        }
                        else {
                            doDrawBtnItem(hWnd, lParam, COLOR_REFRESH_BREAK_NORMAL, COLOR_FONT_NORMAL, DT_RIGHT, 0);
                        }
                    }
                }
                else
                {//This one is needed when the animationWindow blocked.
                    DisplayIconInAngle(hWnd, g_iAngle);
                }
                break;

            case IDC_MAIN_VERTICAL:
                doDrawBtnItem(hWnd, lParam, COLOR_SETTING_BK, COLOR_SETTING_TEXT, DT_CENTER, 0);
                break;

            case IDC_MAIN_TITLE:
                doDrawBtnItem(hWnd, lParam, COLOR_SETTING_BK, COLOR_SETTING_TEXT, DT_LEFT, 0);
                break;

            case IDC_MAIN_BRAND_BMP:
                 doDrawBtnItem(hWnd, lParam, COLOR_SETTING_BK, COLOR_SETTING_TEXT, DT_LEFT, 5);
                
                break;

            case IDC_MAIN_SETTING:
                if(TRUE == g_bMouseOverButton && IDC_MAIN_SETTING == g_iMouseOverButtonID) {
                    doDrawBtnItem(hWnd, lParam, COLOR_SETTING_HOVER, NULL, DT_RIGHT, 0);
                }
                else {
                    doDrawBtnItem(hWnd, lParam, COLOR_SETTING_NORMAL, NULL, DT_RIGHT, 0);
                }
                break;

            default:
                return TRUE;
            }

            return TRUE;
        }
        break;

    case WM_CTLCOLORSTATIC:
        iItemID = GetWindowLong ((HWND) lParam, GWL_ID);
        if(IDC_MAIN_STATE_STR_CONNECT == iItemID) {
            SetTextColor ((HDC) wParam, RGB(255, 255, 255)) ;
        }
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (BOOL)(ULONG_PTR)((HBRUSH)GetStockObject(NULL_BRUSH));
    case WM_TRAY:
        switch (lParam)
        {
        case WM_RBUTTONUP:
            {
                POINT pt;
                int cmd;

                GetCursorPos(&pt);
                SetForegroundWindow(hWnd);
                EnableMenuItem(g_APPUI.hTrayMenu, TRAY_MENU_EVENT_VIEWER_ID, !g_APPState.bLoggedOn);
                cmd = TrackPopupMenu(g_APPUI.hTrayMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);
                switch(cmd)
                {
                case TRAY_MENU_OPEN_RMC_ID:
                    ShowWindow(hWnd, SW_SHOWNORMAL);
                    UpdateWindow(hWnd);
                    SetForegroundWindow(hWnd);
                    break;
                case TRAY_MENU_SELFSERVICE_PORTAL_ID:
                    EngineGoToPortal(L"");
                    break;
                //case TRAY_MENU_OPEN_USER_GUIDE_ID:
                    //ShellExecuteW(NULL, L"open", L"http://customer.nextlabs.com/Documentation%20Library/RM/RMD_9_1_UserGuide.pdf"
                    //   , NULL, NULL, SW_SHOWNORMAL);
                    //break;
                case TRAY_MENU_UPDATE_POLICY_ID:
                    EngineUpdatePolicy();
                    break;
                case TRAY_MENU_ENABLE_DEBUG_ID:
                    EngineEnableDebug(!g_APPState.bIsDebugOn);
                    break;
                case TRAY_MENU_COLLECT_LOGS_ID:
                    //TODO:TEST GENERATE TRANSPARENT BITMAP CODE:
                    //GenerateWaterMarkFile();
                    EngineCollectLog();
					//TODO: REVERT TEMPERARY CHANGE
					//QuitMain();
					break;
				case TRAY_MENU_EVENT_VIEWER_ID:
				{	//_CrtMemState st;
					//_CrtMemCheckpoint(&st);
					/*bool bTest = true;*/
					OpenEventViewerWindow(g_APPUI.hInst, g_APPUI.hResDll, hWnd);
					//_CrtMemDumpAllObjectsSince(&st);
                    break;
                }
                case TRAY_MENU_LOGINOUT_ID:
                    if (!g_logon_ui_flag.on()) {
                        g_APPState.bLoggedOn ? EngineLogOut() : EngineOpenLogIn();
                    }
                    else
                    {
                        HWND hPswdWnd = FindPasswordWindow();
                        BringWindowToTop(hPswdWnd);
                        SetForegroundWindow(hPswdWnd);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case WM_LBUTTONDOWN:
            break;

        case WM_LBUTTONDBLCLK:
            ShowWindow(hWnd, SW_SHOWNORMAL);
            UpdateWindow(hWnd);
            SetForegroundWindow(hWnd);
            break;
        }
        break;

    case WM_CLOSE:
        ShowWindow(hWnd, SW_MINIMIZE);
        ShowWindow(hWnd, SW_HIDE);
        return 0;

    case WM_DESTROY:
        g_ipc_server.shutdown();
        g_APPUI.nid.uID = TRAY_ID;
        g_APPUI.nid.hWnd = hWnd;

        if (g_APPUI.hUserNameTip)
        {
            DestroyWindow(g_APPUI.hUserNameTip);
        }

        if (g_APPUI.hPolicyNameTip)
        {
            DestroyWindow(g_APPUI.hPolicyNameTip);
        }

        Shell_NotifyIcon(NIM_DELETE, &g_APPUI.nid);
        KillTimer(hWnd, (UINT)KEEPALIVE_TIMER);
        KillTimer(hWnd, (UINT)KEYPROTECT_TIMER);
        KillTimer(hWnd, (UINT)NXRM_MEMTUNING_TIMER);	//Kill this time for the memory tuning
        KillTimer(hWnd, (UINT)ROTATE_REFRESH_TIMER);
        EndDialog(hWnd, 0);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

//Owner draw button Proc
LRESULT CALLBACK OwnerDrawBNProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)  
{
    int      iCnt = 0;
    int      iControlID = 0;
    LONG_PTR PreProc = NULL;
    RECT     rect;
                           //ID of all window subclass's controls, whose PROC is  OwnerDrawBNProc
    //int      iButtonID[] = {IDC_TABBN_HOME, IDC_TABBN_UPDATE, IDC_TABBN_NOTIFY, IDC_TABBN_SETTING, IDC_HELP_BN};
    TRACKMOUSEEVENT tme;

    tme.cbSize = sizeof(tme);
    tme.hwndTrack = hWnd;
    tme.dwFlags = TME_LEAVE | TME_HOVER;
    tme.dwHoverTime = 1;

    iControlID = GetWindowLongW(hWnd, GWL_ID);
    //TODO: Get Current Child window control's ID and PreProc(Parent window's PROC normally)
    switch (iControlID)
    {
    case IDC_MAIN_BTN_CLOSE:
        PreProc = g_BtnClosePreProc;
        break;

    case IDC_MAIN_POLICY_REFRESH:
        PreProc = g_BtnRefreshPreProc;
        break;

    case IDC_MAIN_SETTING:
        PreProc = g_BtnSettingPreProc;
        break;

    default:
        return FALSE;
    }

    //Processing the message
    switch (message)
    {
    case WM_LBUTTONDBLCLK:
        PostMessage(hWnd, WM_LBUTTONDOWN, wParam, lParam);
        break;

    case WM_MOUSEMOVE:
        {
            TrackMouseEvent(&tme);  //"generate" WM_MOUSEHOVER and WM_MOUSELEAVE Message
            // return 0;
        }
        break;

    case WM_MOUSEHOVER:
        {
            if(POPUP_SHOWING == g_iPopupStatus || POPUP_HIDING == g_iPopupStatus)
            {
                return TRUE;
            }
            if(!g_bMouseOverButton )
            {
                g_bMouseOverButton = TRUE;
                g_iMouseOverButtonID = iControlID;
                GetClientRect(hWnd, &rect);
                if (iControlID != IDC_MAIN_POLICY_REFRESH || g_updateIconRotate == NX_ICON_RT_STATE_STOPPED)
                {
                    InvalidateRect(hWnd, &rect, TRUE);
                }
            }

            return 0;
        }// case WM_MOUSEHOVER

    case WM_MOUSELEAVE:
        {
            if(POPUP_SHOWING == g_iPopupStatus || POPUP_HIDING == g_iPopupStatus)
            {
                return TRUE;
            }
            if(g_bMouseOverButton)
            {
                g_bMouseOverButton = FALSE;
                GetClientRect(hWnd, &rect);
                if (iControlID != IDC_MAIN_POLICY_REFRESH || g_updateIconRotate == NX_ICON_RT_STATE_STOPPED)
                {
                    InvalidateRect(hWnd, &rect, TRUE);
                }
            }// if(g_bMouseOverButton)

            return 0;
        }// case WM_MOUSELEAVE

    default:
        break;
    }

    return CallWindowProc((WNDPROC)PreProc, hWnd, message, wParam, lParam);
}


//  FUNCTION: MyRegisterClass()
//  PURPOSE: Registers the window class.
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= DLGWINDOWEXTRA;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NXRMUI));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)NULL;
    wcex.lpszMenuName	= 0;                                    // If need a menu, use MAKEINTRESOURCE(IDC_NXRMUI)
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_NXRMUI));

    return RegisterClassEx(&wcex);
}

//   FUNCTION: InitInstance(HINSTANCE, int)
//   PURPOSE:  Saves instance handle and creates main window
//   COMMENTS: In this function, we store the instance handle in a global variable and create and display the main dialog window.
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_APPUI.hInst = hInstance;               // Store instance handle in our global variable
   g_APPUI.hResDll = LoadResourceModule();  // Load Resource Dll

   g_APPUI.hUserNameTip = NULL;
   g_APPUI.hPolicyNameTip = NULL;

   //Initial g_APPState struct
   g_APPState.dwLangId = LANG_NEUTRAL;      // Language ID
   g_APPState.bConnected = true;        
   g_APPState.bPopupNotify = TRUE;
   g_APPState.bIsDebugOn = true;
   g_APPState.bIconOn = false;
   g_APPState.bIsWin10 = false;
   wsprintfW(g_APPState.wzPolicyCreatedTime, L"%s", L"N/A");
   wsprintfW(g_APPState.wzLastUpdateTime, L"%s", L"N/A");
   wsprintfW(g_APPState.wzPolicyVersion, L"%s", L"N/A");

   // Init TrayIcon Menu
   InitTrayIconMenu();

   //TODO: Create main dialog
   HWND hMainDlg = NULL;
   hMainDlg = CreateDialogW(g_APPUI.hInst, MAKEINTRESOURCE(IDD_MAIN_DLG), 0, 0);
   if (!hMainDlg)
   {
        MessageBoxW(NULL, L"Create main dialog failed!", L"ERROR", MB_OK | MB_ICONERROR);
        return FALSE;
   }
   g_APPUI.hMainDlg = hMainDlg;

   //Some other initializations
   InitMainDlg(hMainDlg);
   ShowWindow(hMainDlg, nCmdShow);
   UpdateWindow(hMainDlg);
   InitTrayIcon(hMainDlg, g_APPUI.hInst);
   if (!g_APPState.bIconOn)
   {
       SetTimer(hMainDlg, SHOW_TRAY_ICON_TIMER, SHOW_TRAY_ICON_TIME, NULL);
   }


   // Update Aero Status
   UpdateDwmCompositionStatus();

   // Get Wininet handle
   // we have dependency on wininet. This make sure we don't need to reference the module
   //
   g_APPUI.hWininet = GetModuleHandleW(L"wininet.dll");

   if (g_APPUI.hWininet)
   {
       g_APPUI.fn_org_InternetReadFile = (INTERNETREADFILE)GetProcAddress(g_APPUI.hWininet, "InternetReadFile");

       if (g_APPUI.fn_org_InternetReadFile)
       {
           install_hook(g_APPUI.fn_org_InternetReadFile, (PVOID*)&g_APPUI.fn_InternetReadFile_trampoline, nxrmtrayInternetReadFile);
       }
   }

   HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");

   if (hKernelBase)
   {
	   g_APPUI.fn_org_CreateProcessW = (CREATEPROCESSW)GetProcAddress(hKernelBase, "CreateProcessW");

	   if (g_APPUI.fn_org_CreateProcessW)
	   {
		   install_hook(g_APPUI.fn_org_CreateProcessW, (PVOID*)&g_APPUI.fn_CreateProcessW_trampoline, nxrmtrayCreateProcessW);
	   }
   }
   
   if (NULL == g_APPUI.fn_org_CreateProcessW) {
	   //For Windows 7, hook CreateProcessW at kernel32.dll
	   HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");

	   if (hKernel32) {
		   g_APPUI.fn_org_CreateProcessW = (CREATEPROCESSW)GetProcAddress(hKernel32, "CreateProcessW");

		   if (g_APPUI.fn_org_CreateProcessW)
		   {
			   install_hook(g_APPUI.fn_org_CreateProcessW, (PVOID*)&g_APPUI.fn_CreateProcessW_trampoline, nxrmtrayCreateProcessW);
		   }
	   }
   }
   return TRUE;
}


//TODO: Initial Main Dialog's Tab Control
BOOL InitMainDlg(HWND hMainDlg)
{
    int      iCnt;
    std::wstring wsUiText;      // Store Msg string Loaded from ResDLL
    RECT     rcClient1;
    RECT     rcClient2;

    LONG wBtnCaptionResID[] = {
        IDS_MAIN_LABEL_POLICY, 
        IDS_MAIN_LABEL_LAST_UPDATE, 
        IDS_MAIN_TITLE,
		IDS_MAIN_WINDOW_TITLE};

    int iBtnWithCapID[] = {
        IDC_MAIN_POLICY_LABEL, 
        IDC_MAIN_LAST_UPDATE_LABEL, 
        IDC_MAIN_TITLE,
		IDC_MAIN_WINDOW_TITLE};

    int iBtnClickable[] = {
        IDC_MAIN_BTN_CLOSE, 
        IDC_MAIN_POLICY_REFRESH, 
        IDC_MAIN_SETTING};

    LONG lTipResID[] = {
        IDS_MAIN_CLOSE_TIP, 
        IDS_MAIN_REFRESH_TIP, 
        IDS_MAIN_SETTING_TIP};
    
    //
    //  Initialize Pipe
    //
    try {
        unsigned long session_id = 0;
        ProcessIdToSessionId(GetCurrentProcessId(), &session_id);
        std::wstring port_name(L"nxrmtray_");
        port_name += NX::conversion::to_wstring((int)session_id);
        g_ipc_server.listen(port_name);
    }
    catch (const std::exception& e) {
        UNREFERENCED_PARAMETER(e);
#ifdef _DEBUG
        const std::string errMessage = NX::string_formater("Fail to start pipe (%d, %s)", GetLastError(), e.what());
        MessageBoxA(hMainDlg, errMessage.c_str(), "NextLabs", MB_OK | MB_ICONERROR);
#endif
    }

    // Try to get Engine Information
    EngineGetStatus();

    //TODO: subclass the button so we can know when the mouse was moved over it
    //Close button
    g_BtnClosePreProc = SetWindowLongPtr(GetDlgItem(hMainDlg, IDC_MAIN_BTN_CLOSE), GWLP_WNDPROC, 
        (LONG_PTR)OwnerDrawBNProc);
    //Refresh button
    g_BtnRefreshPreProc = SetWindowLongPtr(GetDlgItem(hMainDlg, IDC_MAIN_POLICY_REFRESH), GWLP_WNDPROC, 
        (LONG_PTR)OwnerDrawBNProc);
    //Setting button
    g_BtnSettingPreProc = SetWindowLongPtr(GetDlgItem(hMainDlg, IDC_MAIN_SETTING), GWLP_WNDPROC, 
        (LONG_PTR)OwnerDrawBNProc);



    //TODO: Initial the Caption of button
    for (iCnt = 0; iCnt < sizeof(wBtnCaptionResID) / sizeof(wBtnCaptionResID[0]); ++iCnt)
    {
        // Set the caption of control
        wsUiText =  NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)wBtnCaptionResID[iCnt], 1024, g_APPState.dwLangId);
        SendMessageW(GetDlgItem(g_APPUI.hMainDlg, iBtnWithCapID[iCnt]), WM_SETTEXT, NULL, (LPARAM)(LPWSTR)wsUiText.c_str());
    }
    // Connect status string
    //wsUiText = NX::RES::LoadMessage(g_APPUI.hResDll,
    //	(g_APPState.bConnected ? IDS_MAIN_STATE_CONNECTED : IDS_MAIN_STATE_DISCONNECTED), 1024,
    //	g_APPState.dwLangId);

    wsUiText = g_APPState.bConnected ? g_APPState.wzCurrentUser : NX::RES::LoadMessage(g_APPUI.hResDll, IDS_MAIN_STATE_DISCONNECTED, 1024, g_APPState.dwLangId);

    SendMessageW(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_STATE_STR_CONNECT), WM_SETTEXT, NULL, (LPARAM)(LPWSTR)wsUiText.c_str());

    //Policy Version
    if(g_APPState.wzPolicyVersion != '\0')
    {
        SendMessageW(GetDlgItem(g_APPUI.hMainDlg,IDC_MAIN_POLICY_VERSION), WM_SETTEXT, NULL, (LPARAM)g_APPState.wzPolicyVersion);
    }
    //Last updated time
    if(g_APPState.wzLastUpdateTime != '\0')
    {
        SendMessageW(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_LAST_UPDATE_TIME), WM_SETTEXT, NULL, (LPARAM)g_APPState.wzLastUpdateTime);
    }


    //TODO: Set the tooltip for button that can be click
    for (iCnt = 0; iCnt < sizeof(lTipResID) / sizeof(lTipResID[0]); ++iCnt)
    {
        // Set the caption of control
        wsUiText =  NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)lTipResID[iCnt], 1024, g_APPState.dwLangId);
        doCreateToolTip(g_APPUI.hMainDlg, iBtnClickable[iCnt], (LPWSTR)wsUiText.c_str());
    }

    InitMainPopupDlg();
    ShowWindow(g_APPUI.hMainPopupDlg,SW_SHOW);
    
    //TODO: Calculate Move windows increment
    GetWindowRect(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_SETTING), &rcClient1);
    GetWindowRect(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_LAST_UPDATE_LABEL), &rcClient2);
    g_lPopupIncrement = rcClient1.top - rcClient2.bottom;
    GetWindowRect(g_APPUI.hMainDlg, &rcClient1);
    g_expandedHeight = rcClient1.bottom - rcClient1.top;

    onBtnSettingClicked();

    return TRUE;
}


VOID UpdateServerStatus(const nxrm::tray::serv_status& st)
{
    g_APPState.bInitialized = TRUE;
    if (g_APPState.bConnected != st.connected()) {
        g_APPState.bConnected = st.connected();

        std::wstring wsUiText = g_APPState.bConnected ? st.logon_user() : NX::RES::LoadMessage(g_APPUI.hResDll, IDS_MAIN_STATE_DISCONNECTED, 1024, g_APPState.dwLangId);

        SendMessageW(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_STATE_STR_CONNECT), WM_SETTEXT, NULL, (LPARAM)(LPWSTR)wsUiText.c_str());
        RedrawWindow(g_APPUI.hMainDlg, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
    }

    if (0 != _wcsicmp(g_APPState.wzCurrentServer, st.logon_server().c_str())) {
        wcsncpy_s(g_APPState.wzCurrentServer, 64, st.logon_server().c_str(), _TRUNCATE);
    }

    UpdateTrayMenu(g_serv_status.debug(), g_serv_status.external_authn(), g_serv_status.logon_user());

    if (0 != _wcsicmp(g_APPState.wzCurrentUser, st.logon_user().c_str())) {
        wcsncpy_s(g_APPState.wzCurrentUser, 64, st.logon_user().c_str(), _TRUNCATE);
        wcsncpy_s(g_APPState.wzCurrentUserId, 64, st.logon_user_id().empty() ? L"" : st.logon_user_id().c_str(), _TRUNCATE);
    }
    if (0 != _wcsicmp(g_APPState.wzPolicyCreatedTime, st.policy_id().c_str())) {
        if (st.policy_id().empty()) {
            wcsncpy_s(g_APPState.wzPolicyCreatedTime, 256, L"N/A", _TRUNCATE);
        }else{
            wcsncpy_s(g_APPState.wzPolicyCreatedTime, 256, st.policy_id().c_str(), _TRUNCATE);
        }
        wcsncpy_s(g_APPState.wzPolicyVersion, 256, g_APPState.wzPolicyCreatedTime, _TRUNCATE);
        SendMessageW(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_POLICY_VERSION), WM_SETTEXT, NULL, (LPARAM)g_APPState.wzPolicyCreatedTime);
        RedrawWindow(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_POLICY_VERSION), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
    }
	bool lastupdate_changed = false;
	if (0 != _wcsicmp(g_APPState.wzLastUpdateTime, st.policy_updatetime().c_str())) {
        // Show new updated information
        if (st.policy_updatetime().empty()) {
            wcsncpy_s(g_APPState.wzLastUpdateTime, 256, L"N/A", _TRUNCATE);
        }
        else {
            wcsncpy_s(g_APPState.wzLastUpdateTime, 256, st.policy_updatetime().c_str(), _TRUNCATE);
        }
        SendMessageW(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_LAST_UPDATE_TIME), WM_SETTEXT, NULL, (LPARAM)g_APPState.wzLastUpdateTime);
        RedrawWindow(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_LAST_UPDATE_TIME), NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
		lastupdate_changed = true;
    }
	if (lastupdate_changed) {
		// Change tray-icon tip
		std::wstring wsTipFmt = NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)IDS_TRAY_TIP, 1024, g_APPState.dwLangId);
		std::wstring wsTip;
		swprintf_s(NX::string_buffer<wchar_t>(wsTip, 1024), 1023, wsTipFmt.c_str(), g_APPState.wzLastUpdateTime);
		wcsncpy_s(g_APPUI.nid.szTip, 127, wsTip.c_str(), _TRUNCATE);
		Shell_NotifyIcon(NIM_MODIFY, &g_APPUI.nid);
	}
}

//  Show Tray icon
void InitTrayIconMenu()
{
    if(NULL == g_APPUI.hTrayMenu) {

        g_APPUI.hTrayMenu = CreatePopupMenu();
        if(NULL == g_APPUI.hTrayMenu) {
            return ;
        }
        // Add menu item
        std::wstring menu_text;  // Msg Loaded from ResDLL
        menu_text =  NX::RES::LoadMessageEx(g_APPUI.hResDll, IDS_TRAY_MENU_SELF_SERVICE_PORTAL, 1024, g_APPState.dwLangId, L"Goto SkyDRM.com");
        AppendMenuW(g_APPUI.hTrayMenu, MF_STRING, TRAY_MENU_SELFSERVICE_PORTAL_ID, menu_text.c_str());
        menu_text =  NX::RES::LoadMessageEx(g_APPUI.hResDll, IDS_TRAY_MENU_OPEN_RMC, 1024, g_APPState.dwLangId, L"Open control panel...");
        AppendMenuW(g_APPUI.hTrayMenu, MF_STRING, TRAY_MENU_OPEN_RMC_ID, menu_text.c_str());

        //menu_text = NX::RES::LoadMessageEx(g_APPUI.hResDll, IDS_TRAY_MENU_OPEN_USER_GUIDE, 1024, g_APPState.dwLangId, L"Open user guide");
        //AppendMenuW(g_APPUI.hTrayMenu, MF_STRING, TRAY_MENU_OPEN_USER_GUIDE_ID, menu_text.c_str());

        menu_text =  NX::RES::LoadMessageEx(g_APPUI.hResDll, IDS_TRAY_MENU_UPDATE_POLICY, 1024, g_APPState.dwLangId, L"Update policy");
        AppendMenuW(g_APPUI.hTrayMenu, MF_STRING, TRAY_MENU_UPDATE_POLICY_ID, menu_text.c_str());
#ifdef _DEBUG
        menu_text =  NX::RES::LoadMessageEx(g_APPUI.hResDll, g_APPState.bIsDebugOn ? IDS_TRAY_MENU_DISABLE_DEBUG : IDS_TRAY_MENU_ENABLE_DEBUG, 1024, g_APPState.dwLangId, g_APPState.bIsDebugOn ? L"Disable debug" : L"Enable debug");
        AppendMenuW(g_APPUI.hTrayMenu, MF_STRING, TRAY_MENU_ENABLE_DEBUG_ID, menu_text.c_str());
#endif
        menu_text =  NX::RES::LoadMessageEx(g_APPUI.hResDll, IDS_TRAY_MENU_COLLECT_LOGS, 1024, g_APPState.dwLangId, L"Collect logs...");
        AppendMenuW(g_APPUI.hTrayMenu, MF_STRING, TRAY_MENU_COLLECT_LOGS_ID, menu_text.c_str());
        InsertMenuW(g_APPUI.hTrayMenu, TRAY_MENU_OPEN_RMC_ID + 1, MF_SEPARATOR, NULL, NULL);  //Draw a horizontal dividing line.
#ifdef _DEBUG
		menu_text = NX::RES::LoadMessageEx(g_APPUI.hResDll, IDS_TRAY_MENU_OPEN_EVTVIEWER, 1024, g_APPState.dwLangId, L"Open event viewer");
		AppendMenuW(g_APPUI.hTrayMenu, MF_STRING, TRAY_MENU_EVENT_VIEWER_ID, menu_text.c_str());
#endif
    }
}


bool InitTrayIcon(HWND hMainDlg, HINSTANCE hInstance)
{
    int   iRet;

    g_APPUI.nid.cbSize = sizeof(NOTIFYICONDATA);
    g_APPUI.nid.hWnd = hMainDlg;
    g_APPUI.nid.uID = TRAY_ID;
    g_APPUI.nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    g_APPUI.nid.uCallbackMessage = WM_TRAY;
    g_APPUI.nid.uTimeout = 1000;                   //The timeout value, in milliseconds, for notification.
    static const GUID BalloonGUID = { 0xb113fe22, 0x21a1, 0x43a8, { 0x8c, 0x62, 0xfb, 0x22, 0x13, 0xd, 0x3, 0x94 } };
    g_APPUI.nid.guidItem = BalloonGUID;
    g_APPUI.nid.dwInfoFlags = NIIF_USER;
    g_APPUI.nid.uVersion = NOTIFYICON_VERSION_4;

    g_APPUI.nid.hIcon = (HICON)LoadImage( // returns a HANDLE so we have to cast to HICON
        hInstance,             // hInstance must be NULL when loading from a file
        MAKEINTRESOURCE(IDI_NXRMSHIELD),    // the icon RESOURCE
        IMAGE_ICON,       // specifies that the file is an icon
        0,         // width of the image (we'll specify default later on)
        0,         // height of the image
        LR_DEFAULTSIZE
    );

    // Tray Icon Tip
    std::wstring wsTipFmt =  NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)IDS_TRAY_TIP, 1024, g_APPState.dwLangId);
    swprintf_s(g_APPUI.nid.szTip, 128, wsTipFmt.c_str(), g_APPState.wzLastUpdateTime);
    // Message bubble title
    std::wstring bubble_title =  NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)IDS_PRODUCT_NAME, 1024, g_APPState.dwLangId);
    lstrcpy(g_APPUI.nid.szInfoTitle, bubble_title.empty() ? L"" : bubble_title.c_str());

    //TODO: Show the tray icon
    Shell_NotifyIconW(NIM_DELETE, &g_APPUI.nid);    // Always remove tray-icon before we add a new one
    iRet = Shell_NotifyIcon(NIM_ADD, &g_APPUI.nid);
    if(FALSE == iRet)
    {
        
        return false;
    }
    g_APPState.bIconOn = true;
    return true;
}

//TODO: show pop-up message when needed, it is not used currently(just for test)
void doShowTrayMsg(_In_ LPCWSTR wzInfo, _In_opt_ LPCWSTR wzTitle)
{
    std::wstring bubble_title((NULL == wzTitle) ? L"" : wzTitle);
    if (bubble_title.empty()) {
        bubble_title = NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)IDS_PRODUCT_NAME, 1024, g_APPState.dwLangId, L"SkyDRM");
    }


    if(g_APPState.bPopupNotify) {
        g_APPUI.nid.uFlags |= NIF_INFO;
        lstrcpy(g_APPUI.nid.szInfoTitle, bubble_title.c_str());
        lstrcpy(g_APPUI.nid.szInfo, wzInfo);
        g_APPUI.nid.hIcon = (HICON)LoadImage( // returns a HANDLE so we have to cast to HICON
            g_APPUI.hInst,             // hInstance must be NULL when loading from a file
            MAKEINTRESOURCE(IDI_NXRMSHIELD),    // the icon RESOURCE
            IMAGE_ICON,       // specifies that the file is an icon
            0,         // width of the image (we'll specify default later on)
            0,         // height of the image
            LR_DEFAULTSIZE
        );
        Shell_NotifyIcon(NIM_MODIFY, &g_APPUI.nid);
        g_APPUI.nid.uFlags &= (~NIF_INFO);
        g_APPUI.nid.szInfoTitle[0] = L'\0';
        g_APPUI.nid.szInfo[0] = L'\0';
    }
}

void doShowTrayMsg(_In_ UINT msgid, _In_opt_ LPCWSTR wzTitle)
{
	std::wstring msg;

	msg = NX::RES::LoadMessage(g_APPUI.hResDll, msgid, 1024, g_APPState.dwLangId, L"");
	if (!msg.empty()) {
		doShowTrayMsg(msg.c_str(), wzTitle);
	}
}

HFONT CreateControlFont(int iButtonID)
{
    HFONT hFont = NULL;

    int nFontHeight = 0;
    int nFontStyle = 0;

    switch (iButtonID)
    {
    case IDC_MAIN_STATE_STR_CONNECT:
        nFontHeight = 24;
        nFontStyle = FW_REGULAR;
        break;
    case IDC_MAIN_POLICY_VERSION:
        nFontHeight = 24;
        nFontStyle = FW_BOLD;
        break;
    case IDC_MAIN_POLICY_LABEL:
        nFontHeight = 16;
        nFontStyle = FW_BOLD;
        break;
    case IDC_MAIN_LAST_UPDATE_TIME:
        nFontHeight = 16;
        nFontStyle = FW_BOLD;
        break;
    default:
        return NULL;
    }

    nFontHeight = g_DPI.Scale(nFontHeight);
    hFont = CreateFont(nFontHeight, 0, 0, 45, nFontStyle, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
        DRAFT_QUALITY, FF_DONTCARE, TEXT("Segoe UI"));
    return hFont;
}


HFONT DecreaseFontHeightAndSelect(HDC &hDC, HFONT &hFont)
{
    HFONT hnewFont = NULL;
    LOGFONT lf;
    GetObject(hFont, sizeof(lf), &lf);
    if (lf.lfHeight > 0)
    {
        lf.lfHeight--;
    }
    else
    {
        lf.lfHeight++;
    }
    hnewFont = CreateFontIndirect(&lf);
    SelectObject(hDC, hnewFont);
    DeleteObject(hFont);
    return hnewFont;
}

// Description:
//   Draw the button item in WM_DRAWITEM
// Parameters:
//   hDc - handle of Device contents
//   rcBtn - REVT of button
//   bkColor - The background color of button.
//   hIcon - The handle of en icon for the button, if the button has one.
//   wzCaption -The caption of the button, if it has a caption
//   uFormat - The method of formatting the text. But should just be DT_CENTER/DT_LEFT/DT_RIGHT
// Returns:
//   void.
void doDrawBtnItem(HWND hWnd, LPARAM lParam, COLORREF bkColor, COLORREF txtColor, UINT uFormat, LONG lMargin)
{
    HWND             hWndItem;
    HICON            hIcon = NULL;
    HBITMAP          hBmp = NULL;
    HBITMAP          hbmOld = NULL;
    HDC              hdcMem = NULL;
    WCHAR            wzCaption[MAX_STRLEN] = {0};
    HBRUSH           brBackground = NULL;

    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
    int              iButtonID   = lpDIS->CtlID;
    HDC              hDc         = lpDIS->hDC; 
    BOOL             bIsPressed  = (lpDIS->itemState & ODS_SELECTED);
    BOOL             bIsFocused  = (lpDIS->itemState & ODS_FOCUS);
    BOOL             bIsDisabled = (lpDIS->itemState & ODS_DISABLED);
    BOOL             bDrawFocusRect = !(lpDIS->itemState & ODS_NOFOCUSRECT);
    RECT             rcBtn     = lpDIS->rcItem;


    LONG centerRectWidth   = rcBtn.right - rcBtn.left;   // temp of original ButtonRect when draw button control's text
    LONG centerRectHeight  = rcBtn.bottom - rcBtn.top;

    hWndItem = GetDlgItem(hWnd, iButtonID);
    GetWindowText(hWndItem, wzCaption, MAX_STRLEN);

    //Paint the background color
    SetBkMode(hDc, TRANSPARENT);
    brBackground = CreateSolidBrush(bkColor);
    FillRect(hDc, &rcBtn, brBackground);
    DeleteObject(brBackground);

    if (lstrlen(wzCaption) != 0 && hIcon == NULL)
    {
        if(DT_LEFT == uFormat)
        {
            rcBtn.left += lMargin;
        }
        else if(DT_RIGHT == uFormat)
        {
            rcBtn.right -= lMargin;
        }
        HFONT hFont = CreateControlFont(iButtonID);
        SetBkMode(hDc, TRANSPARENT);
        SetTextColor(hDc, txtColor);
        SetBkColor(hDc, ::GetSysColor(COLOR_BTNFACE));
        HFONT hOldFont;
        if (hFont)
        {
            hOldFont = SelectFont(hDc, hFont);
        }
        RECT rcTextArea;
        
        if (iButtonID == IDC_MAIN_STATE_STR_CONNECT) 
        {
            RECT rcClient;
            RECT rcCheck;
            RECT rcUsrName;
            GetClientRect(g_APPUI.hMainDlg, &rcClient);
            HWND hChkWnd = GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_STATE_ICON);
            HWND hUsrName = GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_STATE_STR_CONNECT);
            nxrmUtil::GetWindowRectRelative(g_APPUI.hMainDlg, hChkWnd, &rcCheck);
            nxrmUtil::GetWindowRectRelative(g_APPUI.hMainDlg, hUsrName, &rcUsrName);
            DrawText(hDc, wzCaption, -1, &rcTextArea, DT_SINGLELINE | DT_VCENTER | uFormat | DT_CALCRECT);
            if (nxrmUtil::RectWidth(rcTextArea) > nxrmUtil::RectWidth(rcBtn))
            {//At this time Text is longer than the space area to display in UI
             //Use the control size to move window
                int textOffset = nxrmUtil::RectWidth(rcCheck) * 5 / 4;
                int objWidth = textOffset + nxrmUtil::RectWidth(rcUsrName);
                int nStartDisp = (nxrmUtil::RectWidth(rcClient) - objWidth)/2;
                MoveWindow(hChkWnd, nStartDisp, rcCheck.top, nxrmUtil::RectWidth(rcCheck), nxrmUtil::RectHeight(rcCheck), TRUE);
                MoveWindow(hUsrName, nStartDisp+textOffset, rcUsrName.top, nxrmUtil::RectWidth(rcUsrName), nxrmUtil::RectHeight(rcUsrName), TRUE);

                HBRUSH hBrush = CreateSolidBrush((g_APPState.bConnected ? COLOR_STATUS_OK : COLOR_STATUS_BREAK));
                FillRect(hDc, &rcBtn, hBrush);
                DeleteObject(hBrush);

                DrawText(hDc, wzCaption, -1, &rcBtn, DT_SINGLELINE | DT_VCENTER | uFormat | DT_END_ELLIPSIS );
                if (g_APPUI.hUserNameTip)
                {
                    DestroyWindow(g_APPUI.hUserNameTip);
                }
                g_APPUI.hUserNameTip = doCreateToolTip(g_APPUI.hMainDlg, iButtonID, wzCaption);
            }
            else
            {
                //Us string size to move window
                int textOffset = nxrmUtil::RectWidth(rcCheck) * 5 / 4;
                int objWidth = textOffset + nxrmUtil::RectWidth(rcTextArea);
                int nStartDisp = (nxrmUtil::RectWidth(rcClient) - objWidth) / 2;
                MoveWindow(hChkWnd, nStartDisp, rcCheck.top, nxrmUtil::RectWidth(rcCheck), nxrmUtil::RectHeight(rcCheck), TRUE);
                MoveWindow(hUsrName, nStartDisp + textOffset, rcUsrName.top, nxrmUtil::RectWidth(rcUsrName), nxrmUtil::RectHeight(rcUsrName), TRUE);
                HBRUSH hBrush = CreateSolidBrush((g_APPState.bConnected ? COLOR_STATUS_OK : COLOR_STATUS_BREAK));
                FillRect(hDc, &rcBtn, hBrush);
                DeleteObject(hBrush);
                DrawText(hDc, wzCaption, -1, &rcBtn, DT_SINGLELINE | DT_VCENTER | uFormat);
            }
        }
        else if (iButtonID == IDC_MAIN_POLICY_VERSION)
        {
            DrawText(hDc, wzCaption, -1, &rcTextArea, DT_SINGLELINE | DT_VCENTER | uFormat | DT_CALCRECT);
            if (nxrmUtil::RectWidth(rcTextArea) > nxrmUtil::RectWidth(rcBtn))
            {//At this time Text is longer than the space area to display in UI
                DrawText(hDc, wzCaption, -1, &rcBtn, DT_SINGLELINE | DT_VCENTER | uFormat | DT_END_ELLIPSIS);
                if (g_APPUI.hPolicyNameTip)
                {
                    DestroyWindow(g_APPUI.hPolicyNameTip);
                }
                g_APPUI.hPolicyNameTip = doCreateToolTip(g_APPUI.hMainDlg, iButtonID, wzCaption);
            }
            else
            {
                DrawText(hDc, wzCaption, -1, &rcBtn, DT_SINGLELINE | DT_VCENTER | uFormat);
            }
        }
        else
        {
            DrawText(hDc, wzCaption, -1, &rcTextArea, DT_SINGLELINE | DT_VCENTER | uFormat | DT_CALCRECT);
            int count = 5;  //This count is to make sure that it will not block the program.
            while (nxrmUtil::RectWidth(rcTextArea) > nxrmUtil::RectWidth(rcBtn) && count >= 0)
            {
                hFont = DecreaseFontHeightAndSelect(hDc, hFont); //hFont passed in will be deleted, and pass back hFont for delete after use
                DrawText(hDc, wzCaption, -1, &rcTextArea, DT_SINGLELINE | DT_VCENTER | uFormat | DT_CALCRECT);
                count--;
            }
            DrawText(hDc, wzCaption, -1, &rcBtn, DT_SINGLELINE | DT_VCENTER | uFormat);
        }
        
        if (hFont)
        {
            SelectFont(hDc, hOldFont);	//Select back the font.
            DeleteObject(hFont);
        }
    }
    
    //Draw Icon
    int iconWidth = 0;
    switch (iButtonID)
    {
    case IDC_MAIN_BTN_CLOSE:
    {
        CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
        bool bBmpLoaded = false;
        if (TRUE == g_bMouseOverButton && IDC_MAIN_BTN_CLOSE == g_iMouseOverButtonID) {
            bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_CLOSE_HOVER), L"PNG", g_APPUI.hInst);
        }
        else {
            bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_CLOSE_NORMAL), L"PNG", g_APPUI.hInst);
        }
        if (bBmpLoaded)
        {
            Gdiplus::Graphics graphics(hDc);
            graphics.SetSmoothingMode(SmoothingModeHighQuality);
            int iconWidth = nxrmUtil::RectWidth(rcBtn);
            int iconHeight = nxrmUtil::RectHeight(rcBtn);
            Rect rc(rcBtn.left, (iconHeight - iconWidth) / 2, iconWidth, iconWidth);
            graphics.DrawImage(*pBitmap, (const Rect&)rc);
        }
        delete(pBitmap);
    }
    break;

    case IDC_MAIN_STATE_ICON:
    {
        CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
        bool bBmpLoaded = false;
        if (g_APPState.bConnected) {
            bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_STATE_OK), L"PNG", g_APPUI.hInst);
        }
        else {
            bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_STATE_WARNING), L"PNG", g_APPUI.hInst);
        }
        if (bBmpLoaded)
        {
            Gdiplus::Graphics graphics(hDc);
            graphics.SetSmoothingMode(SmoothingModeHighQuality);
            int iconWidth = nxrmUtil::RectWidth(rcBtn);
            int iconHeight = nxrmUtil::RectHeight(rcBtn);
            Rect rc(rcBtn.left, (iconHeight - iconWidth) / 2, iconWidth, iconWidth);
            graphics.DrawImage(*pBitmap, (const Rect&)rc);
        }
        delete(pBitmap);
    }
    break;

    case IDC_MAIN_POLICY_LABEL:
    {
        CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
        bool bBmpLoaded = false;
        bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_POLICY), L"PNG", g_APPUI.hInst);
        if (bBmpLoaded)
        {
            Gdiplus::Graphics graphics(hDc);
            graphics.SetSmoothingMode(SmoothingModeHighQuality);
            int iconWidth = nxrmUtil::RectWidth(rcBtn);
            int iconHeight = nxrmUtil::RectHeight(rcBtn)/2;
            Rect rc(rcBtn.left + iconHeight / 4, rcBtn.top + iconHeight / 2, iconHeight * 3 / 4, iconHeight );
            graphics.DrawImage(*pBitmap, (const Rect&)rc);
        }
        delete(pBitmap);
    }
    break;

    case IDC_MAIN_POLICY_REFRESH:
    {
        CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
        bool bBmpLoaded = false;
        bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_REFRESH), L"PNG", g_APPUI.hInst);
        if (bBmpLoaded)
        {
            iconWidth = g_DPI.Scale(ROTATE_ICON_WIDTH);
            Gdiplus::Graphics graphics(hDc);
            graphics.SetSmoothingMode(SmoothingModeHighQuality);
            int iconHeight = iconWidth;
            Rect rc((rcBtn.left + rcBtn.right) / 2 - iconWidth / 2, (rcBtn.top + rcBtn.bottom) / 2 - iconWidth / 2, iconWidth, iconWidth);
            graphics.DrawImage(*pBitmap, (const Rect&)rc);
        }
        delete(pBitmap);
    }
    break;

    case IDC_MAIN_BRAND_BMP:
        {
            CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
            if (pBitmap->Load(MAKEINTRESOURCE(IDB_BRANDPNG), L"PNG"), g_APPUI.hInst)
            {
                Gdiplus::Graphics graphics(hDc);
                graphics.SetSmoothingMode(SmoothingModeHighQuality);
                centerRectWidth = centerRectWidth * 9 / 10;
                centerRectHeight = nxrmUtil::RectHeight(rcBtn) * 6 / 10;
                Rect rc(0, nxrmUtil::RectHeight(rcBtn) / 5, centerRectWidth, centerRectHeight);
                graphics.DrawImage(*pBitmap, (const Rect&)rc);
            }
            delete(pBitmap);
        }

        break;

    case IDC_MAIN_SETTING:
        {
            CGdiPlusBitmapResource* pBitmap = new CGdiPlusBitmapResource;
            bool bBmpLoaded = false;
            bBmpLoaded = pBitmap->Load(MAKEINTRESOURCE(IDB_SETTING), L"PNG", g_APPUI.hInst);
            if (bBmpLoaded)
            {
                Gdiplus::Graphics graphics(hDc);
                graphics.SetSmoothingMode(SmoothingModeHighQuality);
                int iconWidth = nxrmUtil::RectWidth(rcBtn) * 3 / 5;
                int iconHeight = nxrmUtil::RectHeight(rcBtn);
                Rect rc(rcBtn.left + nxrmUtil::RectWidth(rcBtn) / 5, rcBtn.top + (iconHeight - iconWidth)/2, iconWidth, iconWidth);
                graphics.DrawImage(*pBitmap, (const Rect&)rc);
            }
            delete(pBitmap);
        }
        break;
    default:
        break;
    }
    
    return ;

}

// Description:
//   Creates a tooltip for an item in a dialog box. 
// Parameters:
//   iItemID - identifier of an dialog box item.
//   nDlg - window handle of the dialog box.
//   pszText - string to use as the tooltip text.
// Returns:
//   The handle to the tooltip.
HWND doCreateToolTip(HWND hDlg, int iItemID, LPWSTR pszText)
{
    HWND hwndTip;
    HWND hwndTool;

    if (!iItemID || !hDlg || !pszText)
    {
        return FALSE;
    }

    // Get the window of the tool.
    hwndTool = GetDlgItem(hDlg, iItemID);

    //TODO: Create the tooltip.
    hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                             WS_POPUP |TTS_ALWAYSTIP | TTS_NOANIMATE,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             hDlg, NULL, 
                             g_APPUI.hInst, NULL);

    if (!hwndTool || !hwndTip)
    {
        return (HWND)NULL;
    }                              

    // Associate the tooltip with the tool.
    TOOLINFO toolInfo = { 0 };
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.hwnd = hDlg;
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    toolInfo.uId = (UINT_PTR)hwndTool;
    toolInfo.lpszText = pszText;
    SendMessageW(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

    return hwndTip;
}



void doMoveMainDialog(void)
{
    int   iCnt;
    HWND  hWndItem;
    RECT  rcMainDlg;
    RECT  rcClient;
    SIZE  szClient;
    POINT ptNewClient;     //Right-Down corner's point of child window control
    LONG  lItemNewTop;
    int   iItemID[] = {IDC_MAIN_BRAND_BMP, IDC_MAIN_VERTICAL, IDC_MAIN_TITLE, IDC_MAIN_SETTING};

    //Change y coordinate of main dialog and setting bar's controls
    GetWindowRect(g_APPUI.hMainDlg, &rcMainDlg);
    GetWindowRect(GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_SETTING), &rcClient);   //Relative to Screen
    ptNewClient.x = rcClient.left;
    ptNewClient.y = rcClient.top;
    ScreenToClient(g_APPUI.hMainDlg, &ptNewClient);

    if(POPUP_SHOWING == g_iPopupStatus)
    {
        rcMainDlg.top = rcMainDlg.bottom - g_expandedHeight;
        ptNewClient.y += g_lPopupIncrement;
    }
    else if(POPUP_HIDING == g_iPopupStatus)
    {
        rcMainDlg.top = rcMainDlg.bottom - g_expandedHeight + g_lPopupIncrement;  
        ptNewClient.y -= g_lPopupIncrement;
    }
    else
    {
        return ;
    }
    lItemNewTop = ptNewClient.y;

    //Move main window
    szClient.cx   = rcMainDlg.right  - rcMainDlg.left;
    szClient.cy   = rcMainDlg.bottom - rcMainDlg.top;
    MoveWindow(g_APPUI.hMainDlg, rcMainDlg.left, rcMainDlg.top, szClient.cx, szClient.cy, TRUE);


    //Move child window control
    
    for(iCnt = 0; iCnt < sizeof(iItemID) / sizeof(iItemID[0]); iCnt++)
    {
        hWndItem = GetDlgItem(g_APPUI.hMainDlg, iItemID[iCnt]);
        GetWindowRect(hWndItem, &rcClient);   //Relative to Screen
        ptNewClient.x = rcClient.left;
        ScreenToClient(g_APPUI.hMainDlg, &ptNewClient);
        szClient.cx = rcClient.right  - rcClient.left;
        szClient.cy = rcClient.bottom - rcClient.top; 

        HWND hCtrl = GetDlgItem(g_APPUI.hMainDlg, iItemID[iCnt]);
        RECT ctrlRect = { ptNewClient.x, lItemNewTop, ptNewClient.x + szClient.cx, lItemNewTop + szClient.cy };
        MoveWindow(GetDlgItem(g_APPUI.hMainDlg, iItemID[iCnt]), ptNewClient.x, lItemNewTop, szClient.cx, szClient.cy, TRUE);
    }

    GetWindowRect(g_APPUI.hMainDlg, &rcMainDlg);
    InvalidateRect(g_APPUI.hMainDlg, &rcMainDlg, TRUE);
    return ;
}

void onShowingOrHidingPopup(void)
{
    RECT  rcPopupDlg;
    SIZE  szPopupDlg;
    POINT ptNewClient;

    GetWindowRect(g_APPUI.hMainPopupDlg, &rcPopupDlg);
    
    if(POPUP_SHOWING == g_iPopupStatus) {
        rcPopupDlg.top -= POPUP_INCREMENT;  //Move down to hide
    }
    else if(POPUP_HIDING == g_iPopupStatus) {
        rcPopupDlg.top += POPUP_INCREMENT;  //Move up to show
    }
    else {
        return ;
    }

    ptNewClient.x = rcPopupDlg.left;
    ptNewClient.y = rcPopupDlg.top;
    ScreenToClient(g_APPUI.hMainDlg, &ptNewClient);
    szPopupDlg.cx   = rcPopupDlg.right  - rcPopupDlg.left;
    szPopupDlg.cy   = rcPopupDlg.bottom - rcPopupDlg.top;
    MoveWindow(g_APPUI.hMainPopupDlg, ptNewClient.x, ptNewClient.y, szPopupDlg.cx, szPopupDlg.cy, TRUE);

    return ;
}

void onShowingOrHidingPopupStyle2(void)
{
    BOOL  bRes;
    int   iCnt;
    HWND  hWndItem;
    RECT  rcClient;
    RECT  rcMainDlg;
    //HRGN  hRgnMainDlg;
    SIZE  szClient;
    POINT ptNewClient;     //Right-Down corner's point of child window control
    HDWP  hWinPosInfo;
    int   iItemID[] = {IDC_MAIN_BRAND_BMP, IDC_MAIN_VERTICAL, IDC_MAIN_TITLE, IDC_MAIN_SETTING};

    //Move main window
    GetWindowRect(g_APPUI.hMainDlg, &rcClient);
    if(POPUP_SHOW == g_iPopupStatus || POPUP_SHOWING == g_iPopupStatus)
    {
        rcClient.top -= POPUP_INCREMENT; //POPUP_HEIGHT / 40;     //Move down to hide
    }
    else if(POPUP_HIDE == g_iPopupStatus || POPUP_HIDING == g_iPopupStatus)
    {
        rcClient.top += POPUP_INCREMENT; //POPUP_HEIGHT / 40;     //Move up to show
    }

    szClient.cx   = rcClient.right  - rcClient.left;
    szClient.cy   = rcClient.bottom - rcClient.top;
    MoveWindow(g_APPUI.hMainDlg, rcClient.left, rcClient.top, szClient.cx, szClient.cy, TRUE);

    //Move child window control
    hWinPosInfo = BeginDeferWindowPos(sizeof(iItemID) / sizeof(iItemID[0]));   //Initial
    if(NULL == hWinPosInfo)
    {
        MessageBoxW(NULL, L"Initial HDWP failed!", L"ERROR", MB_OK | MB_ICONERROR);
    }
    
    GetClientRect(g_APPUI.hMainDlg, &rcMainDlg);
    for(iCnt = 0; iCnt < sizeof(iItemID) / sizeof(iItemID[0]); iCnt++)
    {
        hWndItem = GetDlgItem(g_APPUI.hMainDlg, iItemID[iCnt]);
        GetWindowRect(hWndItem, &rcClient);   //Relative to Screen
        ptNewClient.x = rcClient.left;
        ptNewClient.y = rcClient.top;
        ScreenToClient(g_APPUI.hMainDlg, &ptNewClient);

        GetWindowRect(GetDlgItem(g_APPUI.hMainDlg, iItemID[iCnt]), &rcClient);  //Relative to Main dialog
        szClient.cx = rcClient.right  - rcClient.left;
        szClient.cy = rcClient.bottom - rcClient.top; 

        if(POPUP_SHOW == g_iPopupStatus || POPUP_SHOWING == g_iPopupStatus)
        {
            ptNewClient.y += POPUP_INCREMENT; // POPUP_HEIGHT / 40;     //Move Setting bar up
        }
        else if(POPUP_HIDE == g_iPopupStatus || POPUP_HIDING == g_iPopupStatus)
        {
            ptNewClient.y -= POPUP_INCREMENT; //POPUP_HEIGHT / 40;     //Move Setting bar down
        }
        
        hWinPosInfo = DeferWindowPos(hWinPosInfo, hWndItem, HWND_TOP, 
                                     ptNewClient.x, ptNewClient.y, szClient.cx, szClient.cy, 
                                     SWP_NOSIZE | SWP_NOREDRAW | SWP_SHOWWINDOW);
        if(NULL == hWinPosInfo)
        {
            MessageBoxW(NULL, L"Add HDWP failed!", L"ERROR", MB_OK | MB_ICONERROR);
        }
    }
    bRes = EndDeferWindowPos(hWinPosInfo);
    if(NULL == bRes)
    {
        MessageBoxW(NULL, L"EndDeferWindowPos failed!", L"ERROR", MB_OK | MB_ICONERROR);
    }
    
    return ;
}

//processing function when click refresh button
void onBtnRefreshClicked(void)
{
    EngineUpdatePolicy();
}

//processing function when click setting button
void onBtnSettingClicked(void)
{
    switch(g_iPopupStatus)
    {
    case POPUP_SHOW:
        g_iPopupStatus = POPUP_HIDING;
        g_bMouseOverButton = FALSE;
        ::AnimateWindow(g_APPUI.hMainPopupDlg, 1000, AW_VER_POSITIVE | AW_SLIDE | AW_HIDE);
        doMoveMainDialog();
        g_iPopupStatus = POPUP_HIDE;
        ShowWindow(g_APPUI.hMainPopupDlg, SW_HIDE);
        break;

    case POPUP_SHOWING:
        ; // Do nothing
        break;

    case POPUP_HIDE:
        g_iPopupStatus = POPUP_SHOWING;
        g_bMouseOverButton = FALSE;
        doMoveMainDialog();
        ::AnimateWindow(g_APPUI.hMainPopupDlg, 1000, AW_VER_NEGATIVE | AW_SLIDE | AW_ACTIVATE);
        g_iPopupStatus = POPUP_SHOW;
        break;

    case POPUP_HIDING:
        ; // Do nothing
        break;

    default:
        break;
    }
}

HMODULE LoadResourceModule()
{
    HMODULE hMod = NULL;
    WCHAR   wzPath[MAX_PATH] = {0};
    std::wstring wsMod1;
    std::wstring wsMod2;

    //TODO: Load Resource DLL
    if(!GetModuleFileNameW(NULL, wzPath, MAX_PATH)) {
        return NULL;
    }

    WCHAR* pos = wcsrchr(wzPath, L'\\');

    if(NULL == pos) {
        return ::LoadLibraryW(RESFILENAME);
    }

    *pos = L'\0';
    wsMod1 = wzPath;
    wsMod1 += L"\\";
    wsMod1 += RESFILENAME;

    pos = wcsrchr(wzPath, L'\\');
    if(NULL != pos) {
        *(pos+1) = L'\0';
        wsMod2 = wzPath;
        wsMod2 += RESFILENAME2;
    }

    hMod = ::LoadLibraryW(wsMod1.c_str());
    if(NULL == hMod && !wsMod2.empty()) {
        hMod = ::LoadLibraryW(wsMod2.c_str());
    }

    return hMod;
}

void QuitMain()
{
    ::PostMessageW(g_APPUI.hMainDlg, WM_DESTROY, 0, 0);
}

void StopRotatingUpdateIndicator()
{
    
    KillTimer(g_APPUI.hMainDlg, (UINT_PTR)ROTATE_REFRESH_TIMER);
    HWND hbtnWnd = GetDlgItem(g_APPUI.hMainDlg, IDC_MAIN_POLICY_REFRESH);
    InvalidateRect(hbtnWnd, NULL, TRUE); //Force to repaint after rotate
    g_updateIconRotate = NX_ICON_RT_STATE_STOPPED;

}

void SetIconStopRotatingFlag()
{
    if (g_updateIconRotate == NX_ICON_RT_STATE_ROTATING)
    {
        g_updateIconRotate = NX_ICON_RT_STATE_STOPPING;
    }
}

BOOL IsDwmCompositionEnabled()
{
    BOOL enabled = FALSE;
    HRESULT hr = DwmIsCompositionEnabled(&enabled);
    if (FAILED(hr)) {
        enabled = FALSE;
    }
    return enabled;
}

VOID UpdateDwmCompositionStatus()
{
    bool enabled = IsDwmCompositionEnabled() ? true : false;
    std::wstring errmsg;
    if (!NXSERV::set_dwm_status(enabled, errmsg)) {
        if (!errmsg.empty()) {
            doShowTrayMsg(errmsg.data());
        }
    }
}

static void InsertLogonMenu(bool logged_on)
{
    MENUITEMINFO    mii = { 0 };
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    BOOL exist = GetMenuItemInfoW(g_APPUI.hTrayMenu, TRAY_MENU_LOGINOUT_ID, FALSE, &mii);
    if (exist) {
        return;
    }

    std::wstring wsMenuText =  NX::RES::LoadMessage(g_APPUI.hResDll, logged_on ? IDS_TRAY_MENU_LOGOUT : IDS_TRAY_MENU_LOGIN, 1024);
    if (wsMenuText.empty()) {
        wsMenuText = logged_on ? L"Logout" : L"Log in ...";
    }
    AppendMenuW(g_APPUI.hTrayMenu, MF_STRING, TRAY_MENU_LOGINOUT_ID, wsMenuText.c_str());
    InsertMenuW(g_APPUI.hTrayMenu, 5, MF_SEPARATOR, NULL, NULL);
    //int nCount = GetMenuItemCount(g_APPUI.hTrayMenu);
    //InsertMenuW(g_APPUI.hTrayMenu, nCount - 1, MF_SEPARATOR | MF_BYPOSITION, NULL, NULL);
}

static void UpdateLogonMenu(bool logged_on)
{
    std::wstring wsMenuText =  NX::RES::LoadMessage(g_APPUI.hResDll, logged_on ? IDS_TRAY_MENU_LOGOUT : IDS_TRAY_MENU_LOGIN, 1024);
    if (wsMenuText.empty()) {
        wsMenuText = logged_on ? L"Logout" : L"Log in ...";
    }
    MENUITEMINFO    mii = { 0 };
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    BOOL exist = GetMenuItemInfoW(g_APPUI.hTrayMenu, TRAY_MENU_LOGINOUT_ID, FALSE, &mii);
    if (exist) {
        ModifyMenuW(g_APPUI.hTrayMenu, TRAY_MENU_LOGINOUT_ID, MF_STRING | MF_BYCOMMAND, TRAY_MENU_LOGINOUT_ID, wsMenuText.c_str());
    }
    else {
        AppendMenuW(g_APPUI.hTrayMenu, MF_STRING, TRAY_MENU_LOGINOUT_ID, wsMenuText.c_str());
        InsertMenuW(g_APPUI.hTrayMenu, 5, MF_SEPARATOR, NULL, NULL);
        //int nCount = GetMenuItemCount(g_APPUI.hTrayMenu);
        //InsertMenuW(g_APPUI.hTrayMenu, nCount - 1, MF_SEPARATOR | MF_BYPOSITION, NULL, NULL);
    }
}

static void RemoveLogonMenu()
{
    MENUITEMINFO    mii = { 0 };
    memset(&mii, 0, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    BOOL exist = GetMenuItemInfoW(g_APPUI.hTrayMenu, TRAY_MENU_LOGINOUT_ID, FALSE, &mii);
    if (!exist) {
        return;
    }
    DeleteMenu(g_APPUI.hTrayMenu, TRAY_MENU_LOGINOUT_ID, MF_BYCOMMAND);
    DeleteMenu(g_APPUI.hTrayMenu, 5, MF_BYPOSITION);
}

VOID UpdateTrayMenu(bool debug_on, bool external_authn, const std::wstring& logon_user)
{
    if (NULL == g_APPUI.hTrayMenu) {
        return;
    }

    if (g_APPState.bIsDebugOn != debug_on) {
        g_APPState.bIsDebugOn = debug_on;
        std::wstring wsMenuText = NX::RES::LoadMessageEx(g_APPUI.hResDll, g_APPState.bIsDebugOn ? IDS_TRAY_MENU_DISABLE_DEBUG : IDS_TRAY_MENU_ENABLE_DEBUG, 1024, g_APPState.dwLangId, g_APPState.bIsDebugOn ? L"Disable debug" : L"Enable debug");
        ModifyMenuW(g_APPUI.hTrayMenu, TRAY_MENU_ENABLE_DEBUG_ID, MF_STRING | MF_BYCOMMAND, TRAY_MENU_ENABLE_DEBUG_ID, wsMenuText.c_str());
    }

    if (g_APPState.bRemoteAuthn != external_authn) {
        g_APPState.bRemoteAuthn = external_authn;
        if (g_APPState.bRemoteAuthn) {
            // from local to remote logon
            // insert menu
            bool logged_on = (!logon_user.empty() && 0 != _wcsicmp(logon_user.c_str(), L"N/A"));
            g_APPState.bLoggedOn = logged_on;
            UpdateLogonMenu(logged_on);
        }
        else {
            // from remote to local logon
            // remove menu
            RemoveLogonMenu();
        }
    }
    else {
        if (g_APPState.bRemoteAuthn) {
            bool logged_on = (!logon_user.empty() && 0 != _wcsicmp(logon_user.c_str(), L"N/A"));
            if (g_APPState.bLoggedOn != logged_on) {
                g_APPState.bLoggedOn = logged_on;
                UpdateLogonMenu(logged_on);
                if (!logged_on) {
                    // Let user know that his/her authn expired
                    std::wstring msg = NX::RES::LoadMessage(g_APPUI.hResDll, IDS_NOTIFY_LOGON_EXPIRED, 1024, LANG_NEUTRAL, L"Current session expired, please logon again.");
                    doShowTrayMsg(msg.c_str());
                }
            }
        }
    }
}

VOID ProtectOfficeAddinKeys()
{
#ifdef _AMD64_
    ::RegDeleteKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\Word\\Addins\\NxRMAddin", KEY_WOW64_32KEY, 0);
    ::RegDeleteKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\Excel\\Addins\\NxRMAddin", KEY_WOW64_32KEY, 0);
    ::RegDeleteKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\PowerPoint\\Addins\\NxRMAddin", KEY_WOW64_32KEY, 0);
    ::RegDeleteKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\Word\\Addins\\NxRMAddin", KEY_WOW64_64KEY, 0);
    ::RegDeleteKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\Excel\\Addins\\NxRMAddin", KEY_WOW64_64KEY, 0);
    ::RegDeleteKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\PowerPoint\\Addins\\NxRMAddin", KEY_WOW64_64KEY, 0);
#else
    ::RegDeleteKeyW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\Word\\Addins\\NxRMAddin");
    ::RegDeleteKeyW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\Excel\\Addins\\NxRMAddin");
    ::RegDeleteKeyW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Office\\PowerPoint\\Addins\\NxRMAddin");
#endif
}

bool IsStringValidJson(std::string jsonStr, int &openCnt, int &closeCnt)
{
    int openCurlyCount = 0;
    int closeCurlyCount = 0;
    for (size_t i = 0; i < jsonStr.size(); i++)
    {
        auto c = jsonStr.at(i);
        if ( c == '{') openCurlyCount++;
        if ( c == '}') closeCurlyCount++;
    }
    openCnt = openCurlyCount;
    closeCnt = closeCurlyCount;
    return openCurlyCount == closeCurlyCount && openCurlyCount != 0;
}

static BOOL __stdcall nxrmtrayInternetReadFile(
    _In_ HINTERNET hFile,
    _Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID lpBuffer,
    _In_ DWORD dwNumberOfBytesToRead,
    _Out_ LPDWORD lpdwNumberOfBytesRead
)
{
    BOOL bRet = TRUE;

    WCHAR RequestMethod[16] = { 0 };
    DWORD dwBufferLength = 0;
    DWORD dwIndex = 0;
    WCHAR ContentType[128] = { 0 };

    UCHAR *pResponse = NULL;

    do
    {
        bRet = g_APPUI.fn_InternetReadFile_trampoline(hFile, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);

        if (!bRet || g_APPUI.hHostingWnd == NULL)
        {
            break;
        }

        dwBufferLength = sizeof(RequestMethod) - sizeof(WCHAR);

        if (!HttpQueryInfoW(hFile,
                            HTTP_QUERY_REQUEST_METHOD,
                            (LPVOID)RequestMethod,
                            &dwBufferLength,
                            &dwIndex))
        {
            break;
        }

        if (_wcsicmp(RequestMethod, L"POST") != 0)
        {
            break;
        }

        dwBufferLength = sizeof(ContentType) - sizeof(WCHAR);
        dwIndex = 0;

        if (!HttpQueryInfoW(hFile,
                            HTTP_QUERY_CONTENT_TYPE,
                            (LPVOID)ContentType,
                            &dwBufferLength,
                            &dwIndex))
        {
            break;
        }
        
        if (_wcsicmp(ContentType, L"application/json") != 0)
        {
            break;
        }


        pResponse = (UCHAR*)malloc(*lpdwNumberOfBytesRead + 1);

        if (!pResponse)
        {
            break;
        }

        memset(pResponse, 0, *lpdwNumberOfBytesRead + 1);

        memcpy(pResponse, lpBuffer, *lpdwNumberOfBytesRead);

        int openCnt, closeCnt;
        if (!IsStringValidJson((CHAR *)pResponse,openCnt,closeCnt))
        {
            static int nOpenCount = 0;
            static int nCloseCount = 0;

            static UCHAR *pCollectData = NULL;
            static int currSize = 0;
            if(!currSize)
            { 
                pCollectData = (UCHAR*)malloc(*lpdwNumberOfBytesRead);
                currSize += *lpdwNumberOfBytesRead;
                memcpy(pCollectData, lpBuffer, *lpdwNumberOfBytesRead);
            }
            else
            {
                auto p = realloc(pCollectData, currSize + *lpdwNumberOfBytesRead);
                if (p)
                {
                    pCollectData = (UCHAR *)p;
                    memcpy(pCollectData + currSize, lpBuffer, *lpdwNumberOfBytesRead);
                    currSize += *lpdwNumberOfBytesRead;
                }
            }

            free(pResponse);
            pResponse = NULL;

            nOpenCount += openCnt;
            nCloseCount += closeCnt;
            if (nOpenCount == nCloseCount)
            {

                nOpenCount = nCloseCount = 0;

                pCollectData = (UCHAR *)realloc(pCollectData, currSize + 1);
                *(pCollectData + currSize) = 0;             //Adding NULL Terminator here
                auto pData = std::move(pCollectData);
                if (!::PostMessageW(g_APPUI.hHostingWnd, WM_WEBPAGE_URL_SUCCEEDED, 0, (LPARAM)pData))
                {
                    free(pData);
                    pData = NULL;
                }
                pCollectData = NULL;
                currSize = 0;
            }

        }
        else if (!::PostMessageW(g_APPUI.hHostingWnd, WM_WEBPAGE_URL_SUCCEEDED, 0, (LPARAM)pResponse))
        {
            free(pResponse);
            pResponse = NULL;
        }

    } while (FALSE);

    return bRet;
}

static BOOL WINAPI nxrmtrayCreateProcessW(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
	WCHAR UserProfilePath[MAX_PATH] = { 0 };

	HRESULT hr = S_OK;

	BOOL bRet = TRUE;

	do 
	{
		if (!lpEnvironment)
		{
			break;
		}

		hr = SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, UserProfilePath);

		if (hr != S_OK)
		{
			break;
		}

		std::wstring UserProfile = UserProfilePath;

		std::map<std::wstring, std::wstring> env_variavles = get_environment_variables((const wchar_t*)lpEnvironment);

		env_variavles[L"APPDATA"] = UserProfile + L"\\AppData\\Roaming";
		env_variavles[L"LOCALAPPDATA"] = UserProfile + L"\\AppData\\Local";
		env_variavles[L"USERPROFILE"] = UserProfile;

		const std::vector<wchar_t>& env_block = create_environment_block(env_variavles);


		bRet = g_APPUI.fn_CreateProcessW_trampoline(lpApplicationName,
													lpCommandLine,
													lpProcessAttributes,
													lpThreadAttributes,
													bInheritHandles,
													dwCreationFlags,
													(LPVOID)(env_block.empty() ? NULL : env_block.data()),
													lpCurrentDirectory,
													lpStartupInfo,
													lpProcessInformation);

		return bRet;

	} while (FALSE);
	
	return g_APPUI.fn_CreateProcessW_trampoline(lpApplicationName,
												lpCommandLine,
												lpProcessAttributes,
												lpThreadAttributes,
												bInheritHandles,
												dwCreationFlags,
												lpEnvironment,
												lpCurrentDirectory,
												lpStartupInfo,
												lpProcessInformation);
}

static std::map<std::wstring, std::wstring> get_environment_variables(const wchar_t* sys_envs)
{
	std::map<std::wstring, std::wstring> variables;

	if (NULL == sys_envs) {
		return variables;
	}

	while (sys_envs[0] != L'\0') {
		std::wstring line(sys_envs);
		sys_envs += (line.length() + 1);
		auto pos = line.find(L'=');
		std::wstring var_name;
		std::wstring var_value;
		if (pos != std::wstring::npos) {
			var_name = line.substr(0, pos);
			var_value = line.substr(pos + 1);
		}
		else {
			var_name = line;
		}
		if (0 == _wcsicmp(var_name.c_str(), L"APPDATA")
			|| 0 == _wcsicmp(var_name.c_str(), L"LOCALAPPDATA")
			|| 0 == _wcsicmp(var_name.c_str(), L"USERPROFILE")
			|| 0 == _wcsicmp(var_name.c_str(), L"USERNAME")
			) {
			std::transform(var_name.begin(), var_name.end(), var_name.begin(), toupper);
		}
		variables[var_name] = var_value;
	}

	return std::move(variables);
}

static std::vector<wchar_t> create_environment_block(const std::map<std::wstring, std::wstring>& variables)
{
	std::vector<wchar_t> env_block;

	std::for_each(variables.begin(), variables.end(), [&](const std::pair<std::wstring, std::wstring>& var) {
		std::wstring line = var.first;
		line += L"=";
		line += var.second;
		std::for_each(line.begin(), line.end(), [&](const wchar_t& c) {
			env_block.push_back(c);
		});
		env_block.push_back(L'\0');
	});
	env_block.push_back(L'\0');

	return std::move(env_block);
}