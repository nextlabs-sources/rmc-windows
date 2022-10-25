// nxrmc.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <VersionHelpers.h>
#include <minwindef.h>
#include "nxrmc.h"
#include "nxrmRMC.h"
#include <thread>
#include "weblogondlg.hpp"
#include "..\detour\detour.h"
#include "nxrmRMC.h"
#include "commondlg.hpp"
#include "ProfPage.h"
#include "ConnectToRepo.h"
#include "nxrmRMC.h"
#include "nxmsgIds.h"
#include <nx\winutil\path.h>
#include <nx\nxl\nxlrights.h>
#include <mutex>
#include <future>
#include <string.h>
#include "aboutbox.h"


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



APPUI g_APPUI;
NXSharingData g_dataOut;

//extern TrayIcon trayIcon;

//using namespace uiUtilities;

// Global Variables:
//HINSTANCE hInst;                                // current instance
WCHAR szWindowClass[MAX_LOADSTRING];            // the n window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SplashProc(HWND, UINT, WPARAM, LPARAM);

void DataToRightsAndObs(const NXSharingData &dataOut, std::vector<std::wstring>& rights, std::vector<std::wstring>& obs);
void RightsAndObsToData(const std::wstring& rights, NXSharingData &dataIn);

extern std::wstring OutputError(NX::Result &res, wstring errMsg);

void InitScrollBars(HWND hWnd, CRect &rc);

//void LoadData()
//{
//    Sleep(2000);
//    CloseProgressWindow();
//}

#define REPOSITORY_MENU_LIST_ID_BASE	50000
#define PROJECT_MENU_LIST_ID_BASE		51000
#define REPOSITORIES_SUBMEN_INDEX		9
#define SORTBY_SUBMEN_INDEX				5
#define GOTOPROJECT_SUBMENU_INDEX		0

int RmcMain(_In_ HINSTANCE hInstance)
{
    g_APPUI.hInst = hInstance; // Store instance handle in our global variable
    // TODO: Place code here.
    LoadStringW(hInstance, IDS_APP_TITLE, theApp.szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NXRMC, szWindowClass, MAX_LOADSTRING);
    // Initialize global strings
    MyRegisterClass(hInstance);
    //DialogBox(hInstance, MAKEINTRESOURCE(IDD_SPLASH), NULL, SplashProc);

    //DestroyWindow(hwnd);

    // Perform application initialization:
    if (!InitInstance (hInstance, SW_SHOW))
    {
        return FALSE;
    }


    return 0;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

//    wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.style = 0;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NXRMC));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    //wcex.lpszMenuName = NULL;

    wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_NXRMC_SHORT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_NXRMC));

    return RegisterClassExW(&wcex);
}

bool IsStringValidJsonReturn(std::string jsonStr, int &openCnt, int &closeCnt)
{
	
	int openCurlyCount = 0;
    int closeCurlyCount = 0;
    for (size_t i = 0; i < jsonStr.size(); i++)
    {
        auto c = jsonStr.at(i);
        if (c == '{') openCurlyCount++;
        if (c == '}') closeCurlyCount++;
    }
    openCnt = openCurlyCount;
    closeCnt = closeCurlyCount;
	if (jsonStr.find("\"statusCode\"") == std::string::npos && jsonStr.find("\"message\"") == std::string::npos)//check if this is skydrm returned
		return false;

    return openCurlyCount == closeCurlyCount && openCurlyCount != 0;
}

void CheckReadFileBuffer(_In_  HINTERNET hFile, _In_ LPVOID lpbuffer, _In_ DWORD bufflen)
{

	DWORD dwIndex = 0;
	WCHAR ContentType[128] = { 0 };

	WCHAR RequestMethod[16] = { 0 };
	DWORD dwBufferLength = 0;
	UCHAR *pResponse = NULL;

	dwBufferLength = sizeof(RequestMethod) - sizeof(WCHAR);

	if (!HttpQueryInfoW(hFile,
		HTTP_QUERY_REQUEST_METHOD,
		(LPVOID)RequestMethod,
		&dwBufferLength,
		&dwIndex))
	{
		return;
	}

	if (_wcsicmp(RequestMethod, L"POST") != 0 && _wcsicmp(RequestMethod, L"GET") != 0)
	{
		return;
	}

	dwBufferLength = sizeof(ContentType) - sizeof(WCHAR);
	dwIndex = 0;

	if (!HttpQueryInfoW(hFile,
		HTTP_QUERY_CONTENT_TYPE,
		(LPVOID)ContentType,
		&dwBufferLength,
		&dwIndex))
	{
		return;
	}
	std::wstring strcontent = ContentType;

	if (strcontent.find(L"application/json") == std::wstring::npos) {
		return;
	}


	pResponse = (UCHAR*)malloc(bufflen + 1);

	if (!pResponse)
	{
		return;
	}

	memset(pResponse, 0, bufflen + 1);

	memcpy(pResponse, lpbuffer, bufflen);
	static UCHAR *pCollectData = NULL;

	int openCnt, closeCnt;
	if (!IsStringValidJsonReturn((CHAR *)pResponse, openCnt, closeCnt) || pCollectData)
	{
		static int nOpenCount = 0;
		static int nCloseCount = 0;

		if (openCnt <= closeCnt && pCollectData == NULL) {//valid Json, but not return by skydrm
			free(pResponse);
			pResponse = NULL;
			return;
		}
		static int currSize = 0;
		if (!currSize)
		{
			pCollectData = (UCHAR*)malloc(bufflen);
			currSize += bufflen;
			memcpy(pCollectData, lpbuffer, bufflen);
		}
		else
		{
			auto p = realloc(pCollectData, currSize + bufflen);
			if (p)
			{
				pCollectData = (UCHAR *)p;
				memcpy(pCollectData + currSize, lpbuffer, bufflen);
				currSize += bufflen;
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
			auto pData = pCollectData;
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

}

static BOOL __stdcall nxrmtrayInternetReadFileEx(
	_In_  HINTERNET          hFile,
	_Out_ LPINTERNET_BUFFERS lpBuffersOut,
	_In_  DWORD              dwFlags,
	_In_  DWORD_PTR          dwContext
)
{
	BOOL bRet = TRUE;

	WCHAR RequestMethod[16] = { 0 };
	DWORD dwBufferLength = 0;
	DWORD dwIndex = 0;
	WCHAR ContentType[128] = { 0 };

	UCHAR *pResponse = NULL;

	do{
		bRet = g_APPUI.fn_InternetReadFileEx_trampoline(hFile, lpBuffersOut, dwFlags, dwContext);

		if (!bRet || g_APPUI.hHostingWnd == NULL)
		{
			break;
		}
		CheckReadFileBuffer(hFile, lpBuffersOut->lpvBuffer, lpBuffersOut->dwBufferLength);
	} while (FALSE);

	return bRet;
}

static BOOL __stdcall nxrmtrayInternetReadFile(
    _In_ HINTERNET hFile,
    _Out_writes_bytes_(dwNumberOfBytesToRead) __out_data_source(NETWORK) LPVOID lpBuffer,
    _In_ DWORD dwNumberOfBytesToRead,
    _Out_ LPDWORD lpdwNumberOfBytesRead
)
{
    BOOL bRet = TRUE;



    do
    {
        bRet = g_APPUI.fn_InternetReadFile_trampoline(hFile, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);

        if (!bRet || g_APPUI.hHostingWnd == NULL)
        {
            break;
        }

		CheckReadFileBuffer(hFile, lpBuffer, *lpdwNumberOfBytesRead);

    } while (FALSE);

    return bRet;
}

void ShowProjIntroWindow(HWND hParentWnd, int width, int height)
{
    theApp.m_dlgProjIntro.ResizeWindow(width, height);
    CRect rectIntro;
    theApp.m_dlgProjIntro.GetWindowRect(&rectIntro);
    CWnd::FromHandle(hParentWnd)->ScreenToClient(&rectIntro);
    InitScrollBars(hParentWnd, rectIntro);
}

void ShowWelcomeIntroWindow(HWND hParentWnd, int width, int height)
{
    theApp.m_dlgIntro.ResizeWindow(width, height);
    CRect rectIntro;
    theApp.m_dlgIntro.GetWindowRect(&rectIntro);
    CWnd::FromHandle(hParentWnd)->ScreenToClient(&rectIntro);
    InitScrollBars(hParentWnd, rectIntro);
}


void ResizeProjPage(HWND hParentWnd, int width, int height)
{
    theApp.m_dlgProjPage.ResizeWindow(width, height);
    CRect rectProj;
    theApp.m_dlgProjPage.GetWindowRect(&rectProj);
    CWnd::FromHandle(hParentWnd)->ScreenToClient(&rectProj);
    InitScrollBars(hParentWnd, rectProj);
}


void ResizeProjMainPage(HWND hParentWnd, int width, int height)
{
    //theApp.m_dlgProjPage.ResizeWindow(width, height);
    theApp.m_dlgProjMain.ResizeWindow(width, height);
    CRect rectProj;
    theApp.m_dlgProjMain.GetWindowRect(&rectProj);
    CWnd::FromHandle(hParentWnd)->ScreenToClient(&rectProj);
    InitScrollBars(hParentWnd, rectProj);
}

//void ResizeProjMembersPage(HWND hParentWnd, int width, int height)
//{
//    //theApp.m_dlgProjPage.ResizeWindow(width, height);
//    theApp.m_dlgProjMain.ResizeWindow(width, height);
//    CRect rectProj;
//    theApp.m_dlgProjMain.GetWindowRect(&rectProj);
//    CWnd::FromHandle(hParentWnd)->ScreenToClient(&rectProj);
//    InitScrollBars(hParentWnd, rectProj);
//}


void ResizeRepoPage(HWND hParentWnd, int width, int height)
{
    //theApp.m_dlgProjPage.ResizeWindow(width, height);
    theApp.m_dlgRepoPage.ResizeWindow(width, height);
    CRect rectProj;
    theApp.m_dlgRepoPage.GetWindowRect(&rectProj);
    CWnd::FromHandle(hParentWnd)->ScreenToClient(&rectProj);
    InitScrollBars(hParentWnd, rectProj);
}




//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   HWND hWnd = CreateWindowW(szWindowClass, theApp.szTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);


   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_APPUI.hWininet = GetModuleHandleW(L"wininet.dll");

   if (g_APPUI.hWininet)
   {
       g_APPUI.fn_org_InternetReadFile = (INTERNETREADFILE)GetProcAddress(g_APPUI.hWininet, "InternetReadFile");

       if (g_APPUI.fn_org_InternetReadFile)
       {
           install_hook(g_APPUI.fn_org_InternetReadFile, (PVOID*)&g_APPUI.fn_InternetReadFile_trampoline, nxrmtrayInternetReadFile);
       }

	   g_APPUI.fn_org_InternetReadFileEx = (INTERNETREADFILEEX)GetProcAddress(g_APPUI.hWininet, "InternetReadFileExW");

	   if (g_APPUI.fn_org_InternetReadFileEx) {
		   install_hook(g_APPUI.fn_org_InternetReadFileEx, (PVOID*)&g_APPUI.fn_InternetReadFileEx_trampoline, nxrmtrayInternetReadFileEx);
	   }
   }


   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

void DisplayMyVaultInfo(const NX::RmFileShareResult& shareResult, BOOL bshare)
{
    wstringstream txtOut;
    if (bshare)
        txtOut << theApp.LoadStringW(IDS_FILE_ADDED_TO_VAULT).GetBuffer(); // "The shared file has been added to MyVault.\n";
	else
		txtOut << theApp.LoadStringW(IDS_FILE_PROTECTED_IN_VAULT).GetBuffer();    //"The file is protected in MyVault.\n";
    CString vaultPath = shareResult.GetName().c_str();
    int pos = vaultPath.ReverseFind(L'/');
    if (pos != -1)
    {
        vaultPath = vaultPath.Right(vaultPath.GetLength() - pos - 1);
    }
    txtOut << vaultPath.GetBuffer();
    vaultPath = txtOut.str().c_str();
    theApp.ShowTrayMsg(vaultPath);
}

bool g_blogout = FALSE;

void LocalFileShare(const std::wstring& source, const std::wstring& membershipId, const std::vector<std::wstring>& rights, const std::vector<std::wstring>& obs, const std::vector<std::wstring>& recipients)
{
    NX::RmFileShareResult sr;
    NX::time::datetime dt = NX::time::datetime::current_time();
    const NX::time::span& days = NX::time::span::days_span(180);
    dt.add(days);
	theApp.ShowTrayMsg(L"Sharing the file.");
    NX::Result res = theApp.m_pSession->LocalFileShare(source, membershipId, NX::NXL::FileMeta(), NX::NXL::FileTags(), recipients, rights, obs, dt.to_java_time(), sr, &g_blogout);
    if (!res) {
		if (!g_blogout) {//show message only when user is still login
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAIL_SHARE_FILE).GetBuffer() /*"Failed to share file."*/).c_str());
		}
        return;
    }
    DisplayMyVaultInfo(sr, TRUE);
	theApp.RefreshMyVaultInfo();
}


