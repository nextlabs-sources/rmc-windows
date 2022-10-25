#pragma once

#include "resource.h"
#include "windows.h"
#include "Wininet.h"
#include "shellapi.h"
#include <string>


#define WM_NX_RESIZE_MAIN	WM_APP + 100
#define WM_NX_LOGON_END		WM_APP + 101
#define WM_NX_OPEN_PROJECT  WM_APP + 102
#define WM_NX_REFRESH_PROJ_BY_OTHERS        WM_APP + 103
#define WM_NX_PREV_PROJECT       WM_APP + 105
#define WM_NX_NEXT_PROJECT       WM_APP + 106
#define WM_NX_REFRESH_PROJFILELIST       WM_APP + 107

#define WM_NX_LAUNCH_VIEWER       WM_APP + 108


#define NX_LOGON_DLG	1
#define NX_MAIN_DLG		2

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
int RmcMain(_In_ HINSTANCE hInstance);

typedef BOOL(__stdcall *INTERNETREADFILE)(
    _In_ HINTERNET hFile,
    _Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID lpBuffer,
    _In_ DWORD dwNumberOfBytesToRead,
    _Out_ LPDWORD lpdwNumberOfBytesRead
    );

typedef BOOL(__stdcall * INTERNETREADFILEEX)(
	_In_  HINTERNET          hFile,
	_Out_ LPINTERNET_BUFFERS lpBuffersOut,
	_In_  DWORD              dwFlags,
	_In_  DWORD_PTR          dwContext
	);

typedef BOOL(WINAPI *CREATEPROCESSW)(
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

CSize GetNonClientSize(HWND hWnd);


typedef struct tagAPPUI {
    HWND           hMainWnd;                      // Handle of main Dialog
    NOTIFYICONDATA nid;                           // Tray icon
    HMENU          hTrayMenu;                     // Tray Right click Menu
    HINSTANCE      hInst;                         // Handle of current instance
    //HWND           hTab;                          // Handle of Tab control
    //HWND           hMainPopupDlg;                 // Handle of Tabs Dialog in Main Dialog
    //HWND		   hHtmlNotifyDlg;				// Handle of Notify Dlg for Customized HTML info	NEW ADDED 0518
    HMODULE        hResDll;                       // HMODULE of nxrmres.dll
    RECT           rcPopupDlg;                    // Store the RECT of pop up dialog, relative to MainDialog's client

    HWND           hUserNameTip;                  // user name is too long we'll setup a tip there, it will be destroyed when main window is closed
    HWND           hPolicyNameTip;                // Policy is too long we'll setup tooltip there, it will be destroyed when main window is closed

                                                  //
                                                  // hook related
    HMODULE				hWininet;
    INTERNETREADFILE	fn_org_InternetReadFile;
    INTERNETREADFILE	fn_InternetReadFile_trampoline;

	INTERNETREADFILEEX fn_org_InternetReadFileEx;
	INTERNETREADFILEEX fn_InternetReadFileEx_trampoline;

    CREATEPROCESSW		fn_org_CreateProcessW;
    CREATEPROCESSW		fn_CreateProcessW_trampoline;
    std::wstring        szLogonTitle;
    HWND				hHostingWnd;

} APPUI, *PAPPUI;

extern APPUI      g_APPUI;