void LocalFileProtect(const std::wstring& source, const std::vector<std::wstring> &rights, const std::vector<std::wstring> &obs)
{
	NX::NXL::FileMeta metadata;
	NX::NXL::FileTags tags;
	NX::NXL::FileSecret secret;

	wstring target;
	NX::win::FilePath path(source);
	if (NX::iend_with<wchar_t>(path.GetFileName(), L".nxl")) {//should not be nxl file
		ASSERT(TRUE);
		return;
	}

	do {
		Sleep(1000);
		SYSTEMTIME st = { 0 };
		GetSystemTime(&st);
		// -YYYY-MM-DD-HH-MM-SS
		std::wstring  targetName = path.GetFileName();
		const std::wstring sTimestamp(NX::FormatString(L"-%04d-%02d-%02d-%02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond));

		std::wstring::size_type posSuffix = targetName.rfind(L'.');
		if (posSuffix == std::wstring::npos) {
			targetName += sTimestamp;
		}
		else {
			const std::wstring suffix(targetName.substr(posSuffix));
			targetName = targetName.substr(0, posSuffix);
			targetName += sTimestamp;
			targetName += suffix;
		}
		targetName += L".nxl";
		target = path.GetParentDir() + L"\\" + targetName;
	} while (PathFileExists(target.c_str()));

	theApp.ShowTrayMsg(theApp.LoadStringW(IDS_PROTECTING_FILE)/*"Protecting file..."*/);

	std::wstring newFileId;
	std::wstring newFilePath;
	NX::Result res = theApp.m_pSession->LocalFileProtect(source, L"", L"", metadata, tags, rights, obs, &g_blogout, newFileId, newFilePath, &secret);
	if (!res) {
		if (!g_blogout) {//show message only when user is still login
		    theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAIL_TO_PROTECT_FILE).GetBuffer()/*"Failed to protect file."*/).c_str());
		}
		return;
	}
    CString vaultPath = newFilePath.c_str();
    int pos = vaultPath.ReverseFind(L'/');
    if (pos != -1)
    {
        vaultPath = vaultPath.Right(vaultPath.GetLength() - pos - 1);
    }

	std::wstring outStr = theApp.LoadStringW(IDS_FILE_PROTECTED_UPLOADED_VAULT).GetBuffer()/*"The file is protected and uploaded to MyVault:"*/ + vaultPath;
	theApp.ShowTrayMsg(outStr.c_str());
	theApp.RefreshMyVaultInfo();
}

void TestSharingDlg(HWND hWnd)
{

}


BOOL GetPropertiesFromFile(const std::wstring& file , NXSharingData &dataIn, BOOL &bOwner)
{
    if (!NX::iend_with<wchar_t>(file, L".nxl")) {
		return FALSE;
    }

	bOwner = FALSE;
    NX::Result res = RESULT(0);
    std::shared_ptr<NX::NXL::File> fp(theApp.m_pSession->LocalFileOpen(file, true, res));
    if (fp == NULL) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAIL_TO_OPEN_NXL).GetBuffer()/*"Failed to open the .NXL file."*/).c_str());
		return FALSE;
    }

    if (!fp->IsFullToken()) {
        theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_OBTAIN_TOKEN).GetBuffer()/*"Failed to obtain the file token from the server."*/).c_str());
		return TRUE;//continue for server processing
    }
    else {
        if (ERROR_DATA_CHECKSUM_ERROR == res.GetCode()) {
            //TRACE(L"%s", theApp.LoadStringW(IDS_HEADER_CHECK_SUM)/*"File header has been compromised (Mimatched header checksum)"*/);
            TRACE(L"%s", L"File header has been compromised (Mimatched header checksum)");
        }
    }
    NX::NXL::FileAdHocPolicy ahPolicy;
    res = fp->ReadAdHocPolicy(ahPolicy);
    if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_READ_CONTENT).GetBuffer()/*"Failed to read the file contents."*/).c_str());
        return FALSE;
    }
    else {
		for (auto id : theApp.m_pSession->GetCurrentUser().GetMemberships()) {
			if (0 == ahPolicy.GetOwnerId().compare(id.GetId()))
				bOwner = TRUE;
		}
		
		NX::time::datetime dt(ahPolicy.GetIssueTime());
		for (int i = 0; i < (int)ahPolicy.GetRights().size(); i++) {
			RightsAndObsToData(ahPolicy.GetRights()[i].c_str(), dataIn);
		}
		for (int i = 0; i < (int)ahPolicy.GetObligations().size(); i++) {

			RightsAndObsToData(ahPolicy.GetObligations()[i].c_str(), dataIn);
		}
    }
	return TRUE;
}

typedef struct _ShareProtectInfo {
	std::wstring source;
	std::vector<std::wstring> rightstr;
	std::vector<std::wstring> obs;
	BOOL	bSharedflag;
	std::wstring membershipId;
	std::vector<std::wstring> recipients;
} SSHAREPROTECTINFO, *PSSHAREPROTECTINFO;

std::thread uithread;
std::thread sharingThread;
std::thread viewThread;
std::list<SSHAREPROTECTINFO> shareprotectfiles;
std::list<std::wstring> viewfiles;
bool bquit = FALSE;
bool LocalFileView(const std::wstring& file)
{
	while (!bquit) {
		while (viewfiles.size() != 0 && !g_blogout) {
			std::wstring filename;
			theApp.ShowTrayMsg(theApp.LoadStringW(IDS_LAUNCH_VIEWER)/*"Launching viewer..."*/);
			filename = viewfiles.front();
			viewfiles.pop_front();
			theApp.LocalFileView(filename);
		}
		if (g_blogout) {//remove all files
			while (viewfiles.size() != 0) {
				viewfiles.pop_front();
			}
		}
		Sleep(500);
	}
	return true;
}

bool LocalShareProtectFile(const std::wstring & file)
{
	while (!bquit) {
		while (shareprotectfiles.size() != 0 && !g_blogout) {
			SSHAREPROTECTINFO finfo;
			finfo = shareprotectfiles.front();
			shareprotectfiles.pop_front();
			if (finfo.bSharedflag) {
				LocalFileShare(finfo.source, finfo.membershipId, finfo.rightstr, finfo.obs, finfo.recipients);
			}
			else {
				LocalFileProtect(finfo.source, finfo.rightstr, finfo.obs);
			}
		}
		if (g_blogout) {//remove all files
			while (shareprotectfiles.size() != 0) {
				shareprotectfiles.pop_front();
			}
		}
		Sleep(500);
	}
	return true;
}


mutex mtx;
void SharingLocalFile(const HWND hWnd, const wstring filePath)
{
    NXSharingData dataIn = { false, false, false, false, false, false, false, DLGTYPE_SHARING, };
    LPWSTR pAddresses;

    std::lock_guard<std::mutex> lck(mtx);
	if (!sharingThread.joinable())
	{//launch sharing thread
		thread shareth(LocalShareProtectFile, filePath);
		sharingThread = move(shareth);
	}

	if (NX::iend_with<wchar_t>(filePath, L".nxl"))
	{
		BOOL bowner;
		//ignore the return value and show dialog based on rights.
		if (!GetPropertiesFromFile(filePath, dataIn, bowner))
			return;
	
		dataIn.bAssignedRights = TRUE;
	}

    if (!RmuShowShareDialog(hWnd, filePath.c_str(), dataIn, &g_dataOut,&pAddresses))
    {
        auto size = lstrlen(pAddresses);
        if (size)
        {
            vector<wstring> peopleArr;
            CString addresses = pAddresses;
            int curPos = 0;
            CString resToken = addresses.Tokenize(_T(";"), curPos);
            while (resToken != _T(""))
            {
                peopleArr.push_back(resToken.GetBuffer());
                resToken = addresses.Tokenize(_T(";"), curPos);
            };
            std::vector<std::wstring> rights;
            std::vector<std::wstring> obs;
            DataToRightsAndObs(g_dataOut, rights, obs);
			SSHAREPROTECTINFO finfo;
			finfo.source = filePath;
			finfo.bSharedflag = TRUE;
			finfo.membershipId = theApp.m_pSession->GetCurrentUser().GetMemberships().at(0).GetId();
			finfo.rightstr = rights;
			finfo.obs = obs;
			finfo.recipients = peopleArr;

			shareprotectfiles.push_back(finfo);
            std::cout << std::endl;
        }
        g_dataOut.hWnd = NULL;
    }
}

void ProtectingLocalFile(const HWND hWnd, const wstring filePath)
{
	if (NX::iend_with<wchar_t>(filePath, L".nxl")) {
		theApp.ShowTrayMsg(theApp.LoadStringW(IDS_ALREADY_PROTECTED)/*"The file you have selected is already protected."*/);
		return;
	}

	NXSharingData dataIn = { true, false, false, false, false, false, false, DLGTYPE_PROTECT, };
	LPWSTR pAddresses;


	if (!sharingThread.joinable())
	{//launch sharing thread
		thread shareth(LocalShareProtectFile, filePath);
		sharingThread = move(shareth);
	}
	std::lock_guard<std::mutex> lck(mtx);

	if (!RmuShowShareDialog(hWnd, filePath.c_str(), dataIn, &g_dataOut, &pAddresses))
	{
		std::vector<std::wstring> rights;
		std::vector<std::wstring> obs;
		DataToRightsAndObs(g_dataOut, rights, obs);
		SSHAREPROTECTINFO finfo;
		finfo.source = filePath;
		finfo.rightstr = rights;
		finfo.obs = obs;
		finfo.bSharedflag = FALSE;
		shareprotectfiles.push_back(finfo);
		std::cout << std::endl;
		g_dataOut.hWnd = NULL;
	}
}



void TestProtectDlg(HWND hWnd)
{
}


CRect m_rcOriginalRect;
int m_nCurHeight;
int m_nCurWidth;
int m_nScrollPos;
int m_nHScrollPos;
HWND m_hWnd;
MONITORINFO m_monInfo;

HWND GetHwnd()
{
    return m_hWnd;
}

BOOL OnVScroll(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = GetHwnd();
    int nDelta;
    int nMaxPos = m_rcOriginalRect.Height() - m_nCurHeight;
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

BOOL OnHScroll(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = GetHwnd();
    int nDelta;
    int nMaxPos = m_rcOriginalRect.Width() - m_nCurWidth;
    int nSBCode = LOWORD(wParam);
    int nPos = HIWORD(wParam);

    switch (nSBCode)
    {
    case SB_LINELEFT:
        if (m_nHScrollPos >= nMaxPos)
            return FALSE;

        nDelta = min(max(nMaxPos / 20, 5), nMaxPos - m_nHScrollPos);
        break;

    case SB_LINERIGHT:
        if (m_nHScrollPos <= 0)
            return FALSE;
        nDelta = -min(max(nMaxPos / 20, 5), m_nHScrollPos);
        break;
    case SB_PAGERIGHT:
        if (m_nHScrollPos >= nMaxPos)
            return FALSE;
        nDelta = min(max(nMaxPos / 10, 5), nMaxPos - m_nHScrollPos);
        break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        nDelta = (int)nPos - m_nHScrollPos;
        break;

    case SB_PAGELEFT:
        if (m_nHScrollPos <= 0)
            return FALSE;
        nDelta = -min(max(nMaxPos / 10, 5), m_nHScrollPos);
        break;

    default:
        return FALSE;
    }
    m_nHScrollPos += nDelta;
    SetScrollPos(hDlg, SB_HORZ, m_nHScrollPos, TRUE);
    ScrollWindow(hDlg, -nDelta, 0, NULL, NULL);
    return FALSE;
}


BOOL OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = GetHwnd();
    int nMaxPos = m_rcOriginalRect.Height() - m_nCurHeight;
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

SCROLLINFO SetScrollbarInfo()
{
    HWND hDlg = GetHwnd();
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    si.nMin = 0;
    si.nMax = m_rcOriginalRect.Height();
    si.nPage = m_nCurHeight;
    si.nPos = 0;
    SetScrollInfo(hDlg, SB_VERT, &si, TRUE);

    si.fMask = SIF_ALL;
    si.nMin = 0;
    si.nMax = m_rcOriginalRect.Width();
    si.nPage = m_nCurWidth;
    si.nPos = 0;
    SetScrollInfo(hDlg, SB_HORZ, &si, TRUE);

    //_bScroll = (si.nMax > (int)si.nPage);
    return si;
}

CSize GetNonClientSize(HWND hWnd)
{
    CRect rcClient;
    GetClientRect(hWnd, &rcClient);
    CRect rcMain;
    GetWindowRect(hWnd, rcMain);
    int nNCWidth = rcMain.Width() - rcClient.Width();
    int nNCHeight = rcMain.Height() - rcClient.Height();
    return CSize(nNCWidth, nNCHeight);
}

void InitScrollBars(HWND hWnd, CRect &rc)
{
    m_rcOriginalRect = rc;
    CRect rect;
    GetClientRect(hWnd, &rect);
    m_nCurHeight = rect.Height() + 1;
    m_nCurWidth = rect.Width() + 1;
    m_nScrollPos = 0;
    m_nHScrollPos = 0;
    m_hWnd = hWnd;
    SetScrollbarInfo();
}
 
void SetContainerPos(HWND hWnd)
{
    CSize ncSize = ::GetNonClientSize(hWnd);
    CRect rc;
    theApp.m_homePage.GetWindowRect(&rc);
    CRect rcMon = m_monInfo.rcWork;

    auto rmcHeight = rc.Height() + ncSize.cy;
    auto monHeight = rcMon.Height();
    auto rmcWidth = rc.Width() + ncSize.cx;
    auto monWidth = rcMon.Width();
    if (rmcHeight > monHeight || rmcWidth > monWidth)
    {
        if (rc.Width() + ncSize.cx > rcMon.Width())
        {
            int nHeight = rc.Height() + ncSize.cy;
            int nWidth = rc.Width() + ncSize.cx;
            SetWindowPos(hWnd, 0, 0, 0, rcMon.Width(), nHeight + GetSystemMetrics(SM_CYHSCROLL),
                SWP_NOREPOSITION | SWP_NOOWNERZORDER | SWP_NOMOVE);
        }
        if (rmcHeight > monHeight)
        {
            int nHeight = rc.Height() + ncSize.cy;
            int nWidth = rc.Width() + ncSize.cx;
            SetWindowPos(hWnd, 0, 0, 0, nWidth + GetSystemMetrics(SM_CXVSCROLL), rcMon.Height(),
                SWP_NOREPOSITION | SWP_NOOWNERZORDER | SWP_NOMOVE);
        }

    }
    else
    {
        ::SetWindowPos(hWnd, 0, 0, 0, rc.Width() + ncSize.cx + 1, rc.Height() + ncSize.cy + GetSystemMetrics(SM_CYHSCROLL), SWP_NOREPOSITION | SWP_NOOWNERZORDER | SWP_NOMOVE);
        //MINMAXINFO mmi = { 0 };
        //SendMessage(hWnd, WM_GETMINMAXINFO, NULL, (LPARAM)&mmi);
        //POINT sz = { rc.Width() + ncSize.cx + 1, rc.Height() + ncSize.cy + GetSystemMetrics(SM_CYHSCROLL) };
        //mmi.ptMaxSize = sz;
        theApp.m_maxSize = { rc.Width() + ncSize.cx + 1, rc.Height() + ncSize.cy + GetSystemMetrics(SM_CYHSCROLL) };

    }
    auto mainWnd = CWnd::FromHandle(hWnd);
    mainWnd->CenterWindow();
}

void CancelUpgradeCallback()
{

}
//-------------------------------Following file are for local files-------------------------------------------------------




void DataToRightsAndObs(const NXSharingData &dataOut, std::vector<std::wstring>& rights, std::vector<std::wstring>& obs)
{
    if (dataOut.bView)
    {
        rights.push_back(RIGHT_ACTION_VIEW);
    }
    if (dataOut.bPrint)
    {
        rights.push_back(RIGHT_ACTION_PRINT);
    }
    if (dataOut.bShare)
    {
        rights.push_back(RIGHT_ACTION_SHARE);
    }
    if (dataOut.bDownload)
    {
        rights.push_back(RIGHT_ACTION_DOWNLOAD);
    }
    if (dataOut.bWaterMark)
    {
        obs.push_back(OBLIGATION_NAME_WATERMARK);
    }
}

void RightsAndObsToData(const std::wstring& rights, NXSharingData &dataIn )
{
    if (!lstrcmp(RIGHT_ACTION_VIEW, rights.c_str()))
    {
        dataIn.bView = true;
    }
    if (!lstrcmp(RIGHT_ACTION_PRINT, rights.c_str()))
    {
        dataIn.bPrint = true;
    }
    if (!lstrcmp(RIGHT_ACTION_SHARE, rights.c_str()))
    {
        dataIn.bShare = true;
    }
    if (!lstrcmp(RIGHT_ACTION_DOWNLOAD, rights.c_str()))
    {
        dataIn.bDownload = true;
    }
    if (!lstrcmp(OBLIGATION_NAME_WATERMARK, rights.c_str()))
    {
        dataIn.bWaterMark = true;
    }
}

void LocalFileUnprotect(const std::wstring& source)
{
    NX::Result res = RESULT(0);
    std::shared_ptr<NX::NXL::File> fp(theApp.m_pSession->LocalFileOpen(source, true, res));
    if (fp == NULL) {
        theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAIL_TO_OPEN_NXL).GetBuffer()/*"Failed to open NXL file."*/).c_str());
        return;
    }

    if (ERROR_DATA_CHECKSUM_ERROR == res.GetCode()) {
        theApp.ShowTrayMsg(theApp.LoadStringW(IDS_FILE_HEADER_COMPROMISED)/*"The file header has been compromised."*/);
        return;
    }
    //wstring destFile;

    CString srcFile = source.c_str();
    CString fNamePart = srcFile;	//This variable is the file path without extension
    CString fExtPart = L"";			//This variable is the file extension

    int pos = srcFile.ReverseFind(L'.');
    if (pos != -1)
    {
        fNamePart = srcFile.Left(pos);
        fExtPart = srcFile.Right(srcFile.GetLength() - (pos));
        pos = fNamePart.ReverseFind(L'.');
        if (pos != -1)
        {
            fExtPart = fNamePart.Right(fNamePart.GetLength() - (pos));
            fNamePart = fNamePart.Left(pos);
        }
        else
        {
            fExtPart = L"";	//There is not extension for this file.
        }
    }
    else
    {
        TRACE(L"Decrypt non nxl file: %s", srcFile);
        return;
    }

    wstring target = fNamePart + fExtPart;
    int i = 1;
    while (PathFileExists(target.c_str()))
    {
        wstringstream txtOut;
        txtOut << fNamePart.GetBuffer() << L" (" << i << L")" << fExtPart.GetBuffer();
        i++;
        target = txtOut.str().c_str();
    }

    res = fp->Unprotect(target);
    std::cout << std::endl;
    if (!res) {
        theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_DECRYPT).GetBuffer()/*"Failed to decrypt file"*/).c_str());
        return;
    }
    CString filePath = target.c_str();
    pos = filePath.ReverseFind(L'\\');
    if (pos != -1)
    {
        filePath = filePath.Right(filePath.GetLength() - (pos + 1));
    }

    CString outStr = theApp.LoadStringW(IDS_PREFIX_DECRYT_AVAIL_AT)/*"The decrypted file is available at "*/ + filePath;
    theApp.ShowTrayMsg(outStr.GetBuffer());
}


void ProcessLocalFiles(HWND sentWnd, int opCode, wstring filePath)
{
    switch (opCode)
    {
    case NxCmdProtect:
	{
		if (g_dataOut.hWnd)
		{	//If the sharing window is open, we bring this window to top
			if (IsWindow(g_dataOut.hWnd))
			{
				BringWindowToTop(g_dataOut.hWnd);
				SetForegroundWindow(g_dataOut.hWnd);
				SwitchToThisWindow(g_dataOut.hWnd, FALSE);
				break;
			}
		}

		if (uithread.joinable())
		{
			uithread.join();
		}
        thread th(ProtectingLocalFile, sentWnd, filePath);
		uithread = move(th);
	}
		break;
    case NxCmdUnprotect:
        std::async(std::launch::async, LocalFileUnprotect, filePath);
        break;
	case NxCmdView:
	{
		if (viewThread.joinable())
		{
			viewfiles.push_back(filePath);
		}
		else {
			viewfiles.push_back(filePath);
			thread th(LocalFileView, filePath);
			viewThread = move(th);
		}
		//        std::async(std::launch::async, LocalFileView, filePath);
	}
        break;
    case NxCmdShare:
    {	
        if (g_dataOut.hWnd)
        {	//If the sharing window is open, we bring this window to top
            if (IsWindow(g_dataOut.hWnd))
            {
                BringWindowToTop(g_dataOut.hWnd);
                SetForegroundWindow(g_dataOut.hWnd);
                SwitchToThisWindow(g_dataOut.hWnd, FALSE);
                break;
            }
        }

		if (uithread.joinable())
		{
			uithread.join();
		}
		thread th(SharingLocalFile, sentWnd, filePath);
		uithread = move(th);
    }
        break;
    case NxCmdProperties:
        MessageBox(NULL, filePath.c_str(), theApp.LoadStringW(IDS_PROPERTIES)/*"Properties"*/, MB_OK);
        break;
    default:
        break;
    }
    
}
//-------------------------------------------------------Local file functions end here------------------------------------------------------------------------

// static ProgressFuncs progressFuncs = { 0,0,0, false, L"Download..." };
// 
// void ProgressProc(HWND hWnd)
// {
//     RmuOpenProgressWindow(hWnd, progressFuncs, true);
// 
//     Sleep(5000);
//     progressFuncs.closeProgress();
//     progressFuncs.closeProgress = 0;
// 
// 
// }

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static UINT s_uTaskbarRestart;

    switch (message)
    {
    case WM_CREATE:
    {
        TRACE(L"%s",L"nxrmc Window created!");
        auto bLoggedIn = theApp.m_pSession->ValidateCredential();
		NX::Result res;
		if (!bLoggedIn) {
			res = theApp.m_pSession->PrepareLogin(theApp.GetTenant());
			if (!res)
			{
				std::wstring strerr;
				strerr = OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_INIT_SKYDRM).GetBuffer()/*"Failed to Initialize SkyDRM."*/);
				MessageBox(hWnd, strerr.c_str(), theApp.LoadStringW(IDS_ERROR), MB_OK);
				PostQuitMessage(-1);
				return -1;
			}
		}
		else {
			if (!theApp.InitializeRestfulData())
				bLoggedIn = false;//reset login flag
		}

        theApp.m_dlgIntro.Create(IDD_DIALOG_INTRO, ::CWnd::FromHandle(hWnd));
        if (!bLoggedIn)
        {
            theApp.m_dlgIntro.ShowWindow(SW_SHOW);
			theApp.m_curShowedPage = MD_INTRODUCTION;
        }
        ::CRect rc;
        #define introToFullRatio    950 /660
        theApp.m_dlgIntro.GetWindowRect(rc);
        CSize ncSize = GetNonClientSize(hWnd);
		theApp.m_dlgIntro.m_iWindowHight = rc.Height() + ncSize.cy;
		theApp.m_dlgIntro.m_iWindowWidth = rc.Width() * introToFullRatio + ncSize.cx;

		
		

        auto monHeight = GetSystemMetrics(SM_CYSCREEN);
        auto monWidth = GetSystemMetrics(SM_CXSCREEN);
		if (600  > monHeight || 800 > monWidth) {
			MessageBox(hWnd, theApp.LoadStringW(IDS_RESOLUTION_NOT_SUPPORTED)/*"The resolution is not supported."*/, theApp.LoadStringW(IDS_ERROR), MB_OK);
			PostQuitMessage(-1);
			return -1;
		}

        SetWindowPos(hWnd, 0, 0, 0, theApp.m_dlgIntro.m_iWindowWidth + GetSystemMetrics(SM_CXVSCROLL), theApp.m_dlgIntro.m_iWindowHight + GetSystemMetrics(SM_CYHSCROLL),
            SWP_NOREPOSITION | SWP_NOOWNERZORDER | SWP_NOMOVE);

        InitScrollBars(hWnd, rc);

        CWnd *pWnd = CWnd::FromHandle(hWnd);
        pWnd->CenterWindow();

		theApp.m_mainhWnd = hWnd;

		if (bLoggedIn) {
			theApp.InitializeWindows(::CWnd::FromHandle(hWnd));
			theApp.ReloadMainMenu(hWnd);
			theApp.ShowPage(MD_HOMEPAGE);
		}

        g_dataOut.hWnd = NULL;
        theApp.InitTrayIcon(hWnd, AfxGetInstanceHandle());
		s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
		
		if (bLoggedIn) {//launch viewer if needed after trayicon is created.
			if (theApp.GetArgOpenFileName().length() > 0) {
                  PostMessage(hWnd, WM_NX_LAUNCH_VIEWER, 0, 0);
			}
		}
        break;
    }
    case WM_NX_LAUNCH_VIEWER:
    {
        wstring openfile = theApp.GetArgOpenFileName();
        theApp.ClearArgOpenFile();
        viewfiles.push_back(openfile);
        thread th(LocalFileView, openfile);
        viewThread = move(th);
    }
        break;

	case WM_CLOSE:
		ShowWindow(hWnd, SW_HIDE);
		break;
    case WM_NX_LOGON_END:
    {
        if (CMainDlg::GetCloseID() == IDOK)
        {
			theApp.InitializeRestfulData();
			theApp.InitializeWindows(::CWnd::FromHandle(hWnd));
			theApp.ReloadMainMenu(hWnd);
			theApp.m_homePage.RefreshPage();
			theApp.m_dlgRepoPage.RefreshPage();
            theApp.m_dlgProjPage.RefreshProjList(TRUE);
            theApp.m_dlgRepoPage.m_switchDlg.RefreshProjList();
			if (theApp.m_dlgProjMain.m_switchDlg.m_hWnd)
			{
				theApp.m_dlgProjMain.m_switchDlg.RefreshProjList();
			}
			
			g_blogout = FALSE;
            theApp.ShowPage(MD_HOMEPAGE);

			std::wstring openfile = theApp.GetArgOpenFileName();//launch viewer after user login
			if (openfile.length() > 0) {//the code below should only be triggered at first login.
				theApp.ClearArgOpenFile();
				viewfiles.push_back(openfile);
				thread th(LocalFileView, openfile);
				viewThread = move(th);
			}
		}
        else
        {
            DestroyWindow(hWnd);
        }
        break;
    }
    case WM_NX_OPEN_PROJECT:
    {   
        if (theApp.m_dlgProjPage.IsWindowVisible())
        {
            theApp.m_dlgProjMain.SetReturnPage(NXRMC_FROM_PROJECT_PAGE);
        }
        else if (theApp.m_homePage.IsWindowVisible())
        {
            theApp.m_dlgProjMain.SetReturnPage(NXRMC_FROM_HOME_PAGE);
        }
		if(wParam == 65535)
			theApp.m_dlgProjMain.SetProject(*(CProjData*)lParam);
		else {
 			CProjData pdata;
			int index = (int)wParam;
			if (index >= (int)theApp.GetProjectByMeCount())
				theApp.GetProjectByOthersInfo(index - (int)theApp.GetProjectByMeCount(), pdata);
			else
				theApp.GetProjectByMeInfo(index, pdata);
			theApp.m_dlgProjMain.SetProject(pdata);
		}
        theApp.ShowPage(MD_PROJECTMAIN);
        theApp.m_dlgProjMain.ShowProjPage(NX_PROJ_SUMMARY_PAGE);
		theApp.MessageYield();
		if (theApp.m_dlgProjMain.m_switchDlg.m_hWnd)
		{
			theApp.m_dlgProjMain.m_switchDlg.RefreshProjList();
		}

        break;
    }
    case WM_NX_RESIZE_MAIN:
    {
        switch (wParam)
        {
        case NX_LOGON_DLG:
            if (theApp.m_plogonDlg)
            {
                auto hwndChild = theApp.m_plogonDlg->GetHwnd();
                CRect rcChild;
                GetWindowRect(hwndChild, &rcChild);
                CSize sz = GetNonClientSize(hWnd);
                CRect rcMax = m_monInfo.rcWork;
                if (rcChild.Width() + sz.cx > rcMax.Width() || rcChild.Height() + sz.cy > rcMax.Height())
                {
                    ShowWindow(hWnd, SW_SHOWMAXIMIZED);
                }
                else
                {
                    SetWindowPos(hWnd, HWND_TOP, 0, 0, rcChild.Width() + sz.cx + GetSystemMetrics(SM_CXVSCROLL), 
                        rcChild.Height() + sz.cy + GetSystemMetrics(SM_CYHSCROLL), SWP_NOMOVE | SWP_NOZORDER);
                }
                InitScrollBars(hWnd, rcChild);
                break;
            }
            break;
            
        case NX_MAIN_DLG:
        {
            
            SetContainerPos(hWnd);
            break;
        }

        }
        break;
    }
    case WM_HSCROLL:
    {
        OnHScroll(wParam, lParam);
    }
    break;
    case WM_VSCROLL:
    {
        OnVScroll(wParam, lParam);
    }
    break;
    case WM_MOUSEWHEEL:
        OnMouseWheel(wParam, lParam);
        break;
    case WM_COPYDATA:
    {
        g_blogout = !theApp.m_pSession->ValidateCredential();
        if(!g_blogout)
        {
            if (lParam)
            {
                wstring filePath = (LPWSTR)(((COPYDATASTRUCT*)lParam)->lpData);
                int opCode = (int)(((COPYDATASTRUCT*)lParam)->dwData);
                ProcessLocalFiles((HWND)wParam, opCode, filePath);
            }
        } 
        else
        {
            theApp.ShowTrayMsg(theApp.LoadStringW(IDS_ENTER_LOGIN_CREDENTIALS)/*"Enter your login credentials..."*/);
        }
        break;
    }
    case WM_TRAY:
        switch (lParam)
        {
        case WM_SHOWVIEWER:
            break;
		case WM_LBUTTONDBLCLK:
			if (!theApp.m_dlgIntro.m_hWnd)
			{//This is the situation that the network is not connected. Avoid crash
				break;
			}
			ShowWindow(hWnd, SW_SHOWNORMAL);
			BringWindowToTop(hWnd);
			SetForegroundWindow(hWnd);
			break;
        case WM_RBUTTONUP:
        {
            POINT pt;
            //int cmd;
            CMenu menu;
            menu.LoadMenuW(IDR_MENU_TRAYICON);
            auto pMenu = menu.GetSubMenu(0);
            auto bLoggedIn = theApp.m_pSession->ValidateCredential();
			if (theApp.m_curShowedPage == MD_INTRODUCTION || bLoggedIn) {
				pMenu->AppendMenu(MF_STRING, ID_NEXTLABSMENU_LOG_IN_OUT, bLoggedIn ? theApp.LoadStringW(IDS_LOG_OUT) : theApp.LoadStringW(IDS_LOG_IN)/*"Log out" : L"Log in"*/);
				EnableMenuItem(pMenu->m_hMenu, ID_NEXTLABSMENU_LOG_IN_OUT, MF_BYCOMMAND | (theApp.IsDialogPopup() || theApp.IsProgressDlgRunning()) ? MF_GRAYED : MF_ENABLED);
			}
			pMenu->AppendMenu(MF_STRING, IDM_EXIT, theApp.LoadStringW(IDS_EXIT));
			GetCursorPos(&pt);
            SetForegroundWindow(hWnd);
            auto cmd = pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, CWnd::FromHandle(hWnd), NULL);
			switch (cmd)
			{
			case ID_NEXTLABSMENU_LOG_IN_OUT:
				if (bLoggedIn)
				{
					g_blogout = TRUE;
					theApp.LogoutUser();
				}
				else
				{
					theApp.ShowLogin();
				}
				break;
			case ID_NEXTLABSMENU_GOTOSKYDRM:
			{
				NX::Result res = theApp.m_pSession->PrepareLogin(theApp.GetTenant());
				if (!res)
				{
					TRACE(L"%s",L"Failed to prepare login");
				}
				else
				{
					auto pRMSServer = theApp.m_pSession->GetCurrentRMS();
					auto szUrl = pRMSServer.GetUrl();
					ShellExecuteW(NULL, L"open", szUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}

				break;
			}
			case ID_NEXTLABSMENU_OPENRMC:
				ShowWindow(hWnd, SW_SHOWNORMAL);
				BringWindowToTop(hWnd);
				break;
			case IDM_EXIT:
				PostMessage(hWnd, WM_COMMAND, IDM_EXIT, 0);
				break;
			}
        }
        break;
        }
        break;
	case WM_INITMENUPOPUP:
		{
			HMENU hmenu = (HMENU)wParam;
			WORD windex = LOWORD(lParam);

			if (HIWORD(lParam))//system menu related ignore
				break;
			if (theApp.m_curShowedPage < MD_HOMEPAGE) {
				if (theApp.m_curShowedPage != MD_INTRODUCTION) {
					RemoveMenu(hmenu, ID_FILE_LOGIN, MF_BYCOMMAND);
				}
				break;
			}

            if (hmenu == GetSubMenu(theApp.m_hMainMenu, 0)) {//File menu
                EnableMenuItem(hmenu, ID_FILE_OPENFILE, MF_BYCOMMAND |
                    ((theApp.m_curShowedPage == MD_REPORITORYHOME && theApp.m_dlgRepoPage.IsFileSelected() && (theApp.m_dlgRepoPage.IsFileDeleteAvailable()||(theApp.m_dlgRepoPage.m_selBtnIdx== NX_BTN_OTHERREPOS && !theApp.m_dlgRepoPage.IsProtectAvailable()/*nxl file*/)))
                        || (theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsFileInfoAvailable()))
                         ? MF_ENABLED : MF_GRAYED);
            
				if (theApp.m_curShowedPage == MD_HOMEPAGE) {
					EnableMenuItem(hmenu, ID_FILE_PROTECTFILE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hmenu, ID_FILE_SHAREFILE, MF_BYCOMMAND | MF_ENABLED);
				}
				else {
					if (theApp.m_curShowedPage == MD_REPORITORYHOME) {
						EnableMenuItem(hmenu, ID_FILE_PROTECTFILE, MF_BYCOMMAND | (((!theApp.m_dlgRepoPage.IsSwitchPageOpen()) && (theApp.m_dlgRepoPage.IsProtectAvailable() || !theApp.m_dlgRepoPage.IsFileItemSelected())) ? MF_ENABLED : MF_GRAYED));
						EnableMenuItem(hmenu, ID_FILE_SHAREFILE, MF_BYCOMMAND | (((!theApp.m_dlgRepoPage.IsSwitchPageOpen()) && (theApp.m_dlgRepoPage.IsShareAvailable() || !theApp.m_dlgRepoPage.IsFileItemSelected())) ? MF_ENABLED : MF_GRAYED));
					}
                    else
                    {
                        EnableMenuItem(hmenu, ID_FILE_PROTECTFILE, MF_BYCOMMAND | MF_GRAYED);
                        EnableMenuItem(hmenu, ID_FILE_SHAREFILE, MF_BYCOMMAND | MF_GRAYED);
                    }
				}
				EnableMenuItem(hmenu, ID_FILE_UPLOADFILE, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_REPORITORYHOME && theApp.m_dlgRepoPage.IsUploadAvailable())
                    || theApp.m_curShowedPage == MD_PROJECTMAIN && (theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible() || theApp.m_dlgProjMain.m_summaryDlg.IsWindowVisible())) ? MF_ENABLED : MF_GRAYED);
				EnableMenuItem(hmenu, ID_FILE_DELETEFILE, MF_BYCOMMAND | 
                    ((theApp.m_curShowedPage == MD_REPORITORYHOME && theApp.m_dlgRepoPage.IsFileDeleteAvailable()
                        || theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsFileDeleteable()) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_FILE_INFO, MF_BYCOMMAND| ((theApp.m_curShowedPage == MD_REPORITORYHOME && theApp.m_dlgRepoPage.IsFileInfoAvailable()
                        || theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsFileInfoAvailable()) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_FILE_ACTIVITYLOG, MF_BYCOMMAND | MF_GRAYED);
			}
			else if (hmenu == GetSubMenu(theApp.m_hMainMenu, 1)) {//View Menu
				EnableMenuItem(hmenu, ID_VIEW_PREVIOUS, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_REPORITORYHOME && theApp.m_dlgRepoPage.IsBackAvailable()) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_VIEW_NEXT, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_REPORITORYHOME && theApp.m_dlgRepoPage.IsForwardAvailable()) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_VIEW_REFRESHSCREEN, MF_BYCOMMAND | (((theApp.m_curShowedPage == MD_REPORITORYHOME ) || theApp.m_curShowedPage == MD_PROJECTMAIN || theApp.m_curShowedPage == MD_PROJECTSHOME
                    || theApp.m_curShowedPage == MD_HOMEPAGE)
                    ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, SORTBY_SUBMEN_INDEX, MF_BYPOSITION | (((theApp.m_curShowedPage == MD_REPORITORYHOME && !theApp.m_dlgRepoPage.IsSwitchPageOpen())
                    || (theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible())) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, REPOSITORIES_SUBMEN_INDEX, MF_BYPOSITION | ((theApp.GetRepositoryCount() > 1) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_VIEW_HOME, ((theApp.m_curShowedPage == MD_HOMEPAGE) ? MF_GRAYED : MF_ENABLED));
				EnableMenuItem(hmenu, ID_VIEW_GOTOPROJECTS, ((theApp.m_curShowedPage == MD_PROJECTSHOME) ? MF_GRAYED : MF_ENABLED));
			}
			else if (hmenu == GetSubMenu(theApp.m_hMainMenu, 2)) {//Manage Menu
				EnableMenuItem(hmenu, ID_MANAGE_SEARCH, MF_BYCOMMAND | (((theApp.m_curShowedPage == MD_REPORITORYHOME && !theApp.m_dlgRepoPage.IsSwitchPageOpen())
                    || (theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible())
                    || (theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_membersDlg.IsWindowVisible()))? MF_ENABLED : MF_GRAYED));

				EnableMenuItem(hmenu, ID_MANAGE_ADDAREPOSITORY, MF_BYCOMMAND | (((theApp.m_curShowedPage == MD_REPORITORYHOME&&!theApp.m_dlgRepoPage.IsSwitchPageOpen()) || theApp.m_curShowedPage == MD_HOMEPAGE) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_REPOSITORYLIST_RENAME, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_REPORITORYHOME && theApp.m_dlgRepoPage.GetSelectRepositoryIndex() != -1 && !theApp.m_dlgRepoPage.IsSwitchPageOpen()) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_REPOSITORYLIST_REMOVE, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_REPORITORYHOME && theApp.m_dlgRepoPage.GetSelectRepositoryIndex() != -1 && !theApp.m_dlgRepoPage.IsSwitchPageOpen()) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_MANAGE_LOCALFILE, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_HOMEPAGE || (theApp.m_curShowedPage == MD_REPORITORYHOME&&!theApp.m_dlgRepoPage.IsSwitchPageOpen())) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_MANAGE_PROFILE, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_HOMEPAGE) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_MANAGE_ACCOUNT, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_HOMEPAGE) ? MF_ENABLED : MF_GRAYED));
			}
			else if (hmenu == GetSubMenu(theApp.m_hMainMenu, 3) && GetMenuItemID(hmenu, 1) == ID_PROJECT_CREATEAPROJECT) {//Project Menu
				EnableMenuItem(hmenu, GOTOPROJECT_SUBMENU_INDEX, MF_BYPOSITION | ((theApp.GetProjectCount() >= 1) ? MF_ENABLED : MF_GRAYED));
                //We need to make sure that project is activated in order to show the create project item
				EnableMenuItem(hmenu, ID_PROJECT_CREATEAPROJECT, MF_BYCOMMAND | ((int)theApp.GetProjectByMeCount() > 0 && ((theApp.m_curShowedPage == MD_PROJECTSHOME)
                    || (theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_switchDlg.IsWindowVisible())
                    || (theApp.m_curShowedPage == MD_REPORITORYHOME && theApp.m_dlgRepoPage.IsSwitchPageOpen())) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_PROJECT_INVITEPEOPLE, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_PROJECTMAIN
                    && (theApp.m_dlgProjMain.m_membersDlg.IsWindowVisible() || theApp.m_dlgProjMain.m_summaryDlg.IsWindowVisible())) ? MF_ENABLED : MF_GRAYED));
				EnableMenuItem(hmenu, ID_PROJECT_ADDFILE, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_PROJECTMAIN
                    && (theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible() || theApp.m_dlgProjMain.m_summaryDlg.IsWindowVisible())) ? MF_ENABLED : MF_GRAYED));
				//EnableMenuItem(hmenu, ID_PROJECT_MANAGEINVITATIONS, MF_BYCOMMAND | ((theApp.m_curShowedPage == MD_PROJECTSHOME) ? MF_ENABLED : MF_GRAYED));
                RemoveMenu(hmenu, ID_PROJECT_MANAGEINVITATIONS, MF_BYCOMMAND);
                RemoveMenu(hmenu, 5, MF_BYPOSITION); //REMOVE SEPERATOR
			}
			else {//detect if it is submenu 
				if (hmenu == GetSubMenu(GetSubMenu(theApp.m_hMainMenu, 1), REPOSITORIES_SUBMEN_INDEX)) {//Repository menu
					int ncount = GetMenuItemCount(hmenu);
					for (int i = ncount; i > 0; i--) {//remove all menus
						RemoveMenu(hmenu, i - 1, MF_BYPOSITION);
					}
					std::vector<RepositoryInfo> repoarr;
					size_t arrcount = theApp.GetRepositoryArray(repoarr);
					for (size_t st = 1; st < arrcount; st++) {
						AppendMenu(hmenu, MF_STRING | MF_ENABLED, REPOSITORY_MENU_LIST_ID_BASE + st, repoarr[st].Name.c_str());
					}
				}
				if (hmenu == GetSubMenu(GetSubMenu(theApp.m_hMainMenu, 1), SORTBY_SUBMEN_INDEX)) {//SortBy Submenu
					EnableMenuItem(hmenu, ID_SORTBY_FILESIZE, MF_BYCOMMAND | ((theApp.m_dlgRepoPage.m_selBtnIdx != NX_BTN_MYVAULT && theApp.m_dlgRepoPage.m_selBtnIdx != NX_BTN_DELETEFILES && theApp.m_dlgRepoPage.m_selBtnIdx != NX_BTN_SHAREFILES)
                        || (theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible()))? MF_ENABLED : MF_GRAYED);
				}
				if (hmenu == GetSubMenu(GetSubMenu(theApp.m_hMainMenu, 3), GOTOPROJECT_SUBMENU_INDEX)) {//Project menu
					int ncount = GetMenuItemCount(hmenu);
					for (int i = ncount; i > 1; i--) {//remove all menus except the Project Home
						RemoveMenu(hmenu, i - 1, MF_BYPOSITION);
					}
					std::vector<RepositoryInfo> repoarr;
					size_t arrcount = theApp.GetProjectByMeCount();
					size_t st;
					int nindex = PROJECT_MENU_LIST_ID_BASE;
					for ( st = 0; st < arrcount; st++) {
						CProjData pdata;
						theApp.GetProjectByMeInfo((int)st, pdata);
						AppendMenu(hmenu, MF_STRING | MF_ENABLED, nindex++, pdata.m_projName);
					}
					arrcount = theApp.GetProjectByOthersCount();
					for (st = 0; st < arrcount; st++) {
						CProjData pdata;
						theApp.GetProjectByOthersInfo((int)st, pdata);
						AppendMenu(hmenu, MF_STRING | MF_ENABLED, nindex++, pdata.m_projName);
					}

				}
			}
		}
		break;

	case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
                //Testing functions from commondlg.dll
            case ID_SHOWCOMMONDIALOG_SHOWSHARINGDIALOG:
                break;
            case ID_SHOWCOMMONDIALOG_SHOWPROTECTINGDIALOG:
                break;
            case ID_SHOWCOMMONDIALOG_SHOWPROGRESSDIALOG:
                break;
            case IDM_ABOUT:
            {

                CAboutBox dlg(CWnd::FromHandle(theApp.m_mainhWnd));
                dlg.DoModal();
            }
                //DialogBox(g_APPUI.hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case ID_HELP_GETTINGSTARTED:
			{
				std::wstring url;
				url = L"https://help.skydrm.com/docs/windows/start/";
				//temporarily use hardcode string for version and language
				url += std::wstring(L"1.0") + std::wstring(L"/") + std::wstring(L"en-us") + std::wstring(L"/") + std::wstring(L"index.htm");
				ShellExecute(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
				break;
			case ID_HELP_USERMANUAL:
			{
				std::wstring url;
				url = L"https://help.skydrm.com/docs/windows/help/";
				//temporarily use hardcode string for version and language
				url += std::wstring(L"1.0") + std::wstring(L"/") + std::wstring(L"en-us") + std::wstring(L"/");
				switch (theApp.m_curShowedPage)
				{
				case MD_INTRODUCTION:
				case MD_LOGIN:
				case MD_SIGNUP:
					url += std::wstring(L"login.htm");
					break;
				case MD_REPORITORYHOME:
					url += std::wstring(L"repository.htm");
					break;
				case MD_PROJECTSHOME:
				case MD_PROJECTINTRO:
					url += std::wstring(L"projects.htm");
					break;
				case MD_PROJECTMAIN:
					url += std::wstring(L"project.htm");
					break;
				case MD_HOMEPAGE:
				default:
					url += std::wstring(L"home.htm");
					break;
				}
				ShellExecute(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
				break;
            case ID_NAVIGATE_LOGOUT:
                {
                }

                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_VIEW_PREVIOUS:
				theApp.m_dlgRepoPage.OnClickedButtonGoback();
				break;
			case ID_VIEW_NEXT:
				theApp.m_dlgRepoPage.OnClickedButtonForward();
				break;
			case ID_VIEW_REFRESHSCREEN:
				switch (theApp.m_curShowedPage) {
				case MD_HOMEPAGE:
                    theApp.RefreshHomePage();
					break;
				case MD_REPORITORYHOME:
                    if (theApp.m_dlgRepoPage.IsSwitchPageOpen())
                    {
                        theApp.m_dlgRepoPage.m_switchDlg.RefreshProjList();
                    }
                    else
                    {
                        theApp.m_dlgRepoPage.OnBnClickedButtonRefresh();
                    }
					break;
                case MD_PROJECTSHOME:
                {
                    auto cur = SetCursor(LoadCursor(NULL, IDC_WAIT));
                    theApp.m_dlgProjPage.RefreshProjList();
                    SetCursor(cur);
                    break;
                }
                case MD_PROJECTMAIN:
                    if (theApp.m_dlgProjMain.m_switchDlg.IsWindowVisible())
                    {
                        theApp.m_dlgProjMain.m_switchDlg.RefreshProjList(TRUE);
                    } 
                    else if (theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible())
                    {
                        theApp.m_dlgProjMain.m_fileDlg.OnNxRefreshProjfilelist(0, 0);
                    }
                    else if (theApp.m_dlgProjMain.m_summaryDlg.IsWindowVisible())
                    {
                        theApp.m_dlgProjMain.m_summaryDlg.OnNxRefreshProjfilelist(0, 0);
                    }
                    else if (theApp.m_dlgProjMain.m_membersDlg.IsWindowVisible())
                    {
                        theApp.m_dlgProjMain.m_membersDlg.OnBnClickedButtonRefresh();
                    }
                    break;
                }
				break;
			case ID_SORTBY_FILESIZE:
                if ((theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible()))
                {
                    theApp.m_dlgProjMain.m_fileDlg.SortColumn(3);
                }
                else
                {
                    theApp.m_dlgRepoPage.SortColumn(3);
                }
				break;
			case ID_SORTBY_FILENAME:
                if ((theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible()))
                {
                    theApp.m_dlgProjMain.m_fileDlg.SortColumn(0);
                }
                else
                {
                    theApp.m_dlgRepoPage.SortColumn(0);
                }
				break;
			case ID_SORTBY_LASTMODIFIED:
                if ((theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible()))
                {
                    theApp.m_dlgProjMain.m_fileDlg.SortColumn(1);
                }
                else
                {
                    if (theApp.m_dlgRepoPage.m_selBtnIdx == NX_BTN_MYVAULT || theApp.m_dlgRepoPage.m_selBtnIdx == NX_BTN_DELETEFILES || theApp.m_dlgRepoPage.m_selBtnIdx == NX_BTN_SHAREFILES)
                        theApp.m_dlgRepoPage.SortColumn(2);
                    else
                        theApp.m_dlgRepoPage.SortColumn(1);
                }
				break;
            case ID_VIEW_HOME:
				theApp.ShowPage(MD_HOMEPAGE);
                break;
            case ID_VIEW_MYVAULT:
				theApp.ShowPage(MD_REPORITORYHOME);
				theApp.m_dlgRepoPage.GotoRepository(NX_BTN_MYVAULT);
                break;
			case ID_VIEW_MYDRIVE:
				theApp.ShowPage(MD_REPORITORYHOME);
				theApp.m_dlgRepoPage.GotoRepository(NX_BTN_MYDRIVE);
				break;
            case ID_VIEW_GOTOPROJECTS:
                
				theApp.ShowPage(MD_PROJECTSHOME);
                break;
            case ID_FILE_INFO:
                if (theApp.m_curShowedPage == MD_REPORITORYHOME)
                {
                    theApp.m_dlgRepoPage.OnFilecontextmenuFileproperties();
                }
                else if (theApp.m_curShowedPage == MD_PROJECTMAIN)
                {
                    theApp.m_dlgProjMain.m_fileDlg.OnFilecontextmenuFileproperties();
                }

                break;
			case ID_FILE_PROTECTFILE:
				switch (theApp.m_curShowedPage) {
				case MD_REPORITORYHOME:
					if (theApp.m_dlgRepoPage.IsFileSelected()) {
						theApp.m_dlgRepoPage.OnFilecontextmenuProtect();
						break;
					}
					//else call function protect in homepage;
				case MD_HOMEPAGE:
					theApp.m_homePage.OnBnClickedButtonProtect();
					break;
				}
				break;
			case ID_FILE_SHAREFILE:
				switch (theApp.m_curShowedPage) {
				case MD_REPORITORYHOME:
					if (theApp.m_dlgRepoPage.IsFileSelected()) {
						theApp.m_dlgRepoPage.OnFilecontextmenuShare();
						break;
					}
					//else call function protect in homepage;
				case MD_HOMEPAGE:
					theApp.m_homePage.OnBnClickedButtonShare();
					break;
				}
				break;
			case ID_FILE_DELETEFILE:
                if (theApp.m_curShowedPage == MD_REPORITORYHOME)
                {
                    theApp.m_dlgRepoPage.OnFilecontextmenuDelete();
                }
                else if (theApp.m_curShowedPage == MD_PROJECTMAIN)
                {
                    theApp.m_dlgProjMain.m_fileDlg.OnFilecontextmenuDelete();
                }
                break;
			case ID_FILE_OPENFILE:
                if (theApp.m_curShowedPage == MD_REPORITORYHOME)
                {
                    theApp.m_dlgRepoPage.OnFilecontextmenuView();
                }
                else if (theApp.m_curShowedPage == MD_PROJECTMAIN)
                {
                    theApp.m_dlgProjMain.m_fileDlg.OnFilecontextmenuView();
                }
				break;
			case ID_FILE_UPLOADFILE:
                if (theApp.m_curShowedPage == MD_REPORITORYHOME)
                {
                    theApp.m_dlgRepoPage.OnBnClickedButtonUpload();
                }
                else if (theApp.m_curShowedPage == MD_PROJECTMAIN)
                {
                    if (theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible())
                    {
                        theApp.m_dlgProjMain.m_fileDlg.OnBnClickedButtonUpload();
                    }
                    else if (theApp.m_dlgProjMain.m_summaryDlg.IsWindowVisible())
                    {
                        theApp.m_dlgProjMain.m_summaryDlg.OnBnClickedButtonAddfile();
                    }
                }
				break;
			case ID_FILE_LOGIN:
				theApp.m_dlgIntro.OnBnClickedButtonLogin();
				break;
			case ID_FILE_LOGOUT:
				g_blogout = TRUE;
				theApp.LogoutUser();
				break;
			case ID_MANAGE_SEARCH:
                if ((theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible()))
                {
                    theApp.m_dlgProjMain.m_fileDlg.OnBnClickedButtonSearch();
                }
                else if(theApp.m_curShowedPage == MD_PROJECTMAIN && theApp.m_dlgProjMain.m_membersDlg.IsWindowVisible())
                {
                    theApp.m_dlgProjMain.m_membersDlg.OnBnClickedButtonSearch2();
                }
                else
                {
                    theApp.m_dlgRepoPage.OnBnClickedButtonSearch();
                }
				break;
			case ID_MANAGE_ADDAREPOSITORY:
				switch (theApp.m_curShowedPage) {
				case MD_HOMEPAGE:
					theApp.m_homePage.OnBnClickedConnectRepository();
					break;
				case MD_REPORITORYHOME:
					theApp.m_dlgRepoPage.ShowAddRepository();
					break;
				}
				break;
			case ID_REPOSITORYLIST_RENAME:
				if (theApp.OpenRenameRepositoryUI(theApp.m_dlgRepoPage.GetSelectRepositoryIndex()+1/*skip mydrive*/, theApp.m_dlgRepoPage.m_hWnd)) {
					//No need refresh UI. add other code if needed.
				}
				break;
			case ID_REPOSITORYLIST_REMOVE:
				if (theApp.OpenDeleteRepositoryUI(theApp.m_dlgRepoPage.GetSelectRepositoryIndex()+1/*skip mydrive*/, theApp.m_dlgRepoPage.m_hWnd)) {
					//No need refresh UI. add other code if needed.
				}
				break;
			case ID_MANAGE_LOCALFILE:
				theApp.m_homePage.OnManageLocalFile();
				break;
			case ID_MANAGE_ACCOUNT:
				theApp.m_homePage.OnBnClickedPersonButton();
				break;
			case ID_MANAGE_PROFILE:
				theApp.m_homePage.EditUserProfile();
				break;
			case ID_PROJECT_CREATEAPROJECT:
                theApp.AddNewProject(hWnd);
				break;
            case ID_PROJECT_INVITEPEOPLE:
                if (theApp.m_dlgProjMain.m_membersDlg.IsWindowVisible())
                {
                    theApp.m_dlgProjMain.m_membersDlg.OnBnClickedButtonInvite();
                }
                else if (theApp.m_dlgProjMain.m_summaryDlg.IsWindowVisible())
                {
                    theApp.m_dlgProjMain.m_summaryDlg.OnBnClickedButtonInvitemember();
                }
                break;
			case ID_PROJECT_ADDFILE:
                if (theApp.m_dlgProjMain.m_fileDlg.IsWindowVisible())
                {
                    theApp.m_dlgProjMain.m_fileDlg.OnBnClickedButtonUpload();
                }
                else if (theApp.m_dlgProjMain.m_summaryDlg.IsWindowVisible())
                {
                    theApp.m_dlgProjMain.m_summaryDlg.OnBnClickedButtonAddfile();
                }
                break;
			default:
				if (wmId > REPOSITORY_MENU_LIST_ID_BASE && wmId <= REPOSITORY_MENU_LIST_ID_BASE + (int)theApp.GetRepositoryCount()) {
					theApp.ShowPage(MD_REPORITORYHOME);
					std::vector<RepositoryInfo> repoarr;
					theApp.GetRepositoryArray(repoarr);
					theApp.m_dlgRepoPage.GotoRepository(NX_BTN_OTHERREPOS, repoarr[wmId - REPOSITORY_MENU_LIST_ID_BASE].Name);
					break;
				}
				if (wmId >= PROJECT_MENU_LIST_ID_BASE && wmId < PROJECT_MENU_LIST_ID_BASE + (int)(theApp.GetProjectCount())) {
					int nindex = wmId - PROJECT_MENU_LIST_ID_BASE;
                    SendMessage(theApp.m_mainhWnd, WM_NX_OPEN_PROJECT, nindex, 0);
					break;
				}
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_SIZE:
        m_nCurHeight = HIWORD(lParam);
        m_nCurWidth = LOWORD(lParam);
        //theApp.CenterAllWindows(hWnd);
        theApp.m_dlgIntro.CenterWindow(::CWnd::FromHandle(hWnd));
        if (theApp.m_plogonDlg)
        {
            theApp.m_plogonDlg->CenterWindow(hWnd);
            SetScrollbarInfo();
		}
		else if (theApp.m_curShowedPage == MD_REPORITORYHOME) {
			//CSize ncSize = GetNonClientSize(hWnd);
			//theApp.m_dlgRepoPage.ResizeWindow(m_nCurWidth, m_nCurHeight);
            ResizeRepoPage(hWnd, m_nCurWidth, m_nCurHeight);
		}
        else
        {
            if (m_nHScrollPos != 0 || m_nScrollPos != 0)
            {
                OnVScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.
                OnHScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.
            }
            
            if (theApp.m_curShowedPage == MD_HOMEPAGE) {
				theApp.CenterHomePage();
			}
 			
            if (theApp.m_curShowedPage == MD_PROJECTMAIN)
            {
               
                ResizeProjMainPage(hWnd, m_nCurWidth, m_nCurHeight);
            }
            else if (theApp.m_curShowedPage == MD_UNKNOWN 
                || theApp.m_curShowedPage == MD_INTRODUCTION)
            {
                ShowWelcomeIntroWindow(hWnd, m_nCurWidth, m_nCurHeight);
            }
            else if (theApp.m_curShowedPage == MD_PROJECTINTRO)
            {
                ShowProjIntroWindow(hWnd, m_nCurWidth, m_nCurHeight);
            }
            else if (theApp.m_curShowedPage == MD_PROJECTSHOME)
            {
                ResizeProjPage(hWnd, m_nCurWidth, m_nCurHeight);
            }
			SetScrollbarInfo();
        }
        break;

    case WM_SIZING:
          break;

    case WM_GETMINMAXINFO:
            {
            auto lpMMI = (MINMAXINFO*)(lParam);
                    if (lpMMI)
                    {
                        if (theApp.m_dlgIntro.GetSafeHwnd())
                        {
                            lpMMI->ptMinTrackSize = { 800 , 600 };
                            //HMONITOR m_hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                            //m_monInfo.cbSize = sizeof(MONITORINFO);
                            //GetMonitorInfo(m_hMonitor, &m_monInfo);
                            //CRect rcMon = m_monInfo.rcWork;
                            //theApp.m_maxSize.cx = min(theApp.m_maxSize.cx, rcMon.Width());
                            //theApp.m_maxSize.cy = min(theApp.m_maxSize.cy, rcMon.Height());
                            //lpMMI->ptMaxTrackSize = { theApp.m_maxSize.cx, theApp.m_maxSize.cy };
                        }
                    }
            }
            break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            CRect rect;
            GetClientRect(hWnd, &rect);
            HBRUSH hBrush;
            hBrush = CreateSolidBrush(NX_NEXTLABS_STANDARD_WHITE);

            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		if (uithread.joinable())
		{
			PostMessage(g_dataOut.hWnd, WM_COMMAND, IDCANCEL, 0L);
			uithread.join();
		}
		bquit = true;
		g_blogout = true;//set flag to true so all restful API call will return.
		if (sharingThread.joinable())
        {
            sharingThread.join();
        }
		if (viewThread.joinable()) {
			viewThread.join();
		}
        Shell_NotifyIcon(NIM_DELETE, &g_APPUI.nid);
        theApp.DestroyTrayIcon(hWnd);
		if (theApp.m_plogonDlg) {
			theApp.m_plogonDlg->DestroyDialog(IDCANCEL);
			delete theApp.m_plogonDlg;
		}
		theApp.m_plogonDlg = NULL;

        PostQuitMessage(0);
        break;
    default:
		if (message == s_uTaskbarRestart) {
			theApp.InitTrayIcon(hWnd, AfxGetInstanceHandle());
			break;
		}
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

