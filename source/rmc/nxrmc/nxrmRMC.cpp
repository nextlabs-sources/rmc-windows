
// nxrmRMC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "nxrmRMCDlg.h"
#include "nxmsgids.h"
#include "ConnectToRepo.h"
#include "RepoEditor.h"
#include <nx\nxl\nxlrights.h>
#include <nx\winutil\path.h>
#include <nx\winutil\registry.h>
#include <VersionHelpers.h>
#include <thread>
#include <future>
#include <regex>

#include <ManageFileDlg.h>
#include <DlgFileInfo.h>
#include "EditProfileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const std::wstring regKeyNameRouter(L"SOFTWARE\\NextLabs\\SkyDRM\\Router");

int RmcMain(_In_ HINSTANCE hInstance);
void InitScrollBars(HWND hWnd, CRect &rc);
BOOL OnVScroll(WPARAM wParam, LPARAM lParam);
BOOL OnHScroll(WPARAM wParam, LPARAM lParam);
bool SeparatePathAndName(const CString fullPath, CString &path, CString &name, const wchar_t sepChar = L'\\');
wstring GetSpaceString(__int64 bytes);
CString GetFileTimeString(NX::time::datetime dt);
void ResizeProjPage(HWND hParentWnd, int width, int height);

void ShowProjIntroWindow(HWND hParentWnd, int width, int height);
void ShowWelcomeIntroWindow(HWND hParentWnd, int width, int height);

// CnxrmRMCApp

BEGIN_MESSAGE_MAP(CnxrmRMCApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CnxrmRMCApp construction

const vector<std::pair<int, int>> iconIDArr =
{
	{ IDI_BMP_NXL,IDI_BMP },
	{ IDI_DOC_NXL,IDI_DOC },
	{ IDI_DOCX_NXL,IDI_DOCX },
	{ IDI_DWG_NXL,IDI_DWG },
	{ IDI_EXE_NXL,IDI_EXE },
	{ IDI_GIF_NXL,IDI_GIF },
	{ IDI_JPG_NXL,IDI_JPG },
	{ IDI_JT_NXL,IDI_JT },
	{ IDI_DEFAULT_NXL,IDI_DEFAULT },
	{ IDI_NXL_NXL,IDI_NXL },
	{ IDI_PDF_NXL,IDI_PDF },
	{ IDI_PNG_NXL,IDI_PNG },
	{ IDI_PPT_NXL,IDI_PPT },
	{ IDI_PPTX_NXL,IDI_PPTX },
	{ IDI_RTF_NXL,IDI_RTF },
	{ IDI_TIF_NXL,IDI_TIF },
	{ IDI_TXT_NXL,IDI_TXT },
	{ IDI_VDS_NXL,IDI_VDS },
	{ IDI_XLS_NXL,IDI_XLS },
	{ IDI_XLSX_NXL,IDI_XLSX },

    { IDI_3DXM_NXL,IDI_3DMX },
    { IDI_CATP_NXL,IDI_CATP },
    { IDI_CATS_NXL,IDI_CATS },
    { IDI_CGR_NXL ,IDI_CGR },
    { IDI_DXF_NXL ,IDI_DXF },
    { IDI_EXT_NXL ,IDI_EXT },
    { IDI_FILE_NXL,IDI_FILE },
    { IDI_IGES_NXL,IDI_IGES },
    { IDI_IGS_NXL ,IDI_IGS  },
    { IDI_IPT_NXL,  IDI_IPT },

    { IDI_PAR_NXL,  IDI_PAR },
    { IDI_PRT_NXL,  IDI_PRT },
    { IDI_PSM_NXL,  IDI_PSM },
    { IDI_RH_NXL,   IDI_RH },
    { IDI_STEP_NXL, IDI_STEP },
    { IDI_STL_NXL,  IDI_STL },
    { IDI_STP_NXL,  IDI_STP },
    { IDI_TIFF_NXL, IDI_TIFF },
    { IDI_VSDX_NXL, IDI_VSDX },
    { IDI_X_B_NXL,  IDI_X_B },

    { IDI_XMT_NXL,  IDI_XMT },
    { IDI_X_T_NXL,  IDI_X_T },
    { IDI_ZIP_NXL,  IDI_ZIP },
    { IDI_VSD_NXL,  IDI_VSD },
    { IDI_SLDA_NXL,  IDI_SLDA },
    { IDI_SLDP_NXL,  IDI_SLDP },

};

static map<wstring, int> g_fileTypeTable =
{
	{ L".bmp",	0 * 2 },
	{ L".doc",	1 * 2 },
	{ L".docx", 2 * 2 },
	{ L".dwg",  3 * 2 },
	{ L".exe",  4 * 2 },
	{ L".gif",  5 * 2 },
	{ L".jpg",  6 * 2 },
	{ L".jt",	7 * 2 },
	{ L".*",	8 * 2 },
	{ L".nxl",  9 * 2 },
	{ L".pdf",  10 * 2 },
	{ L".png",  11 * 2 },
	{ L".ppt",  12 * 2 },
	{ L".pptx", 13 * 2 },
	{ L".rtf",  14 * 2 },
	{ L".tif",  15 * 2 },
	{ L".txt",  16 * 2 },
	{ L".vds",  17 * 2 },
	{ L".xls",  18 * 2 },
	{ L".xlsx", 19 * 2 },

    { L".3dxml", 20 * 2 },
    { L".catpart",	21 * 2 },
    { L".catshape", 22 * 2 },
    { L".cgr",  23 * 2 },
    { L".dxf",  24 * 2 },
    { L".ext",  25 * 2 },
    { L".file",  26 * 2 },
    { L".iges",	27 * 2 },
    { L".igs",	28 * 2 },
    { L".ipt",  29 * 2 },

    { L".par",  30 * 2 },
    { L".prt",  31 * 2 },
    { L".psm",  32 * 2 },
    { L".rh",   33 * 2 },
    { L".step",  34 * 2 },
    { L".stl",  35 * 2 },
    { L".stp",  36 * 2 },
    { L".tiff",  37 * 2 },
    { L".vsdx",  38 * 2 },
    { L".x_b",   39 * 2 },

    { L".xmt_txt",  40 * 2 },
    { L".x_t",  41 * 2 },
    { L".zip",  42 * 2 },
    { L".vsd",  43 * 2 },
    { L".sldasm",  44 * 2 },
    { L".sldprt",  45 * 2 }
};

RepoData a{ REPO_DROPBOX, IDI_DROPBOX_BLACK,IDI_DROPBOX_GRAY,IDB_DROPBOX, L"Dropbox", L"xyz@yahoo.com.com" };
RepoData b{ REPO_GOOGLE, IDI_GOOGLEDRIVE_BLACK,IDI_GOOGLEDRIVE_GRAY,IDB_GOOGLEDRIVE, L"Google Drive", L"abc@gmail.com" };
RepoData c{ REPO_NEXTLABS, IDI_MYDRIVE_BLACK,IDI_MYDRIVE_GRAY, IDB_MYDRIVE,L"MyDrive", L"xyz@abcdefggg.com" };
RepoData d{ REPO_ONEDRIVE, IDI_ONEDRIVE_BLACK,IDI_ONEDRIVE_GRAY, IDB_ONEDRIVE, L"OneDrive", L"xyz@msn.com" };
RepoData e{ REPO_SHAREPOINT, IDI_SHAREPOINT_BLACK,IDI_SHAREPOINT_GRAY, IDB_ONEDRIVE, L"Share Point Drive", L"xyz@microsoft.com" };

vector<RepoData>    repoArray{ a, b, c, d, e };

map<int, void*> iconTable =
{
	{ NX::RmRepository::UNKNOWNREPO , &c },
	{ NX::RmRepository::MYDRIVE, &c },
	{ NX::RmRepository::GOOGLEDRIVE, &b },
	{ NX::RmRepository::ONEDRIVE, &d },
	{ NX::RmRepository::DROPBOX, &a },
	{ NX::RmRepository::SHAREPOINTONLINE,&e },
	{ NX::RmRepository::SHAREPOINTONPREM,&e },
	{ NX::RmRepository::SHAREPOINTCROSSLAUNCH,&e },
	{ NX::RmRepository::SHAREPOINTONLINE_CROSSLAUNCH,&e }
};
typedef struct _SDOWNLOADINFO {
	DWORD dwindex;
	ULONGLONG ulright;
	BOOL	bflag;
	StatusCallBackFunc func;
	MSG	callbackMessage;
	std::vector<std::wstring> rightstr;
	std::wstring filepath;
	std::wstring filename;
	std::wstring destpath;
	std::wstring key;
	void *pParam;
} SDOWNLOADINFO, *PSDOWNLOADINFO;

typedef struct _SDWONLOADMYVAULT {
	vector<MyVaultFileInfo> *pfilelist;
	StatusCallBackFunc func;
	MSG	callbackMessage;
	std::wstring keyword;
}SDOWNLOADMYVALUT, *PSDOWNLOADMYVAULT;
void CancelProgressThreadCallBack();

static HANDLE			g_progThread = INVALID_HANDLE_VALUE;
static SDOWNLOADINFO	g_dwninfo;
//static HANDLE			g_myvalutThread = INVALID_HANDLE_VALUE;
static SDOWNLOADMYVALUT	g_dwnvalutinfo;
static std::thread		g_packagethrd;
static ProgressFuncs	g_progressFuncs = { CancelProgressThreadCallBack,0,0 , true, L"Download..." };
static bool				g_bCancel = false;
static bool				g_bExitFlag = false;

void CancelProgressThreadCallBack()
{
	if (g_progThread != INVALID_HANDLE_VALUE && g_progThread != NULL) {
		g_bCancel = TRUE;
		DWORD dwret = 0;
		int icount = 0;
		GetExitCodeThread(g_progThread, &dwret);
		while (dwret == STILL_ACTIVE) {
			if (icount >= 100) {//kill if it takes too long
				TerminateThread(g_progThread, 0);
				break;
			}
			icount++;
			Sleep(100);
			if (GetExitCodeThread(g_progThread, &dwret))
				break;
		}
		CloseHandle(g_progThread);
		g_progThread = INVALID_HANDLE_VALUE;
		g_progressFuncs.closeProgress();
	}
}

std::wstring FormatErrorMessage(NX::Result res, bool detail)
{
	wstringstream txtOut;
	txtOut << "code:" << res.GetCode();
	if (detail) {
		txtOut << ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction();
	}
	txtOut<< ", msg:" << res.GetMsg().c_str();
	return txtOut.str().c_str();
}

std::wstring FormatMessage(NX::Result res) {
	wstringstream txtOut;
	int retcode = res.GetCode();
	if (retcode > 12000 && retcode < 13000) {
		txtOut << "Cannot connect to the SkyDRM server.";
		return txtOut.str().c_str();
	}
	switch (retcode)
	{
	case 400:
		if (std::string::npos != res.GetMsg().find("JSON") || std::string::npos != res.GetMsg().find("Missing"))
			return theApp.LoadStringW(IDS_IMCOMPATIBLE_VERSION).GetBuffer()/*"Incompatible version."*/;
		else {
			txtOut << res.GetMsg().c_str();
			return txtOut.str().c_str();
		}
	case 403:
		return theApp.LoadStringW(IDS_ACCESS_DENIED).GetBuffer()/*"Access denied."*/;
	case 404:
		return theApp.LoadStringW(IDS_CANNOT_FIND_FILE).GetBuffer()/*L"SkyDRM can not find file."*/;
	case 500:
		return theApp.LoadStringW(IDS_SKYDRM_SERVER_ERROR).GetBuffer()/*L"SkyDRM server error."*/;
	case 4001:
		return theApp.LoadStringW(IDS_FILE_RIGHTS_REVOKED).GetBuffer()/*L"File rights have been revoked."*/;
	case 4002:
		return theApp.LoadStringW(IDS_OPERATION_FAILED).GetBuffer()/*L"Operation failed."*/;
	case ERROR_ACCESS_DENIED:
		return theApp.LoadStringW(IDS_ACCESS_DENIED).GetBuffer()/*L"Access Denied."*/;
	case ERROR_INVALID_DATA:
		return theApp.LoadStringW(IDS_INVALID_DATA).GetBuffer()/*L"Invalid data"*/;
	case ERROR_INVALID_HANDLE:
		return theApp.LoadStringW(IDS_INVALID_HANDLE).GetBuffer()/*L"Invalid handle"*/;
	case ERROR_NOT_SUPPORTED:
		return theApp.LoadStringW(IDS_API_NOT_SUPPORTED).GetBuffer()/*L"API not supported."*/;
	case ERROR_NOT_FOUND:
		return theApp.LoadStringW(IDS_CANNOT_FIND_ITEM).GetBuffer()/*L"Cannot find the item."*/;
	case ERROR_EXPIRED_HANDLE:
		return theApp.LoadStringW(IDS_HANDLE_EXPIRED).GetBuffer()/*L"Handle has expired."*/;
	case ERROR_NO_USER_SESSION_KEY:
		return theApp.LoadStringW(IDS_FILE_KEY_EXPIRED).GetBuffer()/*L"The file key has expired."*/;
	case ERROR_DATA_CHECKSUM_ERROR:
		return theApp.LoadStringW(IDS_INVALID_FILE).GetBuffer()/*L"Invalid file."*/;
	case ERROR_FILE_IDENTITY_NOT_PERSISTENT:
		return theApp.LoadStringW(IDS_TOKEN_NOT_MATCH).GetBuffer()/*L"Token does not match."*/;
	case ERROR_NO_TOKEN:
		return theApp.LoadStringW(IDS_SERVER_ERROR).GetBuffer()/*L"Server error."*/;
	case ERROR_INVALID_TOKEN:
		return theApp.LoadStringW(IDS_ERROR_OCCURRED_IN_OP).GetBuffer()/*L"An error occurred while performing this operation."*/;
	case ERROR_PATH_NOT_FOUND:
		return theApp.LoadStringW(IDS_PATH_NOT_FOUND).GetBuffer()/*L"Path not found."*/;
	case ERROR_DIRECTORY_NOT_SUPPORTED:
		return theApp.LoadStringW(IDS_OPERATION_NOT_SUPPORTED).GetBuffer()/*L"This operation not supported."*/;
	case ERROR_BAD_FILE_TYPE:
		return theApp.LoadStringW(IDS_INVALID_NXL_FILE).GetBuffer()/*L"Invalid .NXL file."*/;
	case ERROR_TIMEOUT:
		return theApp.LoadStringW(IDS_REQUEST_TIMEOUT).GetBuffer()/*L"Request timeout."*/;
	case ERROR_INVALID_DATATYPE:
		return theApp.LoadStringW(IDS_INVALID_DATA_TYPE).GetBuffer()/*L"Invalid data type."*/;
	case ERROR_FILE_NOT_FOUND:
		return theApp.LoadStringW(IDS_FILE_NOT_FOUND).GetBuffer()/*L"File not found."*/;
	case ERROR_NOT_ENOUGH_MEMORY:
		return theApp.LoadStringW(IDS_NOT_ENOUGH_MEMORY).GetBuffer()/*L"No enough memory."*/;
	case ERROR_EVT_INVALID_PUBLISHER_NAME:
		return theApp.LoadStringW(IDS_INTERNAL_ERROR).GetBuffer()/*L"Internal error."*/;
	case ERROR_FILE_EXISTS:
		return theApp.LoadStringW(IDS_FILE_ALREADY_EXISTS).GetBuffer()/*L"The file already exists."*/;
	default:
		if (retcode < 7000 && retcode > 400) { //other server return code
			txtOut << res.GetMsg().c_str();
			return txtOut.str().c_str();
		}
		break;
	}
	return FormatErrorMessage(res, false);
}
std::wstring OutputError(NX::Result &res, wstring errMsg)
{
	wstringstream txtOut;
	txtOut << errMsg.c_str()
		<< " ("<<FormatErrorMessage(res, true)
		<< ")" << std::endl;
	TRACE(L"%s",txtOut.str().c_str());
	//filter some special server code first.
	if (res.GetCode() == 401) {
		::PostMessage(theApp.m_mainhWnd, WM_COMMAND, ID_FILE_LOGOUT, 0);
		return theApp.LoadStringW(IDS_USER_SESSION_TIMEOUT).GetBuffer()/*L"User session timeout. Re-enter your login credentials."*/;
	}
	else if (res.GetCode() == 304) {
		return theApp.LoadStringW(IDS_NO_CHANGE_IS_MADE).GetBuffer()/*L"No change is made."*/;
	}
	else if (res.GetCode() == 415) {
		return theApp.LoadStringW(IDS_FILE_TYPE_NOT_SUPPORTED).GetBuffer()/*L"This file type is not supported."*/;
	}
	else if (res.GetCode() == 1223) {//user cancel return empty string
		return L"";
	}

	wstringstream msgOut;
	if (errMsg.length()) {
		msgOut << errMsg.c_str()
			<< " (" << FormatMessage(res).c_str()
			<< ")" << std::endl;
	}
	else {
		msgOut << FormatMessage(res).c_str() << std::endl;
	}
	return msgOut.str().c_str();
}

bool RemoveAddressFromVector(vector<wstring> &peopleArr, wstring resToken)
{
	for (auto iter = peopleArr.begin(); iter != peopleArr.end(); iter++)
	{
		if (*iter == resToken)
		{
			peopleArr.erase(iter);
			return true;
		}
	}
	return false;
}

extern CSize GetNonClientSize(HWND hWnd);

BOOL CnxrmRMCApp::CreateIconList(void)
{

	for (auto drv : iconIDArr)
	{
		HICON icon = LoadIcon(MAKEINTRESOURCE(drv.first));
		auto n = m_fileImageList.Add(icon);
		DestroyIcon(icon);

		icon = LoadIcon(MAKEINTRESOURCE(drv.second));
		n = m_fileImageList.Add(icon);
		DestroyIcon(icon);

	}

	auto icon = LoadIcon(MAKEINTRESOURCE(IDI_FOLDER));
	m_fileImageList.Add(icon);
	DestroyIcon(icon);
	icon = LoadIcon(MAKEINTRESOURCE(IDI_ICON_UPFOLDER));
	m_fileImageList.Add(icon);
	DestroyIcon(icon);

	return TRUE;
}

int CnxrmRMCApp::GetFileExtensionImageIndex(std::wstring fileext, BOOL bEncrypt)
{
	try
	{
		std::wstring strext = fileext;
		std::transform(strext.begin(), strext.end(), strext.begin(), towlower);
        if (bEncrypt && fileext == L"")
        {
            return 18;  //this is nxl file without extension
        }
        else
        {
            auto idxImg = g_fileTypeTable.at(strext);
            return bEncrypt ? idxImg : idxImg + 1;
        }

	}
	catch (...)
	{//Index 8 is default icon if nothing is matching.
		return bEncrypt ? 16 : 17;
	}
	return 0;
}

BOOL CnxrmRMCApp::IsProgressDlgRunning()
{
	if (g_progThread == INVALID_HANDLE_VALUE || g_progThread == NULL)
		return FALSE;
	DWORD dwret = 0;
	GetExitCodeThread(g_progThread, &dwret);
	if (dwret == STILL_ACTIVE) {
		return TRUE;
	}
	return FALSE;
}

BOOL CnxrmRMCApp::IsDialogPopup(void)
{
	return !CWnd::FromHandle(m_mainhWnd)->IsWindowEnabled() && !CWnd::FromHandle(m_mainhWnd)->IsIconic();

}


std::wstring CnxrmRMCApp::GetParentFolder(const std::wstring &folder)
{
	BOOL bendwithslash = FALSE;
	std::wstring path = folder;
	if (path.length() == 0)
		return L"/";
	if (path.back() == '/') {
		path.erase(path.length() - 1, 1);//remove the last /
		bendwithslash = TRUE;
	}
	size_t pos = path.find_last_of('/');
	if (pos == std::wstring::npos)
		return L"/";

	path.erase(pos, folder.length() - pos);

	if (bendwithslash) {
		path.append(L"/");
	}

	if (path.length() == 0)
		path = L"/";

	return path;
}

std::wstring CnxrmRMCApp::SerilaizeEmail(const std::vector<std::wstring> &emailArr) 
{
	std::wstring retstr;
	for (auto x : emailArr) {
		retstr.append(x);
		retstr.append(L";");
	}

	if (retstr.length() > 0) {
		retstr.erase(retstr.length()-1, 1);
	}

	return retstr;
}

std::vector<std::wstring> CnxrmRMCApp::DeserializeEmail(const std::wstring &emailstr)
{
	std::vector<std::wstring> retarr;
	retarr = NX::split<wchar_t, ';'>(emailstr, true);
	
	return retarr;
}
std::wstring CnxrmRMCApp::GetRouter()
{
    NX::win::RegUserKey key;
    NX::RmRouter router;

    NX::Result res = key.Open(regKeyNameRouter, true);
    if (!res)
        return NX::GetDefaultRouter().GetHostUrl();

    do {

        std::wstring name;
        res = key.ReadValue(L"CurrentRouter", name);
        if (res)
            router = NX::RmRouter::LoadFromRegistry(name);

    } while (FALSE);

    if (router.Empty())
        return NX::GetDefaultRouter().GetHostUrl();
    else
        return router.GetHostUrl();
}

std::wstring CnxrmRMCApp::GetTenant()
{
    NX::win::RegUserKey key;
    NX::RmTenant tenant;

    NX::Result res = key.Open(regKeyNameRouter, true);
    if (!res)
        return NX::GetDefaultTenant().GetName();

    do {

        std::wstring name;
        res = key.ReadValue(L"CurrentRouter", name);
        if (res)
            tenant = NX::RmTenant::LoadFromRegistry(name);

    } while (FALSE);

    if (tenant.Empty())
        return NX::GetDefaultTenant().GetName();
    else
        return tenant.GetName();
}

void CnxrmRMCApp::RefreshMyVaultInfo()
{
	RefreshHomePageMyDriveUsage();
	m_dlgRepoPage.SetMyVaultRefreshFlag();
	if (m_curShowedPage == MD_REPORITORYHOME) {
		if (m_dlgRepoPage.m_selBtnIdx != NX_BTN_MYDRIVE && m_dlgRepoPage.m_selBtnIdx != NX_BTN_OTHERREPOS) {//refresh immediately
			::PostMessage(m_dlgRepoPage.m_hWnd, WM_NX_REFRESh_MYVALUT_LIST, 0, 0);
		}
	}
}

void CnxrmRMCApp::RefreshHomePageMyDriveUsage()
{
	RestAPIRefreshUsage();
	m_homePage.UpdateStorageInfo();
}

void CnxrmRMCApp::RestAPIRefreshUsage()
{
    NX::RmMyDriveUsage usage;

    NX::Result res = theApp.m_pSession->MyDriveGetUsage(usage);

    if (res) {

        m_total = usage.GetQuota();
        m_used = usage.GetUsedBytes();
        m_myvaultused = usage.GetUsedBytesByMyVault();

    } 
    else 
    {
        TRACE( OutputError(res, L"GetUsageFailed").c_str());
    }
}

void CnxrmRMCApp::RestAPIRefreshUserInfo()
{
    m_usrName = theApp.m_pSession->GetCurrentUser().GetName();
    m_emailAddress = theApp.m_pSession->GetCurrentUser().GetEmail();
}

wstring CnxrmRMCApp::RestAPIGetCurrentUserID()
{
    return theApp.m_pSession->GetCurrentUser().GetId();
   
}
//For sleep 100 mill-seconds only
#define TWOMINUTE_COUNTER	60*2*10 

DWORD CALLBACK SyncRepoList(_In_opt_ PVOID lpData)
{
	int icount = 0;
	while (1) {
		Sleep(100);
		if (theApp.m_bExitThread)
			break;
		if (theApp.m_bPauseThread) {
			continue;
		}
		icount++;
		if (icount >= TWOMINUTE_COUNTER) {
			icount = 0;
			theApp.RefreshRepositoryList(TRUE);
		}
	}
	return 0;
}

DWORD CALLBACK SyncMyDriveFile(_In_opt_ PVOID lpData)
{
	int icount = 0;
	while (1) {
		Sleep(100);
		if (theApp.m_bExitThread)
			break;
		if (theApp.m_bPauseThread) {
			continue;
		}
		icount++;
		if (icount >= TWOMINUTE_COUNTER) {//two minutes
			if (!theApp.RefreshMyDriveFileList())
				icount -= 20;//try refresh after 2 seconds.
			else
				icount = 0;
		}
	}
	return 0;
}

DWORD CALLBACK SyncInvitationList(_In_opt_ PVOID lpData)
{
	int icount = 1200; //start sync immediately
	while (1) {
		Sleep(100);
		if (theApp.m_bExitThread)
			break;
		if (theApp.m_bPauseThread) {
			continue;
		}
		icount++;
		if (icount >= TWOMINUTE_COUNTER) {//two minutes
			if (!theApp.RefreshInvatationList())
				icount -= 20;//try refresh after 2 seconds.
			else
				icount = 0;
		}
	}
	return 0;
}

DWORD CALLBACK SyncProjectList(_In_opt_ PVOID lpData)
{
    const int ONEMINUTE_COUNTER = TWOMINUTE_COUNTER / 2;
	int icount = ONEMINUTE_COUNTER; //start sync immediately
	while (1) {
		Sleep(100);
		if (theApp.m_bExitThread)
			break;
		if (theApp.m_bPauseThread) {
			continue;
		}
		icount++;
		if (icount >= ONEMINUTE_COUNTER) {//two minutes
			if (!theApp.RefreshProjectList())
				icount -= 20;//try refresh after 2 seconds.
			else
				icount = 0;
		}
	}
	return 0;
}

CnxrmRMCApp::CnxrmRMCApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	m_hMainMenu = NULL;
	m_curShowedPage = MD_UNKNOWN;
	m_plogonDlg = NULL;
	m_bHideProjectMenu = FALSE;
	m_thrdMyDriveFile = INVALID_HANDLE_VALUE;
	m_thrdRepoList = INVALID_HANDLE_VALUE;
	m_thrdInviteList = INVALID_HANDLE_VALUE;
	m_thrdProjectList = INVALID_HANDLE_VALUE;
	m_bExitThread = FALSE;
	m_bPauseThread = TRUE;
	m_strMyDrivePath = L"/";
	m_MyDriveLastQueryTime = 0;
    m_hSingleInstanceMutex = NULL;
    m_curProjID = L"";

	// Place all significant initialization in InitInstance
}


// The one and only CnxrmRMCApp object

CnxrmRMCApp theApp;


// CnxrmRMCApp initialization

BOOL CnxrmRMCApp::DestroyTrayIcon(HWND hMainWnd)
{
	m_nid.uID = TRAY_ID;
	m_nid.hWnd = hMainWnd;

	//if (g_APPUI.hUserNameTip)
	//{
	//	DestroyWindow(g_APPUI.hUserNameTip);
	//}

	//if (g_APPUI.hPolicyNameTip)
	//{
	//	DestroyWindow(g_APPUI.hPolicyNameTip);
	//}

	return Shell_NotifyIcon(NIM_DELETE, &m_nid);
}

bool CnxrmRMCApp::InitTrayIcon(HWND hMainWnd, HINSTANCE hInstance)
{
	int   iRet;

	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hWnd = hMainWnd;
	m_nid.uID = TRAY_ID;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	m_nid.uCallbackMessage = WM_TRAY;
	m_nid.uTimeout = 1000;                   //The timeout value, in milliseconds, for notification.
	static const GUID BalloonGUID = { 0xb113fe22, 0x21a1, 0x43a8,{ 0x8c, 0x62, 0xfb, 0x22, 0x13, 0xd, 0x3, 0x94 } };
	m_nid.guidItem = BalloonGUID;
	m_nid.dwInfoFlags = NIIF_USER;
	m_nid.uVersion = NOTIFYICON_VERSION_4;

	m_nid.hIcon = (HICON)LoadImage( // returns a HANDLE so we have to cast to HICON
		hInstance,             // hInstance must be NULL when loading from a file
		MAKEINTRESOURCE(IDI_TRAY),    // the icon RESOURCE
		IMAGE_ICON,       // specifies that the file is an icon
		0,         // width of the image (we'll specify default later on)
		0,         // height of the image
		LR_DEFAULTSIZE
	);

	// Tray Icon Tip
	//std::wstring wsTipFmt = NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)IDS_TRAY_TIP, 1024, g_APPState.dwLangId);
	wchar_t szTitle[256];
	GetWindowText(hMainWnd, szTitle, 256);
	swprintf_s(m_nid.szTip, 128, szTitle);
	//// Message bubble title
	//std::wstring bubble_title = NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)IDS_PRODUCT_NAME, 1024, g_APPState.dwLangId);
	lstrcpy(m_nid.szInfoTitle, theApp.LoadStringW(IDS_INFO_TITLE)/*L"Info Title"*/);

	//TODO: Show the tray icon
	Shell_NotifyIconW(NIM_DELETE, &m_nid);    // Always remove tray-icon before we add a new one
	iRet = Shell_NotifyIcon(NIM_ADD, &m_nid);
	if (FALSE == iRet)
	{

		return false;
	}
	return true;
}


void CnxrmRMCApp::ShowTrayMsg(_In_ LPCWSTR wzInfo, _In_opt_ LPCWSTR wzTitle)
{
	std::wstring bubble_title((NULL == wzTitle) ? theApp.LoadStringW(IDS_APP_TITLE)/*L"SkyDRM"*/ : wzTitle);
	//if (bubble_title.empty()) {
	//	bubble_title = NX::RES::LoadMessage(g_APPUI.hResDll, (DWORD)IDS_PRODUCT_NAME, 1024, g_APPState.dwLangId, L"SkyDRM");
	//}
	if (wcslen(wzInfo) == 0)
		return;
	//if (g_APPState.bPopupNotify) {
		theApp.m_nid.uFlags |= NIF_INFO;
		wcsncpy_s(theApp.m_nid.szInfoTitle, 63/sizeof(WCHAR), bubble_title.c_str(), _TRUNCATE);
		wcsncpy_s(theApp.m_nid.szInfo, 255/sizeof(WCHAR), wzInfo, _TRUNCATE);
		theApp.m_nid.hIcon = (HICON)LoadImage( // returns a HANDLE so we have to cast to HICON
			AfxGetInstanceHandle(),             // hInstance must be NULL when loading from a file
			MAKEINTRESOURCE(IDI_TRAY),    // the icon RESOURCE
			IMAGE_ICON,       // specifies that the file is an icon
			48,         // width of the image (we'll specify default later on)
			48,         // height of the image
			LR_DEFAULTCOLOR
		);
		Shell_NotifyIcon(NIM_MODIFY, &theApp.m_nid);
		theApp.m_nid.uFlags &= (~NIF_INFO);
		theApp.m_nid.szInfoTitle[0] = L'\0';
		theApp.m_nid.szInfo[0] = L'\0';
	//}
}

void CnxrmRMCApp::ShowSignup()
{
	if (theApp.m_plogonDlg) {
		return;
	}
	if (NULL == m_plogonDlg) {
		theApp.m_plogonDlg = new CMainDlg(AfxGetInstanceHandle(), theApp.m_mainhWnd, IDD_DIALOG_WEBLOGON, IsWindows10OrGreater());
		RemoveMenu(theApp.m_hMainMenu, ID_FILE_LOGIN, MF_BYCOMMAND);
	}
	auto pRMSServer = theApp.m_pSession->GetCurrentRMS();
	auto tenantName = theApp.m_pSession->GetCurrentRMS().GetTenantName();
	auto szUrl = pRMSServer.GetUrl();
	wstringstream txtOut;
	txtOut << szUrl.c_str() << L"/rms/Register.jsp?tenant=" << tenantName.c_str();
	INT_PTR nRet = theApp.m_plogonDlg->LogOn(txtOut.str().c_str(), L"no_url_data", theApp.m_pSession->GetClientId().GetClientId());
	m_curShowedPage = MD_SIGNUP;
}

void CnxrmRMCApp::ShowLogin()
{
	if (theApp.m_plogonDlg) {
		return;
	}
	theApp.m_dlgIntro.ShowWindow(SW_HIDE);
	theApp.m_plogonDlg = new CMainDlg(AfxGetInstanceHandle(), theApp.m_mainhWnd, IDD_DIALOG_WEBLOGON, IsWindows10OrGreater());
	RemoveMenu(theApp.m_hMainMenu, ID_FILE_LOGIN, MF_BYCOMMAND);
	auto pRMSServer = theApp.m_pSession->GetCurrentRMS();
	auto tenantName = theApp.m_pSession->GetCurrentRMS().GetTenantName();
	auto szUrl = pRMSServer.GetUrl();
	wstringstream txtOut;
	txtOut << szUrl.c_str() << L"/rms/Login.jsp?tenant=" << tenantName.c_str();
	INT_PTR nRet = theApp.m_plogonDlg->LogOn(txtOut.str().c_str(), L"no_url_data", theApp.m_pSession->GetClientId().GetClientId());
	m_curShowedPage = MD_LOGIN;
}

void CnxrmRMCApp::ShowPage(MAINUI_DIAGLOGLIST pageid)
{
	theApp.m_curShowedPage = pageid;
	theApp.m_dlgIntro.ShowWindow((pageid == MD_INTRODUCTION) ? SW_SHOW : SW_HIDE);
	theApp.m_homePage.ShowWindow((pageid == MD_HOMEPAGE)?SW_SHOW : SW_HIDE);
	theApp.m_dlgRepoPage.ShowWindow((pageid == MD_REPORITORYHOME) ? SW_SHOW: SW_HIDE);
	theApp.m_dlgRepoPage.ResetMySpaceButton();
	theApp.m_dlgProjPage.ShowWindow((pageid == MD_PROJECTSHOME) ? SW_SHOW : SW_HIDE);
    theApp.m_dlgProjMain.ShowWindow((pageid == MD_PROJECTMAIN) ? SW_SHOW : SW_HIDE);
    theApp.m_dlgProjIntro.ShowWindow((pageid == MD_PROJECTINTRO) ? SW_SHOW : SW_HIDE);
    OnVScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.
    OnHScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.

	if (pageid == MD_INTRODUCTION || pageid == MD_LOGIN) {
		m_bPauseThread = TRUE;
		g_bCancel = TRUE;
		g_bExitFlag = TRUE;//exit all threads; need set two flags because the function may use different flag.
	}
	else {
		m_bPauseThread = FALSE;
	}

    CWnd *pActiveWnd = NULL;
    switch (pageid)
    {
    case MD_INTRODUCTION:
    {
        pActiveWnd = &theApp.m_dlgIntro;
        CRect rcMainWnd;
        GetClientRect(theApp.m_mainhWnd, &rcMainWnd);
        ShowWelcomeIntroWindow(theApp.m_mainhWnd, rcMainWnd.Width(), rcMainWnd.Height());
    }
        break;
    case MD_HOMEPAGE:
        //theApp.m_homePage.OnRefreshProjMessage(0, 0);
        pActiveWnd = &theApp.m_homePage;
        break;
    case MD_REPORITORYHOME:
        pActiveWnd = &theApp.m_dlgRepoPage;
        break;
    case MD_PROJECTSHOME:
        //theApp.m_dlgProjPage.On
        pActiveWnd = &theApp.m_dlgProjPage;
        break;
    case MD_PROJECTMAIN:
        pActiveWnd = &theApp.m_dlgProjMain;
        break;
    case MD_PROJECTINTRO:
    {
         pActiveWnd = &theApp.m_dlgProjIntro;

         CRect rcMainWnd;
         GetClientRect(theApp.m_mainhWnd, &rcMainWnd);
         ShowProjIntroWindow(theApp.m_mainhWnd, rcMainWnd.Width(), rcMainWnd.Height());
    }
        break;
    }
    if (pActiveWnd)
    {
        CRect rc;
		pActiveWnd->GetWindowRect(&rc);
		if (pageid == MD_INTRODUCTION) {
			//pActiveWnd->CenterWindow();
		}
		else if (pageid == MD_REPORITORYHOME) {
			theApp.m_dlgRepoPage.m_switchDlg.ShowWindow(SW_HIDE);
			GetWindowRect(theApp.m_mainhWnd, &rc);
			CSize ncSize = GetNonClientSize(theApp.m_mainhWnd);
			m_dlgRepoPage.ResizeWindow(rc.right - rc.left - ncSize.cx, rc.bottom - rc.top - ncSize.cy);
            CRect rect = { 0,0,400, 400 };
            InitScrollBars(theApp.m_mainhWnd, rect);
		}
		else if (pageid == MD_HOMEPAGE) {
			pActiveWnd->ScreenToClient(&rc);
			CRect winrc;
			CWnd::FromHandle(theApp.m_mainhWnd)->GetWindowRect(&winrc);
			pActiveWnd->MoveWindow((winrc.Width() - rc.Width()) / 2, rc.top, rc.Width(), rc.Height(), FALSE);
			CWnd::FromHandle(theApp.m_mainhWnd)->ScreenToClient(&rc);
			InitScrollBars(theApp.m_mainhWnd, rc);
		}
        else if (pageid == MD_PROJECTSHOME) {
            CRect winrc;
            CWnd::FromHandle(theApp.m_mainhWnd)->GetClientRect(&winrc);
            ResizeProjPage(theApp.m_mainhWnd, winrc.Width(), winrc.Height());

        }
		else if (pageid != MD_PROJECTMAIN) 
        {
			CWnd::FromHandle(theApp.m_mainhWnd)->ScreenToClient(&rc);
			InitScrollBars(theApp.m_mainhWnd, rc);
        }
	}
}

BOOL CnxrmRMCApp::InitInstance()
{
	// Set our application's AppUserModelID.  This ID needs to be the same as
	// the one set by nxrviewer.exe.  This way the Windows taskbar will
	// recognize that these two .exe's belong to the same application, and
	// group their windows together under a single taskbar button.
	HRESULT hResult = SetCurrentProcessExplicitAppUserModelID(L"NextLabs.SkyDRM");
	if (!SUCCEEDED(hResult))
	{
		wstringstream txtOut;
		txtOut << "Failed to set AppUserModelID, hResult=" << hResult << std::endl;
		TRACE(L"%s", txtOut.str().c_str());
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	//CShellManager *pShellManager = new CShellManager;

	//// Activate "Windows Native" visual manager for enabling themes in MFC controls
	//CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("SkyDRM"));


	CString className;
	className.LoadStringW(IDC_NXRMC);
	CString winName;
	winName.LoadStringW(IDS_APP_TITLE);

	m_hAccel = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDC_NXRMC));
	m_fileImageList.Create(32, 32, ILC_COLOR32, 0, 100);
	CreateIconList();
	if (lstrlen(theApp.m_lpCmdLine))
	{ //Here we are working on the file.
		LPWSTR *szArglist = NULL;
		int nArgs;
		szArglist = CommandLineToArgvW(theApp.m_lpCmdLine, &nArgs);
		m_strArgOpenFileName = szArglist[0];
		if (!PathFileExists(m_strArgOpenFileName.c_str())) {
			m_strArgOpenFileName.empty();
		}
		LocalFree(szArglist);
		szArglist = NULL;
	}
	m_hSingleInstanceMutex = CreateMutex(NULL, TRUE, L"nxrmc_SingleInstanceMutex");
	DWORD dwError = GetLastError();
    if (dwError == ERROR_ALREADY_EXISTS)
    {
        auto hMainWnd = FindWindow(className, winName);
        if (hMainWnd)
        {
            if (m_strArgOpenFileName.length())
            {
                static COPYDATASTRUCT copyData;
                HWND hActiveWnd = GetActiveWindow();
                copyData.dwData = NxCmdView;
                copyData.cbData = (DWORD)(m_strArgOpenFileName.length() + 1) * 2;
                copyData.lpData = (PVOID)m_strArgOpenFileName.c_str();
                //must use SendMessage to send it to exist window
                SendMessage(hMainWnd, WM_COPYDATA, (WPARAM)hActiveWnd, (LPARAM)&copyData);
			}
            else
            {
                SendMessage(hMainWnd, WM_TRAY, 0, WM_LBUTTONDBLCLK);
            }
            return FALSE;
        } 
        else
        {
            return FALSE;
        }
    }

    static CnxrmRMCDlg dlg;
    m_pMainWnd = &dlg;

    INT_PTR nResponse = dlg.Create(MAKEINTRESOURCE(IDD_SPLASH));
    dlg.ShowWindow(SW_SHOW);
	
	m_thrdRepoList = ::CreateThread(NULL, 0, SyncRepoList, NULL, 0, NULL);
	m_thrdMyDriveFile = ::CreateThread(NULL, 0, SyncMyDriveFile, NULL, 0, NULL);
	m_thrdInviteList = ::CreateThread(NULL, 0, SyncInvitationList, NULL, 0, NULL);
	m_thrdProjectList = ::CreateThread(NULL, 0, SyncProjectList, NULL, 0, NULL);
	return TRUE;
}

void CnxrmRMCApp::StopRunningThread(HANDLE h)
{
	int icount = 0;
	if (h != INVALID_HANDLE_VALUE && h != NULL) {
		DWORD dwret = 0;
		GetExitCodeThread(h, &dwret);
		while (dwret == STILL_ACTIVE) {
			if (icount >= 5) {
				ASSERT(TRUE);
				TerminateThread(h, 0);
				break;
			}
			icount++;
			Sleep(1000);
			if (GetExitCodeThread(h, &dwret))
				break;
		}
		CloseHandle(h);
		h = NULL;
	}
}
BOOL CnxrmRMCApp::ExitInstance()
{
	//delete m_ps;
	m_bExitThread = TRUE;
    Gdiplus::GdiplusShutdown(m_gdiplusToken);
	g_bCancel = TRUE;
	g_bExitFlag = true;
	int icount = 0;

	while (IsProgressDlgRunning()) {
		if (icount >= 5) {
			TerminateThread(g_progThread, 0);
			break;
		}
		icount++;
		Sleep(1000);
	}
	if (g_progThread != INVALID_HANDLE_VALUE && g_progThread != NULL) {
		CloseHandle(g_progThread);
	}
	m_fileImageList.DeleteImageList();
	StopRunningThread(m_thrdRepoList);
	StopRunningThread(m_thrdMyDriveFile);
	StopRunningThread(m_thrdInviteList);
	StopRunningThread(m_thrdProjectList);


	theApp.m_pSession.reset();
    CloseHandle(m_hSingleInstanceMutex);

    return 0;
}

BOOL CnxrmRMCApp::InitializeWindows(CWnd * parentWindow)
{
	BOOL bret;
	if (!IsWindow(m_homePage.m_hWnd))
		bret = m_homePage.Create(IDD_HOME1, parentWindow);
	else
		bret = TRUE;
	if(!IsWindow(m_dlgProjPage.m_hWnd))
		bret &= m_dlgProjPage.Create(IDD_PROJECT, parentWindow);

	if(!IsWindow(m_dlgRepoPage.m_hWnd))
		bret &= m_dlgRepoPage.Create(IDD_FILE_LIST, parentWindow);

    if (!IsWindow(m_dlgProjMain.m_hWnd))
        bret &= m_dlgProjMain.Create(IDD_RMCMAIN_PROJECT, parentWindow);

    if (!IsWindow(m_dlgProjIntro.m_hWnd))
    {
        m_dlgProjIntro.SetProjFlag();
        bret &= m_dlgProjIntro.Create(IDD_PROJECT_INTRO, parentWindow);
    }


	return bret;
}

BOOL CnxrmRMCApp::InitializeRestfulData(void)
{
	//reset cancel flags of thread.
	g_bExitFlag = FALSE;
	g_bCancel = FALSE;
	
	if (!InitializeProjData())
		return FALSE;
	if (!RefreshRepositoryList())
		return FALSE;
    RestAPIRefreshUsage();
    RestAPIRefreshUserInfo();

	return TRUE;
}

DWORD CALLBACK OpenProjFileProc(_In_opt_ PVOID lpData)
{
	if (g_dwninfo.filename.size() != 0)
    {
        //auto pos = fileName.find_last_of(L'/');

        //auto fname = fileName.substr(pos + 1);
        wstring fullPath;
        //auto res = theApp.m_pSession->ProjectDownloadFile(projData.m_proj.GetId(), fileName, fullPath, false);
        //auto res = theApp.m_pSession->ProjectDownloadFileForViewer(g_dwninfo.key, g_dwninfo.filename, fullPath, &g_bCancel);
        auto res = theApp.m_pSession->ViewProjectFile(g_dwninfo.key, g_dwninfo.filename, NULL);
        if (!res) {
            {
                wstringstream txtOut;
                txtOut << theApp.LoadStringW(IDS_FAIL_TO_VIEW_FILE_PREFIX).GetBuffer()/*L"Fail to view file: "*/ << g_dwninfo.filepath;;
                theApp.ShowTrayMsg(OutputError(res, txtOut.str()).c_str());
            }
        }
    }
    g_progressFuncs.closeProgress();

	return 0;
}


void CnxrmRMCApp::OpenProjFile(const CProjData & projData, const wstring fileName, wstring dispName)
{
	g_dwninfo.filename = fileName;
	g_dwninfo.filepath = dispName;
	g_dwninfo.key = projData.m_nID;
    g_dwninfo.pParam = (void*)&projData;
	if (IsProgressDlgRunning())
		return;
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;
	
	g_progressFuncs.titleText = theApp.LoadStringW(IDS_PROCESS).GetBuffer()/*L"Process..."*/;
	if (RmuOpenProgressWindow(theApp.m_mainhWnd, g_progressFuncs, true))
    {
		g_bCancel = FALSE;
		g_progThread = ::CreateThread(NULL, 0, OpenProjFileProc, &g_dwninfo, 0, NULL);
    }
}


bool CnxrmRMCApp::LocalFileView(const std::wstring& file, const std::wstring& displayName)
{
    NX::Result res = theApp.m_pSession->ViewLocalFile(file, displayName, &g_bExitFlag);
    if (!res) {
		wstringstream txtOut;
        CString fullText;
        if (0 == displayName.length())
        {
            fullText.Format(IDS_FMT_S_FAILED_OPEN_FILE_FOR_VIEW, file.c_str()); /*L"Failed to open the file " << file << L" for viewing. "*/
        }
        else
        {
            fullText.Format(IDS_FMT_S_FAILED_OPEN_FILE_FOR_VIEW, displayName.c_str()); /*L"Failed to open the file " << displayName << L" for viewing. "*/
        }
		txtOut << fullText.GetBuffer();

		theApp.ShowTrayMsg(OutputError(res, txtOut.str()).c_str());
        return false;
    }
    return true;
}


bool CnxrmRMCApp::InitializeProjData()
{
	bool bret = true;
	{//initialize project list for home page
		lock_guard<mutex> lock(m_mutProject);
		m_ProjectByMeCount = 0;
		m_ProjectByOthersCount = 0;
		m_TotalProjectByMe = 0;
		m_TotalProjectByOthers = 0;

		m_AllProjects.clear();
		std::vector<NX::RmProject> projects;
		{
			NX::RmFinder finder(6, std::vector<int>({ NX::OrderByLastActionTimeDescend , NX::OrderByNameAscend }), L"", L"", L"");
			BOOL bupdate = FALSE;
			auto res = m_pSession->ProjectListFirstPageProjects(finder, projects, &g_bExitFlag, NX::RmSession::PROJECT_OWNER_TYPE::OwnedByMe);
			if (!res) {
				TRACE( OutputError(res, L"Fail to list projects").c_str());
				bret = false;
			}
			else
			{
				for (auto project : projects) {
					m_AllProjects.push_back(project);
				}
				m_ProjectByMeCount = projects.size();
				m_TotalProjectByMe = (size_t) finder.GetTotalRecords();
			}
		}

		projects.clear();
		{
			NX::RmFinder finder(6, std::vector<int>({ NX::OrderByLastActionTimeDescend , NX::OrderByNameAscend }), L"", L"", L"");
			BOOL bupdate = FALSE;
			auto res = m_pSession->ProjectListFirstPageProjects(finder, projects, &g_bExitFlag, NX::RmSession::PROJECT_OWNER_TYPE::OwnedByOthers);
			if (!res) {
				TRACE( OutputError(res, L"Fail to list projects").c_str());
				bret = false;
			}
			else
			{
				for (auto project : projects) {
					m_AllProjects.push_back(project);
				}
				m_ProjectByOthersCount = projects.size();
				m_TotalProjectByOthers = (size_t) finder.GetTotalRecords();
			}

		}
	}



	lock_guard<mutex> lock(m_mutInvitation);
	NX::Result nres = RESULT(2);
	m_InvatationList.clear();
	nres = theApp.m_pSession->ProjectGetUserPendingInvitations(m_InvatationList, &g_bExitFlag);
	if (!nres) {

		TRACE( OutputError(nres, L"Fail to list project invites").c_str());
		return false;
	}

	std::sort(m_InvatationList.begin(), m_InvatationList.end(),
		[](NX::RmProjectInvitation &a, const NX::RmProjectInvitation &b)
	{
		return a.GetInviteTime() > b.GetInviteTime();
	});


	return bret;
}

ULONGLONG CnxrmRMCApp::StringRightsToUll(const NXSharingData &dataOut)
{
	ULONGLONG ull = 0;
	ull |= dataOut.bView ? BUILTIN_RIGHT_VIEW : 0;
	ull |= dataOut.bPrint ? BUILTIN_RIGHT_PRINT : 0;
	ull |= dataOut.bShare ? BUILTIN_RIGHT_SHARE : 0;
	ull |= dataOut.bDownload ? BUILTIN_RIGHT_DOWNLOAD : 0;
	ull |= dataOut.bWaterMark ? BUILTIN_OBRIGHT_WATERMARK : 0;
	return ull;
}

std::vector<std::wstring> CnxrmRMCApp::StringRightsToVector(const NXSharingData & dataOut)
{
    std::vector<std::wstring>  rights;;
    if(dataOut.bView) rights.push_back(RIGHT_ACTION_VIEW);
    if (dataOut.bPrint) rights.push_back(RIGHT_ACTION_PRINT);
    if (dataOut.bShare) rights.push_back(RIGHT_ACTION_SHARE);
    if (dataOut.bDownload) rights.push_back(RIGHT_ACTION_DOWNLOAD);
    if (dataOut.bWaterMark) rights.push_back(OBLIGATION_NAME_WATERMARK);

    return move(rights) ;
}


BOOL CnxrmRMCApp::OpenAddRepositoryUI(const HWND hWnd)
{
	CConnectToRepo dlg(CWnd::FromHandle(((NULL == hWnd) ? m_mainhWnd : hWnd)));
	dlg.DoModal();
	if (dlg.m_needRefresh)
	{
		RefreshRepositoryList();
	}

	return dlg.m_needRefresh;
}

BOOL CnxrmRMCApp::OpenDeleteRepositoryUI(int repoindex, const HWND hwnd /* = NULL */)
{
	CRepoEditor dlg(CWnd::FromHandle(hwnd));
	RepositoryInfo info;
	GetRepositoryInfo(repoindex, info);
	dlg.m_bDisconnectOnly = TRUE;
	dlg.SetCurrentRepository(info);
	if (dlg.DoModal() == IDOK)
	{
		RefreshRepositoryList();
		return TRUE;
	}

	return FALSE;
}

BOOL CnxrmRMCApp::OpenRenameRepositoryUI(int repoindex, const HWND hwnd /* = NULL */)
{
	CRepoEditor dlg(CWnd::FromHandle(hwnd));
	RepositoryInfo info;
	GetRepositoryInfo(repoindex, info);
	dlg.SetCurrentRepository(info);
	dlg.m_bRenameOnly = TRUE;
	if (dlg.DoModal() == IDOK)
	{
		RefreshRepositoryList();
		return TRUE;
	}

	return FALSE;
}

BOOL CnxrmRMCApp::OpenProtectDlg(const HWND hWnd, int index, const RepositoryFileInfo &repoFile)
{
	BOOL bret = FALSE;
	ASSERT(index < (int)m_RepositoryList.size());
	
	NX::RmRepository rmrepo;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (index >= (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[index];
	}

	NXSharingData dataIn = { true, false, false, false, false, false, false, DLGTYPE_PROTECT,0 };
	NXSharingData dataOut;

	if (repoFile.bIsNXL)
	{
		dataIn.bView = true;
	}
	auto fname = std::wstring(L"\\") + repoFile.strFileName;
	LPWSTR pAddresses;
	if (!RmuShowShareDialog(hWnd, fname.c_str(), dataIn, &dataOut, &pAddresses))
	{
		vector<wstring> peopleArr;
		peopleArr.clear();

		//auto idx = m_RepoList.GetCurSel();
		//auto repo = (*m_reposArr)[idx];

		auto ullRights = StringRightsToUll(dataOut);
		NX::RmFileShareResult sr;
		NX::RmRepoFile rfile(repoFile.strPath, repoFile.strDisplayPath, repoFile.strFileName, (repoFile.bIsDirectory == TRUE), repoFile.iFileSize, repoFile.lFileDate);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		auto res = theApp.m_pSession->RepoProtectFile(theApp.m_pSession->GetCurrentUser().GetMemberships().at(0).GetId(),
			rmrepo,
			rfile,
			ullRights,
			NX::NXL::FileMeta(),
			NX::NXL::FileTags(),
			sr,
			&g_bExitFlag
		);
		if (!res) {
			ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_PROTECT_REPO_FILE).GetBuffer()/*L"Failed to protect the repository file."*/).c_str());
		}
		else {
			DisplayMyVaultInfo(sr, FALSE);
			bret = TRUE;
		}
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}

	return bret;
}

BOOL CnxrmRMCApp::ShowMyVaultFileProperty(const HWND hWnd, MyVaultFileInfo & fileinfo, HICON fileicon, BOOL bviewonly)
{
	BOOL bret = FALSE;
	auto duid = fileinfo.strDuid;
	NX::RmMyVaultFileMetadata metadata;
	NX::Result res = m_pSession->MyVaultGetFileInfo(duid, fileinfo.rFileInfo.strPath, metadata);
	if (!res) {
		ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_GET_FILE_INFO).GetBuffer()/*L"Failed to get file information."*/).c_str());
		return bret;;
	}
	if (bviewonly) {
		CDlgFileInfo dlg(CWnd::FromHandle(hWnd));

		dlg.m_fileRights = metadata.GetRights();
		dlg.m_bOwner = TRUE;

		dlg.m_strFileName = fileinfo.rFileInfo.strFileName.c_str();
		dlg.m_strFileSize = fileinfo.rFileInfo.strFileSize.c_str();
		dlg.m_strFileModify = fileinfo.rFileInfo.strFileDate.c_str();
		dlg.m_strFilePath = fileinfo.strOriginalPath.c_str();

		dlg.DoModal();
	}
	else {
		CManageFileDlg dlg(CWnd::FromHandle(hWnd), !fileinfo.bIsShared && !fileinfo.bIsRevoked && !fileinfo.bIsDeleted);
		dlg.m_bRevoked = fileinfo.bIsRevoked;
		dlg.m_bDeleted = fileinfo.bIsDeleted;
		dlg.m_bViewOnly = bviewonly;
		dlg.m_bOwner = TRUE;

		dlg.m_fileRights = metadata.GetRights();
		dlg.m_strFileName = fileinfo.rFileInfo.strFileName.c_str();
		if (fileinfo.strSharedWith.length() != 0) {
			dlg.m_strSharedWithEmail = fileinfo.strSharedWith.c_str();
			if(0 == fileinfo.strSharedArr.size()){
				NX::RmMyVaultFileMetadata metadata;
				res = theApp.m_pSession->MyVaultGetFileInfo(fileinfo.strDuid, fileinfo.rFileInfo.strPath, metadata);
				if (res) {
					fileinfo.strSharedArr = metadata.GetRecipients();
				}
				else  {
					if (!g_bCancel) {
						CString err;
                        err.Format(IDS_FMT_S_FAILED_TO_SYNC_FILE_INFO, fileinfo.rFileInfo.strFileName.c_str());
                            /*L"Fail to sync file information:"*/ 
						theApp.ShowTrayMsg(OutputError(res, err.GetBuffer()).c_str());
					}
					return false;
				}
			}
		}
		else {
			dlg.m_strSharedWithEmail = theApp.LoadStringW(IDS_NONE)/*L"None"*/;
		}
		if (fileinfo.bIsRevoked || fileinfo.bIsDeleted || fileinfo.bIsShared) {
			dlg.m_iSharedMembers = (int)fileinfo.strSharedArr.size();
			dlg.m_strSharedWithEmail = SerilaizeEmail(fileinfo.strSharedArr).c_str();
		}

		std::wstring filename = fileinfo.rFileInfo.strFileName.c_str();
		if (fileinfo.rFileInfo.bIsNXL) {//remove .nxl extension
			filename.erase(filename.end() - 4, filename.end());
		}

		if (dlg.DoModal() == IDOK) {//revoke all rights
			if (dlg.m_bManage) {
				LPWSTR pstremail = NULL;
				if (!RmuShowManageUsersDialog(hWnd, dlg.m_strFileName.GetBuffer(), fileinfo.strSharedArr, &pstremail)) {
					std::vector<std::wstring> addemail;
					std::vector<std::wstring> removeemail;
					std::vector<std::wstring> retadded;
					std::vector<std::wstring> retremoved;
					if (NULL == pstremail) {//remove all
						removeemail = fileinfo.strSharedArr;
						addemail.clear();
					}
					else {
						std::wstring emailstr = pstremail;
						RmuFreeResource(pstremail);
						addemail = theApp.DeserializeEmail(emailstr);
						for (auto x : fileinfo.strSharedArr) {
							BOOL bfound = FALSE;
							for (std::vector<std::wstring>::iterator Iter = addemail.begin(); Iter != addemail.end(); Iter++) {
								if (0 == x.compare(*Iter)) {//found same item. remove from add list.
									addemail.erase(Iter);
									bfound = TRUE;
									break;
								}
							}

							if (!bfound)//add it to remove list
								removeemail.push_back(x);
						}
					}
					res = m_pSession->MyVaultUpdateShareRecipients(duid, addemail, removeemail, retadded, retremoved);
					if (!res) {
						theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_ERROR_UPDATE_RECEPIENTS).GetBuffer()/*L"An error occurred while updating the recipients."*/).c_str());
					}
					else
					{
						theApp.ShowTrayMsg(theApp.LoadStringW(IDS_SUCCEED_CHANGED_RECEPIENTS)/*L"Successfully changed the recipients of the file."*/);
						bret = TRUE;
					}
				}
			}
			else {
				res = m_pSession->MyVaultRevokeShare(duid);
				if (!res) {
					theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_REVOKE_RIGHTS).GetBuffer()/*L"Failed to revoke file rights."*/).c_str());
				}
				else
				{
					theApp.ShowTrayMsg(theApp.LoadStringW(IDS_RIGHTS_REVOKED_FOR_ALL_USERS)/*L"Rights to this file have been revoked for all users."*/);
					bret = TRUE;
				}
			}
		}

	}
	return bret;
}

void CnxrmRMCApp::ShowRepositoryFileProperty(const HWND hWnd, const RepositoryFileInfo &fileinfo, HICON fileicon)
{
	BOOL bret = FALSE;
	NX::NXL::FileAdHocPolicy policy;
	NX::RmRepository rmrepo;
	std::wstring reponame;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (fileinfo.iRepoIndex >= (int)m_RepositoryList.size())
			return;
		rmrepo = m_RepositoryList[fileinfo.iRepoIndex];
		reponame = m_RepositoryList[fileinfo.iRepoIndex].GetName();
	}

	NX::NXL::FileSecret secret;
	NX::Result res = m_pSession->RepoGetNxlFileHeader(rmrepo, fileinfo.strPath, &g_bExitFlag, &secret, NULL, &policy, NULL);
	if (!res) {
		ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_GET_FILE_INFO).GetBuffer()/*L"Failed to get file information."*/).c_str());
		return;
	}
	CDlgFileInfo dlg(CWnd::FromHandle(hWnd));

	if (NULL != m_pSession->GetCurrentUser().Findmembership(policy.GetOwnerId())) {
		dlg.m_bOwner = TRUE;
	}

	if (fileinfo.bIsNXL)
	{
		if (secret.GetToken().IsFullToken()) {
			dlg.m_fileRights = policy.GetRights();
			const std::vector<std::wstring>& obs = policy.GetObligations();
			std::for_each(obs.begin(), obs.end(), [&](const std::wstring& s) {
				if (!lstrcmpi(s.c_str(), L"WATERMARK")) dlg.m_fileRights.push_back(s);
			});
		}
	}
	dlg.m_bRepositoryFile = TRUE;
	dlg.m_strFileName = fileinfo.strFileName.c_str();
	dlg.m_strFileSize = fileinfo.strFileSize.c_str();
	dlg.m_strFileModify = fileinfo.strFileDate.c_str();
	ASSERT(fileinfo.iRepoIndex != -1);
	dlg.m_strFilePath = reponame.c_str();
	dlg.m_strFilePath += GetParentFolder(fileinfo.strDisplayPath).c_str();

	if (dlg.DoModal() == IDOK) {
	}

}

BOOL CnxrmRMCApp::ReShareMyValutFile(const HWND hwnd, HICON fileicon, const MyVaultFileInfo &mvFile)
{
	NXSharingData dataIn = { true, false, false, false, false, false, false, DLGTYPE_SHARING, };
	NXSharingData dataOut;

	BOOL bret = FALSE;
	dataIn.hFileIcon = fileicon;
	dataIn.dtString = mvFile.rFileInfo.strFileDate;
	dataIn.sizeString = mvFile.rFileInfo.strFileSize;

	NX::RmMyVaultFileMetadata metadata;
	NX::Result res = m_pSession->MyVaultGetFileInfo(mvFile.strDuid, mvFile.rFileInfo.strPath, metadata);
	if (!res) {
		return bret;
	}
	const std::vector<std::wstring>& rights = metadata.GetRights();
	//std::wcout << L"  - Rights: " << std::endl;
	std::for_each(rights.begin(), rights.end(), [&](const std::wstring& s) {
		if (!lstrcmpi(s.c_str(), L"VIEW")) dataIn.bView = true;
		if (!lstrcmpi(s.c_str(), L"EDIT")) dataIn.bEdit = true;
		if (!lstrcmpi(s.c_str(), L"SHARE")) dataIn.bShare = true;
		if (!lstrcmpi(s.c_str(), L"PRINT")) dataIn.bPrint = true;
		if (!lstrcmpi(s.c_str(), L"DOWNLOAD")) dataIn.bDownload = true;
		if (!lstrcmpi(s.c_str(), L"WATERMARK")) dataIn.bWaterMark = true;
	});

	if (mvFile.rFileInfo.bIsNXL)
	{
		const std::vector<std::wstring>& recipients = metadata.GetRecipients();
		std::for_each(recipients.begin(), recipients.end(), [&](const std::wstring& s) {
			dataIn.peopleArr.push_back(s);
		});
		dataIn.bAssignedRights = TRUE;
	}

	wchar_t *pAddresses;
	if (!RmuShowShareDialog(hwnd, mvFile.rFileInfo.strFileName.c_str(), dataIn, &dataOut, &pAddresses))
	{//Here we need to do reshare as files in my vault are protected.
		if (mvFile.rFileInfo.bIsNXL)
		{
			if (mvFile.bIsShared) {
				vector<wstring> addAddr;
				vector<wstring> recipientAdded, recipientRemoved;
				auto size = lstrlen(pAddresses);
				if (size)
				{
					//vector<wstring> peopleArr;
					CString addresses = pAddresses;
					int curPos = 0;
					CString resToken = addresses.Tokenize(_T(";"), curPos);
					while (resToken != _T(""))
					{
						if (!RemoveAddressFromVector(dataIn.peopleArr, resToken.GetBuffer()))
						{
							addAddr.push_back(resToken.GetBuffer());
						}
						resToken = addresses.Tokenize(_T(";"), curPos);
					};
				}

				res = m_pSession->MyVaultUpdateShareRecipients(mvFile.strDuid, addAddr, dataIn.peopleArr, recipientAdded, recipientRemoved);
				if (!res) {
					ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_RESHARE_MYVAULT_FILE).GetBuffer()/*L"Failed to re-share MyVault file."*/).c_str());
				}
				else
				{
					ShowTrayMsg(theApp.LoadStringW(IDS_SUCCEED_CHANGED_RECEPIENTS)/*L"Successfully changed the recipients of the file."*/);
					bret = TRUE;
				}
			}
			else {
				vector<wstring> peopleArr;
				CString addresses = pAddresses;
				int curPos = 0;
				CString resToken = addresses.Tokenize(_T(";"), curPos);
				while (resToken != _T(""))
				{
					peopleArr.push_back(resToken.GetBuffer());
					resToken = addresses.Tokenize(_T(";"), curPos);
				};

				auto ullRights = StringRightsToUll(dataOut);
				NX::RmRepository rmrepo;
				{
					lock_guard<mutex> lock(m_mutRepoData);
					rmrepo = m_RepositoryList[0];
				}

				NX::RmFileShareResult sr;
				res = m_pSession->MyVaultShareFile(m_pSession->GetCurrentUser().GetMemberships().at(0).GetId(),
					rmrepo,
					mvFile.rFileInfo.strPath,
					mvFile.rFileInfo.strFileName,
					mvFile.rFileInfo.strDisplayPath,
					ullRights,
					NX::NXL::FileMeta(),
					NX::NXL::FileTags(),
					peopleArr,
					(__int64)0,
					sr
				);
				if (!res) {
					ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_SHARE_MYVAULT_FILE).GetBuffer()/*L"Failed to share MyVault file."*/).c_str());
				}
				else
				{
					ShowTrayMsg(theApp.LoadStringW(IDS_FILE_HAS_BEEN_SHARED)/*L"The file has been shared."*/);
					bret = TRUE;
				}
			}
		}
	}
	
	return bret;
}

BOOL CnxrmRMCApp::ShareRepositoryFile(const HWND hWnd, const RepositoryFileInfo &repoFile)
{
	BOOL bret = FALSE;
	ASSERT(repoFile.iRepoIndex < (int)m_RepositoryList.size());
	NX::RmRepository rmrepo;
	std::wstring reponame;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (repoFile.iRepoIndex >= (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[repoFile.iRepoIndex];
	}

	NXSharingData dataIn = { true, false, false, false, false, false, false, DLGTYPE_SHARING, };
	NXSharingData dataOut;

	auto fileName = repoFile.strFileName;
 	if (repoFile.bIsNXL)
 	{
		NX::NXL::FileAdHocPolicy policy;
		NX::NXL::FileSecret secret;
		NX::Result res = m_pSession->RepoGetNxlFileHeader(rmrepo, repoFile.strPath, &g_bExitFlag, &secret, NULL, &policy, NULL);
		if (!res) {
			ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_GET_FILE_INFO).GetBuffer()/*L"Fail to get File Information"*/).c_str());
			return FALSE;
		}

		const bool isOwner = (NULL != m_pSession->GetCurrentUser().Findmembership(secret.GetOwnerIdW()));

		if (!isOwner && (BUILTIN_RIGHT_SHARE != (policy.RightsToInt64() & BUILTIN_RIGHT_SHARE) || !secret.GetToken().IsFullToken())) {
			m_pSession->LogActivityShare(false, true,
				NX::bintohs<wchar_t>(secret.GetToken().GetId().data(), secret.GetToken().GetId().size()),
				secret.GetOwnerIdW(),
				rmrepo.GetId(),
				repoFile.strPath,
				repoFile.strFileName,
				repoFile.strDisplayPath);
			ShowTrayMsg(theApp.LoadStringW(IDS_FAILED_TO_SHARE_FILE)/*L"Failed to share the file (access denied)."*/);
			return FALSE;
		}

		const std::vector<std::wstring>& rights = policy.GetRights();
		//std::wcout << L"  - Rights: " << std::endl;
		std::for_each(rights.begin(), rights.end(), [&](const std::wstring& s) {
			if (!lstrcmpi(s.c_str(), L"VIEW")) dataIn.bView = true;
			if (!lstrcmpi(s.c_str(), L"EDIT")) dataIn.bEdit = true;
			if (!lstrcmpi(s.c_str(), L"SHARE")) dataIn.bShare = true;
			if (!lstrcmpi(s.c_str(), L"PRINT")) dataIn.bPrint = true;
			if (!lstrcmpi(s.c_str(), L"DOWNLOAD")) dataIn.bDownload = true;
		});
		const std::vector<std::wstring>& obs = policy.GetObligations();
		std::for_each(obs.begin(), obs.end(), [&](const std::wstring& s) {
			if (!lstrcmpi(s.c_str(), L"WATERMARK")) dataIn.bWaterMark = true;
		});

		dataIn.bAssignedRights = TRUE;
	}

	auto fname = repoFile.strDisplayPath;
	LPWSTR pAddresses;
	if (!RmuShowShareDialog(hWnd, fname.c_str(), dataIn, &dataOut, &pAddresses))
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

			auto ullRights = StringRightsToUll(dataOut);

			NX::RmFileShareResult sr;
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
			NX::RmRepoFile rfile(repoFile.strPath, repoFile.strDisplayPath, repoFile.strFileName, (repoFile.bIsDirectory == TRUE), repoFile.iFileSize, repoFile.lFileDate);
			auto res = m_pSession->RepoShareFile(m_pSession->GetCurrentUser().GetMemberships().at(0).GetId(),
				rmrepo,
				rfile,
				ullRights,
				NX::NXL::FileMeta(),
				NX::NXL::FileTags(),
				peopleArr,
				(__int64)0,
				sr
			);
			if (!res) {
				ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_SHARE_REPO_FILE).GetBuffer()/*L"Failed to share repository file."*/).c_str());
			}
			else {
				DisplayMyVaultInfo(sr, TRUE);
				bret = TRUE;
			}
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
		RmuFreeResource(pAddresses);

	}

	return bret;

}

int CnxrmRMCApp::GetRepositoryIndex(const std::wstring &repoid)
{
	lock_guard<mutex> lock(m_mutRepoData);
	for (size_t s = 0; s < m_RepositoryList.size(); s++) {
		if (m_RepositoryList[s].GetId().compare(repoid) == 0) {
			return (int) s;
		}
	}
	return -1;
}

BOOL CnxrmRMCApp::GetRepositoryFileList(int index, std::wstring path, vector<RepositoryFileInfo> & filelist, std::wstring keyword, BOOL brefresh)
{
	ASSERT(index < (int)m_RepositoryList.size());
	NX::RmRepository rmrepo;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (index >= (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[index];
	}

	if (keyword.length() > 0)//alway go remote when searching
		brefresh = TRUE;
	filelist.clear();
	NX::Result nres = RESULT(2);
	std::vector<NX::RmRepoFile> files;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	if (!brefresh) {
		nres = m_pSession->RepoGetFolderFileListFromCache(rmrepo, path, files);
	}

	if (!nres) {
		if (0 == keyword.length()) {
			nres = m_pSession->RepoGetFolderFileListFromRemote(rmrepo, path, files);
		}
		else {
			path = L"/";
			nres = m_pSession->RepoSearchFiles(rmrepo, path, keyword, &g_bExitFlag, files);
		}
	}
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	if (!nres) {
		if (nres.GetCode() == 5005 && keyword.length() == 0) {//popup add repository dialog
				std::wstring errMsg;
				errMsg = NX::FormatString(theApp.LoadStringW(IDS_AUTHORIZATION_REQUIRED).GetBuffer()
                /*L"Authorization is required for this repository\nPlease login to SkyDRM for one time authorization"*/);
				MessageBox(m_mainhWnd,errMsg.c_str(), theApp.LoadStringW(IDS_AUTHORIZATION)/*L"Authorization"*/, MB_OK | MB_ICONINFORMATION);
				auto pRMSServer = theApp.m_pSession->GetCurrentRMS();
				auto szUrl = pRMSServer.GetUrl();
				ShellExecuteW(NULL, L"open", szUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		return FALSE;
	}
	filelist.clear();
	std::for_each(files.cbegin(), files.cend(), [&](const NX::RmRepoFile& file) {//add folder first
		if (file.IsFolder()) {
			RepositoryFileInfo fileinfo = SetFileInfo(file);
			fileinfo.iRepoIndex = index;
			filelist.push_back(fileinfo);
		}
	});
	std::for_each(files.cbegin(), files.cend(), [&](const NX::RmRepoFile& file) {//add files after folder
		if (!file.IsFolder()) {
			RepositoryFileInfo fileinfo = SetFileInfo(file);
			fileinfo.iRepoIndex = index;
			filelist.push_back(fileinfo);
		}
	});

	return TRUE;
}

std::wstring CnxrmRMCApp::FormatFileSizeString(__int64 size)
{
	std::wstring sret;
	if (size == -1 ) {
		sret = theApp.LoadString(IDS_N_SLASH_A)/*L"N/A"*/;
	}
	else if (size < 1024) {
		sret = NX::i64tos<WCHAR>(size) + theApp.LoadString(IDS_BYTES_SUFFIX).GetBuffer()/*L" Bytes"*/;
	}
	else if (size < 1024 * 1024) {
		sret = NX::i64tos<WCHAR>(size / 1024) + theApp.LoadString(IDS_KB_SUFFIX).GetBuffer()/* L" KB"*/;
	}
	else {
		if (size / (1024 * 1024) < 1000) {
			wchar_t buf[16];
			std::swprintf(buf, sizeof(buf), theApp.LoadString(IDS_FMT_2F_MB).GetBuffer()/*L"%2.2f MB"*/, float(size) / (1024 * 1024));
			sret = buf;
		}
		else {
			sret = NX::i64tos<WCHAR>(size / (1024 * 1024)) + theApp.LoadString(IDS_MB_SUFFIX).GetBuffer()/*L" MB"*/;
		}
	}

	return sret;
}

std::wstring CnxrmRMCApp::FormatFileTimeString(SYSTEMTIME &st)
{

	std::wstring strret;

	WCHAR    dateFormat[32];
	WCHAR    dateString[32];
	if (GetDateFormatEx(LOCALE_NAME_SYSTEM_DEFAULT, DATE_LONGDATE, &st, L"YYYY-MM-DD", dateFormat, sizeof(dateFormat), NULL)) {
		swprintf_s(dateString, 32, dateFormat, st.wYear, st.wMonth, st.wDay);
	}
	else {
		swprintf_s(dateString, 32, L"%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
	}

	strret = dateString;
	strret += L" ";
	WCHAR    timeFormat[32];
	WCHAR    timeString[32];
	if (GetTimeFormatEx(LOCALE_NAME_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, &st, L"HH:mm:ss", timeFormat, sizeof(timeFormat))) {
		swprintf_s(timeString, 32, timeFormat, st.wHour, st.wMinute, st.wMinute);
	}
	else {
		swprintf_s(timeString, 32, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
	}

	strret += timeString;
	return strret;
}

std::wstring CnxrmRMCApp::GetFileExtension(std::wstring filename, BOOL *pIsNXL)
{
	NX::win::FilePath filepath(filename);
	std::wstring ext;
	ext = filepath.GetFileExtension();
	if (pIsNXL) {
		*pIsNXL = (0 == _wcsnicmp(ext.c_str(), L".NXL", 4));
	}
	return ext;
}

RepositoryFileInfo CnxrmRMCApp::SetFileInfo(const NX::RmRepoFile &repoFile)
{
	RepositoryFileInfo fileinfo;

	fileinfo.strFileName = repoFile.GetName();
	fileinfo.bIsDirectory = repoFile.IsFolder();
	NX::time::datetime dt = NX::time::datetime::from_unix_time(repoFile.GetLastModifiedTime());
	SYSTEMTIME st;
	dt.to_systemtime(&st, true);
	fileinfo.strFileDate = FormatFileTimeString(st);
	fileinfo.lFileDate = repoFile.GetLastModifiedTime();
	fileinfo.iFileSize = repoFile.GetSize();
	fileinfo.strFileSize = FormatFileSizeString(repoFile.GetSize());
	fileinfo.strPath = repoFile.GetId();
	fileinfo.strDisplayPath = repoFile.GetPath();
	fileinfo.strFilePropertyArr = repoFile.GetProperties();
	fileinfo.strFileExtension = GetFileExtension(repoFile.GetName(), &fileinfo.bIsNXL);
 	if (fileinfo.bIsNXL) {
 		std::wstring file = repoFile.GetName();
 		file.erase(file.end() - 4, file.end());
 		fileinfo.strFileExtension = GetFileExtension(file);
 	}
	fileinfo.bIsFavorite = FALSE;
	fileinfo.bIsOffline = FALSE;

	return fileinfo;
}

MyVaultFileInfo CnxrmRMCApp::SetFileInfo(const NX::RmMyVaultFile &mvFile)
{
	MyVaultFileInfo fileinfo;
	fileinfo.rFileInfo.strFileName = mvFile.GetName();
	fileinfo.rFileInfo.bIsDirectory = FALSE;
	NX::time::datetime dt = NX::time::datetime::from_unix_time(mvFile.GetSharedTime());
	SYSTEMTIME st;
	dt.to_systemtime(&st, true);
	fileinfo.rFileInfo.strFileDate = FormatFileTimeString(st);
	fileinfo.rFileInfo.lFileDate = mvFile.GetSharedTime();
	fileinfo.rFileInfo.iFileSize = mvFile.GetSize();
	fileinfo.rFileInfo.strFileSize = FormatFileSizeString(mvFile.GetSize());
	fileinfo.rFileInfo.strPath = mvFile.GetPath();
	fileinfo.rFileInfo.strDisplayPath = mvFile.GetDisplayPath();
	fileinfo.rFileInfo.strFileExtension = GetFileExtension(mvFile.GetName(), &fileinfo.rFileInfo.bIsNXL);
 	if (fileinfo.rFileInfo.bIsNXL) {
 		std::wstring file = mvFile.GetName();
 		file.erase(file.end() - 4, file.end());
 		fileinfo.rFileInfo.strFileExtension = GetFileExtension(file);
 	}
	fileinfo.rFileInfo.iRepoIndex = GetRepositoryIndex(mvFile.GetRepoId());
	fileinfo.rFileInfo.bIsFavorite = FALSE;
	fileinfo.rFileInfo.bIsOffline = FALSE;

	fileinfo.bIsDeleted = mvFile.IsDeleted();
	fileinfo.bIsRevoked = mvFile.IsRevoked();
	fileinfo.bIsShared = mvFile.IsShared();
	fileinfo.strDuid = mvFile.GetDuid();
	fileinfo.strSharedWith = mvFile.GetSerializedSharedWith();
	fileinfo.strOriginalPath = mvFile.GetSourceFileInfo().GetRepoName() + std::wstring(L":") + mvFile.GetSourceFileInfo().GetDisplayPath();

	return fileinfo;
}

DWORD CALLBACK DownloadMyVaultFileProc(_In_opt_ PVOID lpData)
{
	NX::Result res = theApp.m_pSession->MyVaultDownloadFile(g_dwninfo.filename, g_dwninfo.filepath, false, &g_bCancel);
	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_TO_DOWNLOAD_MYVAULT).GetBuffer()/* L"Failed to download file from MyVault."*/).c_str());
	}
	else {
		wstringstream txtOut;
		txtOut << theApp.LoadString(IDS_FILE_DOWNLOADED_TO_SUFFIX).GetBuffer()/*L"The file has been downloaded to "*/ << g_dwninfo.filepath;
		theApp.ShowTrayMsg(txtOut.str().c_str());
	}
	g_progressFuncs.closeProgress();
	return 0;
}

DWORD CALLBACK RefreshHomepageProc(_In_opt_ PVOID lpData)
{
	theApp.RefreshRepositoryList();
	theApp.RestAPIRefreshUsage();
	theApp.RestAPIRefreshUserInfo();
	theApp.RefreshProjectList();
    g_progressFuncs.closeProgress();
    theApp.m_homePage.PostMessage(WM_NX_REFRESH_HOME_PAGE, 0, 0);
    return 0;
}


BOOL CnxrmRMCApp::RefreshHomePage()
{
    if (IsProgressDlgRunning())
        return FALSE;
    CloseHandle(g_progThread);
    g_progThread = INVALID_HANDLE_VALUE;

    g_progressFuncs.titleText = theApp.LoadString(IDS_REFRESH).GetBuffer()/*L"Refresh..."*/;
    OnVScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.
    OnHScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.
    if (RmuOpenProgressWindow(theApp.m_mainhWnd, g_progressFuncs, true))
    {
        g_bCancel = FALSE;
        g_progThread = ::CreateThread(NULL, 0, RefreshHomepageProc, 0, 0, NULL);
        return TRUE;
    }
    return FALSE;
}


BOOL CnxrmRMCApp::DownloadMyValutFile(RepositoryFileInfo &fileinfo, std::wstring localpath)
{
	if (IsProgressDlgRunning())
		return FALSE;
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;

	g_dwninfo.filepath = localpath;
	g_dwninfo.filename = fileinfo.strPath;
	g_progressFuncs.titleText = theApp.LoadString(IDS_DOWNLOAD).GetBuffer()/*L"Download..."*/;
	if (RmuOpenProgressWindow(theApp.m_mainhWnd, g_progressFuncs, true))
	{
		g_bCancel = FALSE;
		g_progThread = ::CreateThread(NULL, 0, DownloadMyVaultFileProc, &g_dwninfo, 0, NULL);
	}
	
	return TRUE;
}

BOOL CnxrmRMCApp::DeleteMyValutFile(MyVaultFileInfo & fileinfo)
{
	NX::Result res = m_pSession->MyVaultDeleteFile(fileinfo.strDuid, fileinfo.rFileInfo.strPath);
	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_TO_SYNC_MYVAULT).GetBuffer() /*L"Failed to sync MyVault files."*/).c_str());
	}

	return (res);
}

DWORD CALLBACK DownloadMyVaultFirstPageProc(_In_opt_ PVOID lpData)
{
	std::vector<NX::RmMyVaultFile> files;
	NX::RmFinder * pfinder = (NX::RmFinder *) lpData;
	NX::Result res = theApp.m_pSession->MyVaultGetFirstPageFiles(*pfinder, &g_bCancel, files);
	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_TO_SYNC_MYVAULT).GetBuffer()/*L"Fail to sync MyVault files"*/).c_str());
		return -1;
	}

	if (g_bCancel || files.empty()) {
		return 0;
	}

	vector<MyVaultFileInfo> filelist;
	std::for_each(files.cbegin(), files.cend(), [&](const NX::RmMyVaultFile &file) {
		MyVaultFileInfo fileinfo = theApp.SetFileInfo(file);
		filelist.push_back(fileinfo);
	});

	//set the list first to fresh UI.
	g_dwnvalutinfo.pfilelist->clear();
	for (auto file : filelist) {
		g_dwnvalutinfo.pfilelist->push_back(file);
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	if (g_dwnvalutinfo.func) {
		(*g_dwnvalutinfo.func)(g_dwnvalutinfo.callbackMessage);
	}

	return 0;
}

DWORD CALLBACK DownloadMyVaultNextPageProc(_In_opt_ PVOID lpData)
{
	std::vector<NX::RmMyVaultFile> files;
	NX::RmFinder * pfinder = (NX::RmFinder *) lpData;
	vector<MyVaultFileInfo> filelist;
	if (!pfinder->NoMoreData()) {
		files.clear();
		NX::Result res = theApp.m_pSession->MyVaultGetNextPageFiles(*pfinder, &g_bCancel, files);
		if (!res) {
			theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_TO_SYNC_MYVAULT).GetBuffer()/*L"Failed to sync MyVault files."*/).c_str());
			return -1;
		}
		std::for_each(files.cbegin(), files.cend(), [&](const NX::RmMyVaultFile &file) {
			MyVaultFileInfo fileinfo = theApp.SetFileInfo(file);
			filelist.push_back(fileinfo);
		});
	}
	for (auto file : filelist) {
		g_dwnvalutinfo.pfilelist->push_back(file);
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	if (g_dwnvalutinfo.func) {
		(*g_dwnvalutinfo.func)(g_dwnvalutinfo.callbackMessage);
	}

	return 0;
}

#define MAX_MYVALUT_FILELIST_NUM	1000
DWORD CALLBACK DownloadMyValutFileListProc(_In_opt_ PVOID lpData)
{
	std::vector<NX::RmMyVaultFile> files;
	NX::RmFinder finder(25, std::vector<int>({ OrderByCreationTimeDescend }), L"allFiles", L"fileName", g_dwnvalutinfo.keyword);
	NX::Result res = theApp.m_pSession->MyVaultGetFirstPageFiles(finder, &g_bCancel, files);
	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_TO_SYNC_MYVAULT).GetBuffer()/*L"Failed to sync MyVault files."*/).c_str());
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		return -1;
	}

	if (g_bCancel || files.empty()) {
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		return 0;
	}

	vector<MyVaultFileInfo> filelist;
	std::for_each(files.cbegin(), files.cend(), [&](const NX::RmMyVaultFile &file) {
		MyVaultFileInfo fileinfo = theApp.SetFileInfo(file);
		filelist.push_back(fileinfo);
	});

	//set the list first to fresh UI.
	g_dwnvalutinfo.pfilelist->clear();
	for (auto file : filelist) {
		g_dwnvalutinfo.pfilelist->push_back(file);
	}

	if (g_dwnvalutinfo.func) {
		(*g_dwnvalutinfo.func)(g_dwnvalutinfo.callbackMessage);
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	filelist.clear();//reset and retrieve left files
	while (!finder.NoMoreData() && !g_bCancel) {
		files.clear();
		res = theApp.m_pSession->MyVaultGetNextPageFiles(finder, &g_bCancel, files);
		if (!res) {
			break;
		}
		std::for_each(files.cbegin(), files.cend(), [&](const NX::RmMyVaultFile &file) {
			MyVaultFileInfo fileinfo = theApp.SetFileInfo(file);
			filelist.push_back(fileinfo);
		});
		if (filelist.size() >= MAX_MYVALUT_FILELIST_NUM)
			break;
	}
	if (!g_bCancel) {
		for (auto file : filelist) {
			g_dwnvalutinfo.pfilelist->push_back(file);
		}

		if (g_dwnvalutinfo.func) {
			(*g_dwnvalutinfo.func)(g_dwnvalutinfo.callbackMessage);
		}
		theApp.m_dlgRepoPage.SetMyVaultRefreshFlag(FALSE);//refresh is done.
	}

	return 0;
}

BOOL CnxrmRMCApp::GetMyValutFirstPage(NX::RmFinder * pfinder, vector<MyVaultFileInfo> *pfilelist, StatusCallBackFunc func, MSG * message)
{
	while (IsProgressDlgRunning()) {
		g_bCancel = TRUE;//cancel last one.
		Sleep(100);
	}
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;
	if (NULL != message) {
		g_dwnvalutinfo.callbackMessage = *message;
	}
	g_dwnvalutinfo.pfilelist = pfilelist;
	g_dwnvalutinfo.func = func;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	g_dwnvalutinfo.pfilelist = pfilelist;
	g_bCancel = FALSE;
	g_progThread = ::CreateThread(NULL, 0, DownloadMyVaultFirstPageProc, pfinder, 0, NULL);

	return TRUE;
}

BOOL CnxrmRMCApp::GetMyVaultMoreData(NX::RmFinder *pfinder, vector<MyVaultFileInfo> *pfilelist, StatusCallBackFunc func, MSG * message)
{
	while (IsProgressDlgRunning()) {
		g_bCancel = TRUE;//cancel last one.
		Sleep(100);
	}
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;
	if (NULL != message) {
		g_dwnvalutinfo.callbackMessage = *message;
	}
	g_dwnvalutinfo.pfilelist = pfilelist;
	g_dwnvalutinfo.func = func;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	g_dwnvalutinfo.pfilelist = pfilelist;
	g_bCancel = FALSE;
	g_progThread = ::CreateThread(NULL, 0, DownloadMyVaultNextPageProc, pfinder, 0, NULL);

	return TRUE;
}

BOOL CnxrmRMCApp::GetMyValutFileList(std::wstring keyword, vector<MyVaultFileInfo> *pfilelist, StatusCallBackFunc func, MSG * message)
{
	while (IsProgressDlgRunning()) {
		g_bCancel = TRUE;//cancel last one.
		Sleep(100);
	}
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;

	if (NULL != message) {
		g_dwnvalutinfo.callbackMessage = *message;
	}
	g_dwnvalutinfo.pfilelist = pfilelist;
	g_dwnvalutinfo.func = func;
	g_dwnvalutinfo.keyword = keyword;


	g_bCancel = FALSE;
	g_progThread = ::CreateThread(NULL, 0, DownloadMyValutFileListProc, &g_dwnvalutinfo, 0, NULL);

	return TRUE;
}

BOOL CnxrmRMCApp::DeleteFavoriteFile(RepositoryFileInfo &fileinfo)
{
	NX::RmRepository rmrepo;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		ASSERT(fileinfo.iRepoIndex < (int)m_RepositoryList.size() && fileinfo.iRepoIndex >= 0);
		if (fileinfo.iRepoIndex >= (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[fileinfo.iRepoIndex];
	}


	NX::RmRepoFile file(fileinfo.strPath, fileinfo.strDisplayPath, fileinfo.strFileName, (fileinfo.bIsDirectory == TRUE), fileinfo.iFileSize, fileinfo.lFileDate);
	NX::Result res = theApp.m_pSession->RepoUnsetFavoriteFile(rmrepo, file);
	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_DELETE_FAVORITE_FILE).GetBuffer()/*L"Fail to delete Favorite file"*/).c_str());
	}

	return (res);
}

BOOL CnxrmRMCApp::GetFavoriteFileList(std::vector<std::pair<int, std::vector<RepositoryFileInfo>>>& files, BOOL brefresh)
{
	std::vector<std::pair<std::wstring, std::vector<NX::RmRepoFile>>> favfiles;
	NX::Result res = RESULT(2);
	if (!brefresh) {
		res = theApp.m_pSession->RepoGetFavoriteFilesFromCache(favfiles);
	}

	if (!res) {
		res = theApp.m_pSession->RepoGetFavoriteFilesFromRemote(favfiles);
	}

	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_GET_FAVORITE_FILE).GetBuffer()/*L"Fail to get favorite files"*/).c_str());
		return FALSE;
	}
	files.clear();
	std::for_each(favfiles.cbegin(), favfiles.cend(), [&](const std::pair<std::wstring, std::vector<NX::RmRepoFile>>& file) {
		int index = GetRepositoryIndex(file.first);
		std::vector<NX::RmRepoFile> repofiles = file.second;
		std::vector<RepositoryFileInfo> filelist;
		std::for_each(repofiles.cbegin(), repofiles.cend(), [&](const NX::RmRepoFile repofile) {
			RepositoryFileInfo fileinfo = SetFileInfo(repofile);
			fileinfo.iRepoIndex = index;
			fileinfo.bIsFavorite = TRUE;
			filelist.push_back(fileinfo);
		});
		files.push_back(make_pair(index, filelist));
	});

	return TRUE;
}

BOOL CnxrmRMCApp::UpdateRepository(std::vector<NX::RmRepository> &repolist)
{
	lock_guard<mutex> lock(m_mutRepoData);
	BOOL bFound = FALSE;

	if (repolist.size() == m_RepositoryList.size())//compare if the list is changed.
	{
		for(auto x: repolist) {
			size_t i;
			for (i = 0; i < repolist.size(); i++) {
				if (0 == x.GetAccountId().compare(m_RepositoryList[i].GetAccountId()) && 0 == x.GetName().compare(m_RepositoryList[i].GetName()))
					break;
			}
			if (i == repolist.size())
				bFound = TRUE;
		}
		if (!bFound)
			return FALSE;
	}

	bFound = FALSE;
	m_RepositoryList.clear();
	for (size_t i = 0; i < repolist.size(); i++) {//add mydrive first
		if (repolist[i].GetType() == NX::RmRepository::MYDRIVE)
		{
			m_RepositoryList.push_back(repolist[i]);
			bFound = TRUE;
			break;
		}
	};

	if (!bFound) {//something wrong, discuss this list.
		ASSERT(TRUE);
		return FALSE;
	}

	//Add other repositories
	for_each(repolist.cbegin(), repolist.cend(), [&](const NX::RmRepository repo) {
		if (repo.GetType() != NX::RmRepository::MYDRIVE) {
			m_RepositoryList.push_back(repo);
		}
	});

	return TRUE;
}

BOOL CnxrmRMCApp::RefreshRepositoryList(BOOL bRefresh)
{
	std::vector<NX::RmRepository> repolist;
	NX::Result res = RESULT(2);
	if (!bRefresh) {
		res = m_pSession->RepoGetReposListFromCache(repolist);
	}

	if (!res) {
		res = m_pSession->RepoGetReposListFromRemote(repolist);
	}
	if (!res)
		return FALSE;

	if (!UpdateRepository(repolist))
		return TRUE;

	if (IsWindow(m_dlgRepoPage.m_hWnd)) {
	::PostMessage(m_dlgRepoPage.m_hWnd, WM_NX_REFRESH_REPO_LIST, 0, 0);
	}
	if (IsWindow(m_homePage.m_hWnd)) {
		::PostMessage(m_homePage.m_hWnd, WM_NX_REFRESH_REPO_LIST, 0, 0);
	}
	return TRUE;
}

BOOL CnxrmRMCApp::GetRepositoryInfo(int index, RepositoryInfo & repo)
{
	lock_guard<mutex> lock(m_mutRepoData);
	if (index >= (int)m_RepositoryList.size())
		return FALSE;
	repo.Id = m_RepositoryList[index].GetId();
	repo.AccountId = m_RepositoryList[index].GetAccountId();
	repo.AccountName = m_RepositoryList[index].GetAccountName();
	repo.Type = m_RepositoryList[index].GetType();
	repo.TypeName = m_RepositoryList[index].GetTypeName();
	repo.Name = m_RepositoryList[index].GetName();

	return TRUE;
}

BOOL CnxrmRMCApp::RenameRepository(std::wstring orgName, std::wstring newName)
{
	BOOL bret = FALSE;

	NX::RmRepository rmrepo;
	BOOL bfound = FALSE;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		for (size_t i = 0; i < m_RepositoryList.size(); i++) {
			if (m_RepositoryList[i].GetName().compare(orgName) == 0) {
				bfound = TRUE;
				rmrepo = m_RepositoryList[i];
			}
		}
	}

	if (!bfound)
		return bret;
	auto res = m_pSession->RepoRename(rmrepo, newName);
	wstringstream txtOut;
	if (!res) {
		txtOut << "Failed to rename the repository:" << res.GetCode()
			<< ", Repository:" << orgName
			<< std::endl;
		TRACE(L"%s", txtOut.str().c_str());
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_TO_RENAME_REPO).GetBuffer()/*L"Failed to rename the repository."*/).c_str());
	}
	else {
		CString txtOut;

        txtOut.Format(IDS_FMT_S_RENAME_TO_S, orgName.c_str(), newName.c_str());

		//txtOut.Format << L"The file "<< orgName <<L" has been renamed to " << newName << endl;
		theApp.ShowTrayMsg(txtOut);
		bret = TRUE;
	}

	return bret;
}

BOOL CnxrmRMCApp::RemoveRepository(std::wstring orgName)
{
	BOOL bret = FALSE;
	NX::RmRepository rmrepo;
	BOOL bfound = FALSE;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		for (size_t i = 0; i < m_RepositoryList.size(); i++) {
			if (m_RepositoryList[i].GetName().compare(orgName) == 0) {
				bfound = TRUE;
				rmrepo = m_RepositoryList[i];
			}
		}
	}

	if (!bfound)
		return bret;
	auto res = m_pSession->RepoRemove(rmrepo);
	wstringstream txtOut;
	if (!res) {
		wstringstream txtOut;
		txtOut << "Failed to remove the repository:" << res.GetCode()
			<< ", Repository:" << orgName
			<< std::endl;
		TRACE(L"%s", txtOut.str().c_str());
	}
	else {
		CString txtOut;

        txtOut.Format(IDS_FMT_S_REPO_REMOVED, orgName.c_str());
            //<< "The repository ("<< orgName << ") has been removed." << endl;
		bret = TRUE;
        theApp.ShowTrayMsg(txtOut);
    }

	return bret;
}


size_t CnxrmRMCApp::GetRepositoryArray(vector<RepositoryInfo> & repoArr, BOOL brefresh)
{
	if (brefresh) {
		RefreshRepositoryList();
	}
	repoArr.clear();

	for (size_t i = 0; i < m_RepositoryList.size(); i++) {
		RepositoryInfo repo;
		GetRepositoryInfo((int)i, repo);
        repoArr.push_back(repo);
	}

	return m_RepositoryList.size();
}


BOOL CnxrmRMCApp::CreateRepositoryFolder(int index, std::wstring path, std::wstring newfolder)
{
	NX::RmRepository rmrepo;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (index >= (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[index];
	}
	NX::RmRepoFile file;
	auto res = m_pSession->RepoCreateFolder(rmrepo, path, newfolder, file);
	if (!res) {
		if (res.GetCode() == 4001) {
			theApp.ShowTrayMsg(theApp.LoadString(IDS_FAILED_CREATE_FOLDER_ERRNAME)/*L"Failed to create the folder (invalid folder name)."*/);
		}
		else {
			theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_CREATE_FOLDER).GetBuffer()/* L"Failed to create folder."*/).c_str());
		}
	}
	else {
		CString txtOut;
        txtOut.Format(IDS_FMT_S_FOLDER_CREATED, newfolder.c_str());
            /*L"The folder " << newfolder << L" has been created.";*/
		theApp.ShowTrayMsg(txtOut);
	}
	return (res);
}

BOOL CnxrmRMCApp::DeleteRepositoryFolder(int index, std::wstring path, std::wstring foldername)
{
	NX::RmRepository rmrepo;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (index >= (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[index];
	}
	NX::RmRepoFile file;
	auto res = m_pSession->RepoDeleteFolder(rmrepo, path);
	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_DELETE_FOLDER).GetBuffer()/*L"Failed to delete the folder."*/).c_str());
	}
	return (res);
}

BOOL CnxrmRMCApp::DeleteRepositoryFile(int index, std::wstring path, std::wstring filename)
{
	NX::RmRepository rmrepo;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (index >= (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[index];
	}
	auto res = m_pSession->RepoDeleteFile(rmrepo, path);

	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_ERROR_WHILE_DELETE).GetBuffer()/*L"Error occurred while deleting the file."*/).c_str());
	}
	return (res);
}

DWORD CALLBACK DownloadRepFileProc(_In_opt_ PVOID lpData)
{
	NX::RmRepository rmrepo = theApp.m_RepositoryList[g_dwninfo.dwindex];
	auto res = theApp.m_pSession->RepoDownloadFile(rmrepo, g_dwninfo.filename, g_dwninfo.filepath, &g_bCancel);
	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_DOWNLOAD_FROM_REPO).GetBuffer()/*L"Failed to download the file from the repository."*/).c_str());
		DeleteFile(g_dwninfo.filepath.c_str());
	}
	else {
		wstringstream txtOut;
		txtOut << theApp.LoadString(IDS_FILE_DOWNLOADED_TO_PREFIX).GetBuffer()/*L"The file has been downloaded to: "*/ << g_dwninfo.filepath;
		theApp.ShowTrayMsg(txtOut.str().c_str());
	}
	g_progressFuncs.closeProgress();
	return 0;
}

BOOL CnxrmRMCApp::GetRepoNXLFileUid(int index, std::wstring path, std::wstring &uid)
{
	uid.clear();
	if (index >= (int)m_RepositoryList.size())
		return FALSE;

	if (IsProgressDlgRunning())
		return FALSE;
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;

	NX::NXL::FileAdHocPolicy policy;
	NX::NXL::FileSecret secret;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	NX::Result res = m_pSession->RepoGetNxlFileHeader(m_RepositoryList[index], path, &g_bExitFlag, &secret, NULL, &policy, NULL);
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	if (!res) {
		ShowTrayMsg(OutputError(res, L"").c_str());
		return FALSE;
	}
	
	uid = NX::bintohs<wchar_t>(secret.GetToken().GetId().data(), secret.GetToken().GetId().size());
	return TRUE;
}

BOOL CnxrmRMCApp::DownloadRepositoryFile(int index, std::wstring path, std::wstring localpath, BOOL bnxl)
{
	if (index >= (int)m_RepositoryList.size())
		return FALSE;

	if (IsProgressDlgRunning())
		return FALSE;
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;

	if (bnxl) {
		NX::NXL::FileAdHocPolicy policy;
		NX::NXL::FileSecret secret;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		NX::Result res = m_pSession->RepoGetNxlFileHeader(m_RepositoryList[index], path, &g_bExitFlag, &secret, NULL, &policy, NULL);
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		if (!res) {
			ShowTrayMsg(OutputError(res, L"").c_str());
			return FALSE;
		}
		if (NULL == m_pSession->GetCurrentUser().Findmembership(policy.GetOwnerId())) {
			//check rights if not owner.
			const unsigned __int64 rights = policy.RightsToInt64();
			if (BUILTIN_RIGHT_DOWNLOAD != (BUILTIN_RIGHT_DOWNLOAD & rights) || !secret.GetToken().IsFullToken()) {
				// No download rights
				ShowTrayMsg(theApp.LoadString(IDS_ACCESS_DENIED_DLD_NOT_ALLOWED)/*L"Access denied: Download is not allowed."*/);
				NX::RmRepoFile file;
				res = m_pSession->RepoGetFileInfo(m_RepositoryList[index], path, file);
				if (res) {
					m_pSession->LogActivityDownload(false,
													NX::bintohs<wchar_t>(secret.GetToken().GetId().data(), secret.GetToken().GetId().size()),
													secret.GetOwnerIdW(),
													m_RepositoryList[index].GetId(),
													path,
													file.GetName(),
													file.GetPath());
				}
				return FALSE;
			}
		}
	}

	g_dwninfo.dwindex = index;
	g_dwninfo.filename = path;
	g_dwninfo.filepath = localpath;
	g_progressFuncs.titleText = theApp.LoadString(IDS_DOWNLOAD).GetBuffer()/*L"Download..."*/;
	if (RmuOpenProgressWindow(theApp.m_mainhWnd, g_progressFuncs, true))
	{
		g_bCancel = FALSE;
		g_progThread = ::CreateThread(NULL, 0, DownloadRepFileProc, &g_dwninfo, 0, NULL);
	}

	return TRUE;
}

BOOL CnxrmRMCApp::GetInvitationInfo(int index, CProjData &data)
{
	lock_guard<mutex> lock(m_mutInvitation);
	if (index >= (int)m_InvatationList.size() || index < 0)
		return FALSE;
	data.m_projName = m_InvatationList[index].GetProjectName().c_str();
	data.m_imageID = 0;
	data.m_flag = NXRMC_FROM_BY_OTHERS_PENDING;

	data.m_invitorName = m_InvatationList[index].GetInviterDisplayName().c_str();
	data.m_nID = m_InvatationList[index].GetId();
	data.m_nCode = m_InvatationList[index].GetCode();
	data.m_creationTime = m_InvatationList[index].GetInviteTime();

	return TRUE;
}

size_t CnxrmRMCApp::GetInvitationArray(std::vector<CProjData> & inviteArr)
{
	lock_guard<mutex> lock(m_mutInvitation);
	inviteArr.clear();
	for (auto invitation : m_InvatationList) {
		CProjData proj;

		proj.m_projName = invitation.GetProjectName().c_str();
		proj.m_imageID = 0;
		proj.m_flag = NXRMC_FROM_BY_OTHERS_PENDING;

		proj.m_invitorName = invitation.GetInviterDisplayName().c_str();
		proj.m_nID = invitation.GetId();
		proj.m_nCode = invitation.GetCode();
		proj.m_creationTime = invitation.GetInviteTime();
		inviteArr.push_back(proj);
	}

	return inviteArr.size();
}

BOOL CnxrmRMCApp::GetProjectByMeInfo(int index, CProjData &data)
{
	lock_guard<mutex> lock(m_mutProject);

	if (index > (int)m_ProjectByMeCount || index < 0)
		return FALSE;

	for (size_t i = 0; i < m_AllProjects.size(); i++) {
		if (m_AllProjects[i].OwnedByMe()) {
			if (index == 0) {
                data = AssignProjData(m_AllProjects[i]);
				break;
			}
			else
				index--;
		}
	}

	
	return TRUE;
}

BOOL CnxrmRMCApp::GetProjectByOthersInfo(int index, CProjData &data)
{
	lock_guard<mutex> lock(m_mutProject);

	if (index > (int)m_ProjectByOthersCount || index < 0)
		return FALSE;

	for (size_t i = 0; i < m_AllProjects.size(); i++) {
		if (!m_AllProjects[i].OwnedByMe()) {
			if (index == 0) {
				data.m_projName = m_AllProjects[i].GetName().c_str();
				data.m_imageID = 0;
				data.m_fileCount = m_AllProjects[i].GetFileCount();
                data.m_ownerID = m_AllProjects[i].GetOwner().GetUserId();
                data.m_flag = NXRMC_PROJ_BY_OTHERS;
				data.m_nID = m_AllProjects[i].GetId();
				data.m_creationTime = m_AllProjects[i].GetCreationTime();
				data.m_desc = m_AllProjects[i].GetDescription().c_str();

				auto projOwner = m_AllProjects[i].GetOwner();
				data.m_ownerName = projOwner.GetName().c_str();
				data.m_invitorName = L"";
				data.m_initials.clear();
				data.m_imembercount = m_AllProjects[i].GetMemberCount();
				data.m_Simmembers = m_AllProjects[i].GetMembers();

				for (auto member : data.m_Simmembers)
				{
					data.m_initials.push_back(member.GetDisplayName().c_str());
				}
				break;
			}
			else
				index--;
		}
	}

	return TRUE;
}

CProjData CnxrmRMCApp::AssignProjData(NX::RmProject &project)
{
    CProjData proj;
    proj.m_projName = project.GetName().c_str();
    //txtOut << proj.m_projName.GetBuffer();
    proj.m_imageID = 0;
    proj.m_flag = NXRMC_PROJ_BY_ME;
    proj.m_fileCount = project.GetFileCount();
    proj.m_ownerID = project.GetOwner().GetUserId();
    proj.m_desc = project.GetDescription().c_str();
    proj.m_nID = project.GetId();
    proj.m_creationTime = project.GetCreationTime();
    proj.m_imembercount = project.GetMemberCount();
    proj.m_Simmembers = project.GetMembers();
    auto size = proj.m_Simmembers.size();
    for (auto member : proj.m_Simmembers)
    {
        proj.m_initials.push_back(member.GetDisplayName().c_str());
    }
    return proj;
}

size_t CnxrmRMCApp::GetProjectByMeArray(std::vector<CProjData> &projArr)
{
	lock_guard<mutex> lock(m_mutProject);
	projArr.clear();

	for(auto project : m_AllProjects) {
		if (project.OwnedByMe()) {
			CProjData proj = AssignProjData(project);
			projArr.push_back(proj);
		}
	};

	ASSERT(projArr.size() == m_ProjectByMeCount);

	return m_ProjectByMeCount;
}

size_t CnxrmRMCApp::GetProjectByOthersArray(std::vector<CProjData> &projArr)
{
	lock_guard<mutex> lock(m_mutProject);
	projArr.clear();

	for (auto project : m_AllProjects) {
		if (!project.OwnedByMe()) {
			CProjData proj;
			proj.m_projName = project.GetName().c_str();
			proj.m_imageID = 0;
			proj.m_fileCount = project.GetFileCount();
			proj.m_flag = NXRMC_PROJ_BY_OTHERS;
			proj.m_nID = project.GetId();
			proj.m_creationTime = project.GetCreationTime();

			auto projOwner = project.GetOwner();
			proj.m_ownerName = projOwner.GetName().c_str();
			proj.m_invitorName = L"";
            proj.m_ownerID = project.GetOwner().GetUserId();
            proj.m_desc = project.GetDescription().c_str();
            proj.m_initials.clear();
			proj.m_imembercount = project.GetMemberCount();
			proj.m_Simmembers = project.GetMembers();

			for (auto member : proj.m_Simmembers)
			{
				proj.m_initials.push_back(member.GetDisplayName().c_str());
			}

			projArr.push_back(proj);
		}
	}

	ASSERT(projArr.size() == m_ProjectByOthersCount);

	return m_ProjectByOthersCount;
}

DWORD CALLBACK UploadFileProc(_In_opt_ PVOID lpData)
{
	BOOL bret = FALSE;
	NX::RmRepoFile repoFile;
	NX::RmRepository rmrepo;

	rmrepo = theApp.m_RepositoryList[g_dwninfo.dwindex];
	auto res = theApp.m_pSession->RepoUploadFile(rmrepo, g_dwninfo.filepath, g_dwninfo.filename, repoFile, &g_bCancel);
	if (res) {
		CString txtOut;
        txtOut.Format(IDS_FMT_S_FILE_UPLOADED, g_dwninfo.filepath.c_str());
            /*L"The file " << g_dwninfo.filepath << " has been uploaded.";*/
		theApp.ShowTrayMsg(txtOut);
		bret = TRUE;
	}
	else {
		wstringstream txtOut;
		txtOut << theApp.LoadString(IDS_FAILED_UPLOADE_FILE_PREFIX).GetBuffer()/*L"Failed to upload the file: "*/ << g_dwninfo.filepath << std::endl;
		if (res.GetCode() == 4002) {
			txtOut << theApp.LoadString(IDS_FILE_ALREADY_EXIST_PAREN).GetBuffer()/*L"(The file already exists.)"*/;
			theApp.ShowTrayMsg(txtOut.str().c_str());
		}
		else {
			theApp.ShowTrayMsg(OutputError(res, txtOut.str().c_str()).c_str());
		}
	}
	
	if (g_dwninfo.bflag && bret) {
		NX::RmFileShareResult sr;
		auto res = theApp.m_pSession->RepoProtectFile(theApp.m_pSession->GetCurrentUser().GetMemberships().at(0).GetId(),
			rmrepo,
			repoFile,
			g_dwninfo.ulright,
			NX::NXL::FileMeta(),
			NX::NXL::FileTags(),
			sr,
			&g_bCancel
		);
		if (!res) {
			theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_PROTECT_REPO_FILE).GetBuffer()/*L"Fail to protect repository file"*/).c_str());
		}
		else {
			DisplayMyVaultInfo(sr, FALSE);
		}
	}
	if (g_dwninfo.func && bret) {
		(*g_dwninfo.func)(g_dwninfo.callbackMessage);
	}
	g_progressFuncs.closeProgress();

	return 0;
}

DWORD CALLBACK UploadProjectFileProc(_In_opt_ PVOID lpData)
{
	BOOL bret = FALSE;
    NX::NXL::FileTags tags;
    NX::RmProjectFile newFile;

    NX::Result res = theApp.m_pSession->ProjectUploadFile(g_dwninfo.key, g_dwninfo.destpath, g_dwninfo.filename, g_dwninfo.filepath, g_dwninfo.rightstr, tags, newFile, &g_bCancel);
    if (res) {
        CString txtOut;
        txtOut.Format(IDS_FMT_S_FILE_UPLOADED, g_dwninfo.filepath.c_str());
        //txtOut << L"The file " << g_dwninfo.filepath << " has been uploaded.";
        theApp.ShowTrayMsg(txtOut);
        bret = TRUE;
    }
    else {
        wstringstream txtOut;
        txtOut << theApp.LoadString(IDS_FAILED_UPLOADE_FILE_PREFIX).GetBuffer()/* L"Failed to upload the file: "*/ << g_dwninfo.filepath << std::endl;
        theApp.ShowTrayMsg(OutputError(res, txtOut.str().c_str()).c_str());
    }

    if (g_dwninfo.func && bret) {
        (*g_dwninfo.func)(g_dwninfo.callbackMessage);
    }
    g_progressFuncs.closeProgress();

	return 0;
}
bool CnxrmRMCApp::GetProjectInfo(const std::wstring& projectId, CProjData & proj)
{
    NX::RmProject project;
    NX::Result res = m_pSession->ProjectGetInfo(projectId, project);
    if (!res) {
        ShowTrayMsg(OutputError(res, L"").c_str());
        return false;
    }
    proj.m_fileCount = project.GetFileCount();
    proj.m_ownerID = project.GetOwner().GetUserId();
    proj.m_desc = project.GetDescription().c_str();
    proj.m_imembercount = project.GetMemberCount();
    proj.m_Simmembers = project.GetMembers();
    auto size = proj.m_Simmembers.size();
    proj.m_initials.clear();
    for (auto member : proj.m_Simmembers)
    {
        proj.m_initials.push_back(member.GetDisplayName().c_str());
    }

    return true;
}

bool CnxrmRMCApp::SetProjectInfoInData(CProjData & proj)
{
	return GetProjectInfo(proj.m_nID, proj);
}



BOOL CnxrmRMCApp::UploadFileToRepository(HWND hparentwnd, int index, std::wstring path, std::wstring Name, BOOL bProtectAfter, StatusCallBackFunc func, MSG * message)
{
	if (index >= (int)m_RepositoryList.size())
		return FALSE;

	if (IsProgressDlgRunning())
		return FALSE;
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;

	if (NULL != message) {
		g_dwninfo.callbackMessage = *message;
	}
	g_dwninfo.filepath = path;
	g_dwninfo.filename = Name;
	g_dwninfo.dwindex = index;
	g_dwninfo.func = func;

	ULONGLONG ullRights;
	if (bProtectAfter) {
		NXSharingData dataIn = { true, false, false, false, false, false, false, DLGTYPE_PROTECT,0 };
		NXSharingData dataOut;

		LPWSTR pAddresses;
		if (!RmuShowShareDialog(theApp.m_mainhWnd, Name.c_str(), dataIn, &dataOut, &pAddresses))
		{
			ullRights = StringRightsToUll(dataOut);
			g_dwninfo.ulright = ullRights;
		}
		else {
			bProtectAfter = FALSE;
		}
	}
	g_dwninfo.bflag = bProtectAfter;

	if (bProtectAfter)
		g_progressFuncs.titleText = theApp.LoadString(IDS_UPLOAD_PROTECT).GetBuffer()/*L"Upload & Protect..."*/;
	else
		g_progressFuncs.titleText = theApp.LoadString(IDS_UPLOAD).GetBuffer()/*L"Upload..."*/;
	if (RmuOpenProgressWindow(theApp.m_mainhWnd, g_progressFuncs, true))
	{
		g_bCancel = FALSE;
		g_progThread = ::CreateThread(NULL, 0, UploadFileProc, &g_dwninfo, 0, NULL);
	}

	return bProtectAfter;
}

BOOL CnxrmRMCApp::ShowEditUserProfile(const HWND hwnd /* = NULL */)
{
	HWND hparent = hwnd;
	if (NULL == hparent)
		hparent = m_mainhWnd;
	CEditProfileDlg dlg(CWnd::FromHandle(hparent));
	if (dlg.DoModal() == IDOK)
	{
		RestAPIRefreshUserInfo();
		m_homePage.UpdateUserInfo();
		return TRUE;
	}

	return FALSE;
}

BOOL CnxrmRMCApp::UploadFileToProject(HWND hparentwnd, const wstring &projID, std::wstring destDir, std::wstring path, std::wstring name, StatusCallBackFunc func, MSG * message)
{
	if (IsProgressDlgRunning())
		return FALSE;
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;

    if (NULL != message) {
        g_dwninfo.callbackMessage = *message;
    }
	g_dwninfo.func = func;
	g_dwninfo.key = projID;
	g_dwninfo.destpath = destDir;
	g_dwninfo.filepath = path;
	g_dwninfo.filename = name;
    NXSharingData dataIn = { true, false, false, false, false, false, false, DLGTYPE_PROTECT_PROJ,0 };
    NXSharingData dataOut;

    LPWSTR pAddresses;
    if (!RmuShowShareDialog(theApp.m_mainhWnd, name.c_str(), dataIn, &dataOut, &pAddresses))
    {
        g_dwninfo.rightstr = theApp.StringRightsToVector(dataOut);
		
        g_progressFuncs.titleText = theApp.LoadString(IDS_UPLOAD).GetBuffer()/*L"Upload..."*/;
        if (RmuOpenProgressWindow(theApp.m_mainhWnd, g_progressFuncs, true))
        {
			g_bCancel = FALSE;
			g_progThread = ::CreateThread(NULL, 0, UploadProjectFileProc, &g_dwninfo, 0, NULL);
        }
    }
    return TRUE;
}


BOOL CnxrmRMCApp::SetFileToFavorite(int index, std::wstring parentpath, RepositoryFileInfo &file)
{
	NX::RmRepository rmrepo;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (index >= (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[index];
	}

	NX::RmRepoFile repofile(file.strPath, file.strDisplayPath, file.strFileName, (file.bIsDirectory == TRUE), file.iFileSize, file.lFileDate);
	// TODO: A folderid should be added
	NX::Result res = m_pSession->RepoSetFavoriteFile(rmrepo, parentpath, repofile);
	wstringstream txtOut;
	if (!res) {
		TRACE(OutputError(res, theApp.LoadString(IDS_FAILED_TO_SET_FAVORITE).GetBuffer()/*L"Fail to set Favorite files"*/).c_str());
	}
	else
	{
		txtOut << file.strDisplayPath
			<< theApp.LoadString(IDS_SET_FAVORITE_SUFFIX).GetBuffer()/*L", has been set to Favorite"*/;
	}
	theApp.ShowTrayMsg(txtOut.str().c_str());
	return (res);
}

std::wstring CnxrmRMCApp::GetOfflineFilePath(RepositoryFileInfo fileinfo)
{
	ASSERT(fileinfo.iRepoIndex < (int)m_RepositoryList.size() && fileinfo.iRepoIndex >= 0);
	std::wstring id;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (fileinfo.iRepoIndex >= (int)m_RepositoryList.size())
			return FALSE;
		id = m_RepositoryList[fileinfo.iRepoIndex].GetId();
	}

	std::wstring retpath;
	NX::Result res = m_pSession->RepoFindOfflineFile(id, fileinfo.strPath, retpath, NULL);

	if (!res)
		return std::wstring(L"");

	return retpath;

}

std::wstring CnxrmRMCApp::GenerateOfflineFilePath(RepositoryFileInfo fileinfo)
{
	ASSERT(fileinfo.iRepoIndex < (int)m_RepositoryList.size() && fileinfo.iRepoIndex >= 0);
	std::wstring id;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (fileinfo.iRepoIndex >= (int)m_RepositoryList.size())
			return FALSE;
		id = m_RepositoryList[fileinfo.iRepoIndex].GetId();
	}

	std::wstring retpath;
	NX::Result res = m_pSession->RepoCreateOfflineFilePath(id, fileinfo.strPath, fileinfo.lFileDate, retpath);

	if (!res)
		return std::wstring(L"");

	return retpath;
}

BOOL CnxrmRMCApp::GetOfflineFileList(std::vector<std::pair<int, std::vector<RepositoryFileInfo>>>& files)
{
	std::vector<std::pair<std::wstring, std::vector<NX::RmRepoFile>>> offfiles;
	NX::Result res = m_pSession->RepoGetOfflineFiles(offfiles);
	if (!res) {
		TRACE(OutputError(res, theApp.LoadString(IDS_FAILED_LIST_OFFLINE_FILES).GetBuffer()/*L"Fail to list Offline files"*/).c_str());
		return FALSE;
	}
	files.clear();
	std::for_each(offfiles.cbegin(), offfiles.cend(), [&](const std::pair<std::wstring, std::vector<NX::RmRepoFile>>& file) {
		int index = GetRepositoryIndex(file.first);
		std::vector<NX::RmRepoFile> repofiles = file.second;
		std::vector<RepositoryFileInfo> filelist;
		std::for_each(repofiles.cbegin(), repofiles.cend(), [&](const NX::RmRepoFile repofile) {
			RepositoryFileInfo fileinfo = SetFileInfo(repofile);
			fileinfo.iRepoIndex = index;
			fileinfo.bIsOffline = TRUE;
			if(index >= 0)
				filelist.push_back(fileinfo);
		});
		files.push_back(make_pair(index, filelist));
	});

	return TRUE;
}

BOOL CnxrmRMCApp::SetFileToOffline(std::vector<std::pair<int, std::vector<RepositoryFileInfo>>>& files)
{
	std::vector<std::pair<std::wstring, std::vector<NX::RmRepoFile>>> offfiles;
	for_each(files.cbegin(), files.cend(), [&](const std::pair<int, std::vector<RepositoryFileInfo>> & file) {
		std::vector<NX::RmRepoFile> repofiles;
		for_each(file.second.cbegin(), file.second.cend(), [&](const RepositoryFileInfo rfile) {
			NX::RmRepoFile repofile(rfile.strPath, rfile.strDisplayPath, rfile.strFileName, (rfile.bIsDirectory == TRUE), rfile.iFileSize, rfile.lFileDate);
			repofiles.push_back(repofile);
		});
		std::wstring id;
		{
			lock_guard<mutex> lock(m_mutRepoData);
			ASSERT(file.first < (int)m_RepositoryList.size());
			id = m_RepositoryList[file.first].GetId();
		}
		offfiles.push_back(make_pair(id, repofiles));
	});
	auto res = m_pSession->RepoSaveOfflineFiles(offfiles);
	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_SET_OFFLINE_FILE).GetBuffer()/*L"Fail to set offline file"*/).c_str());
	}

	return (res);
}

BOOL CnxrmRMCApp::GetFileActivityLog(std::wstring filename, std::wstring uid, vector<FileActivity> & fileactivities)
{
	NX::Result res;
	std::vector<NX::RmFileActivityRecord> logs;
	res = m_pSession->FetchAllActivityLogs(filename, logs, uid);

	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_GET_ACTIVITY_LOGS).GetBuffer()/*L"Failed to get the activity logs."*/).c_str());
	}
	else {
		fileactivities.clear();
		for (auto rec: logs) {
			FileActivity fact;
			fact.lFileDate = rec.GetAccessTime();
			NX::time::datetime dt = NX::time::datetime::from_unix_time(fact.lFileDate);
			SYSTEMTIME st;
			dt.to_systemtime(&st, true);
			fact.strFileTime = FormatFileTimeString(st);
			fact.strDevice = rec.GetDeviceType();
			fact.strDeviceName = rec.GetDeviceId();
			fact.strOperation = rec.GetOperation();
			fact.strOperator = rec.GetEmail();
			fact.bAllowed = !rec.GetAccessResult().compare(L"Allow");
			fileactivities.push_back(fact);
		}
	}

	return (res);
}

BOOL CnxrmRMCApp::ViewLocalFile(RepositoryFileInfo file)
{
	bool bcancel = false;
	wstringstream txtOut;
	NX::Result res = m_pSession->ViewLocalFile(file.strPath, file.strDisplayPath, &bcancel);
	if (res) {
		txtOut << theApp.LoadString(IDS_FILE_OPENED_IN_LOCAL_VIEWER).GetBuffer()/*L"The file has been opened in the local viewer."*/ << std::endl;
		TRACE(L"%s",txtOut.str().c_str());
	}
	else {
        CString txtOut;
        txtOut.Format(IDS_FMT_S_FAILED_OPEN_FILE_FOR_VIEW, file.strFileName.c_str());
		//textOut << L"Failed to open the file " << file.strFileName << L" for viewing." << std::endl;
		ShowTrayMsg(OutputError(res, txtOut.GetBuffer()).c_str());
	}
	return (res);
}

BOOL CnxrmRMCApp::ViewMyVaultFile(std::wstring path, std::wstring filename)
{
	wstringstream txtOut;
	NX::RmRepository rmrepo;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (0 == (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[0];
	}

	NX::Result res = m_pSession->ViewMyVaultFile(rmrepo, path, NULL);
	if (res) {
		txtOut << L"The file is opened in the remote viewer." << std::endl;
		TRACE(L"%s", txtOut.str().c_str());
	}
	else {
        CString txtOut;
        txtOut.Format(IDS_FMT_S_FAILED_OPEN_FILE_FOR_VIEW, filename.c_str());
		//txtOut << L"Failed to open the file " << filename << L" for viewing.";
		ShowTrayMsg(OutputError(res, txtOut.GetBuffer()).c_str());
	}

	return (res);
}

BOOL CnxrmRMCApp::ViewRepositoryFile(int index, std::wstring path, std::wstring filename)
{
	NX::RmRepository rmrepo;
	{
		lock_guard<mutex> lock(m_mutRepoData);
		if (index >= (int)m_RepositoryList.size())
			return FALSE;
		rmrepo = m_RepositoryList[index];
	}


	NX::Result res = m_pSession->ViewRepoFile(rmrepo, path, NULL);
	wstringstream txtOut;
	if (res) {
		txtOut << L"The file is opened in the remote viewer." << std::endl;
		TRACE(L"%s", txtOut.str().c_str());
	}
	else {
        CString txtOut;
        txtOut.Format(IDS_FMT_S_FAILED_OPEN_FILE_FOR_VIEW, filename.c_str());
		//txtOut << L"Failed to open the file " << filename << L" for viewing.";
		ShowTrayMsg(OutputError(res, txtOut.GetBuffer()).c_str());
	}

	return (res);
}


int GetRepoSelIconID(const int repoType)
{
    try
    {
        void *p = iconTable[repoType];
        if (!p)
        {
            return 0;
        }
        return ((RepoData*)p)->nIcoSelID;
    }
    catch (...)
    {
        return 0;
    }
}

int GetRepoIconID(const int repoType)
{
    try
    {
        void *p = iconTable[repoType];
        return ((RepoData*)p)->nIcoID;
    }
    catch (...)
    {
        return 0;
    }
}

int GetRepoImageID(const int repoType)
{
    try
    {
        void *p = iconTable[repoType];
        if (!p)
        {
            return 0;
        }
        return ((RepoData*)p)->nImgID;
    }
    catch (...)
    {
        return 0;
    }
}


std::wstring GetDriveTypeName(const int repoType)
{
    try
    {
        void *p = iconTable[repoType];
        if (!p)
        {
            return 0;
        }
        return ((RepoData*)p)->driveName.GetBuffer();
    }
    catch (...)
    {
        return L"";
    }
}

//Project Functions:

bool CnxrmRMCApp::CreateProject(NXProjData &projData, vector<wstring> &addrArr, AddDlgStyle dlgStyle, wstring &projID)
{
    bool retVal = false;
    //wstring projID;
    auto res = theApp.m_pSession->ProjectCreate(projData.szProjName, projData.szProjDesc, L"", addrArr, projID);

    if (!res) {
        if (res.GetCode() == 12007)
        {
            MessageBox(m_mainhWnd, theApp.LoadString(IDS_NO_NETWORK_CONNECTION)/*L"No network connection."*/, L"Error", MB_OK | MB_ICONERROR);
        }
        else
        {
            ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_CREATE_PROJECT).GetBuffer()/*L"Fail to Create Project"*/).c_str());
        }
    }
    else {
        CString txtOut;
        txtOut.Format(IDS_FMT_S_PROJECT_CREATED, projData.szProjName);
        //txtOut << L"The project " << projData.szProjName << L" has been created." << endl;
        if (dlgStyle == NX_ADD_BUTTON_HOMEPAGE)
        {
            txtOut  = txtOut + L"\n" + theApp.LoadString(IDS_NEWLY_ADDED_PROJ_LISTED)/*L"The newly added project is listed on the projects page."*/;
        }
        theApp.ShowTrayMsg(txtOut);

        theApp.m_dlgProjPage.RefreshProjList(TRUE);
        theApp.m_homePage.DisplayProjectList();

        retVal = true;
    }
    return retVal;
}


bool CnxrmRMCApp::FindProj(const wstring projID, CProjData *pProjData)
{
	lock_guard<mutex> lock(m_mutProject);
	for (auto x : theApp.m_AllProjects) {
		if (x.GetId() == projID) {
			CProjData proj;
			proj.m_projName = x.GetName().c_str();
			proj.m_imageID = 0;
			if(x.OwnedByMe())
				proj.m_flag = NXRMC_PROJ_BY_ME;
			else {
				proj.m_flag = NXRMC_PROJ_BY_OTHERS;
				auto projOwner = x.GetOwner();
				proj.m_ownerName = projOwner.GetName().c_str();
				proj.m_invitorName = L"";
			}

			proj.m_fileCount = x.GetFileCount();
			proj.m_ownerID = x.GetOwner().GetUserId();
			proj.m_desc = x.GetDescription().c_str();
			proj.m_nID = x.GetId();
			proj.m_creationTime = x.GetCreationTime();
			proj.m_imembercount = x.GetMemberCount();
			proj.m_Simmembers = x.GetMembers();
			auto size = proj.m_Simmembers.size();
			proj.m_initials.clear();
			for (auto member : proj.m_Simmembers)
			{
				proj.m_initials.push_back(member.GetDisplayName().c_str());
			}

			*pProjData = proj;
            return true;
		}
	}
    return false;
}

bool CnxrmRMCApp::AddNewProject(HWND hwnd)
{
    bool retVal = false;
    NXProjData projData = { L"", L"" };
    projData.projFlag = NX_PROJ_CREATE;
    LPWSTR pAddresses;
    wstring projID;
    if (!RmuShowProjDialog(hwnd, projData, &pAddresses))
    {
        vector<wstring> peopleArr;
        auto size = pAddresses != NULL ? lstrlen(pAddresses) : 0;
        if (size)
        {

            CString addresses = pAddresses;
            int curPos = 0;
            CString resToken = addresses.Tokenize(_T(";"), curPos);
            while (resToken != _T(""))
            {
                peopleArr.push_back(resToken.GetBuffer());
                resToken = addresses.Tokenize(_T(";"), curPos);
            };
            RmuFreeResource(pAddresses);
        }
        CString projName = projData.szProjName;
        lstrcpy(projData.szProjName, projName.Trim());

        if (CreateProject(projData, peopleArr, NX_ADD_BUTTON_HOMEPAGE, projID))
        {
            theApp.RefreshProjectList();
            theApp.m_dlgRepoPage.m_switchDlg.RefreshProjList();

            CProjData ProjData;
            if (FindProj(projID, &ProjData))
            {
                theApp.ShowPage(MD_PROJECTMAIN);
                theApp.m_dlgProjMain.SetProject(ProjData);
                if (theApp.m_dlgProjMain.m_switchDlg.GetSafeHwnd())
                {
                    theApp.m_dlgProjMain.m_switchDlg.RefreshProjList();
                }
                theApp.m_dlgProjMain.ShowProjPage(NX_PROJ_SUMMARY_PAGE);
            }
			if (m_bHideProjectMenu) {
				ReloadMainMenu(hwnd);
				m_bHideProjectMenu = FALSE;
			}
            retVal = true;
        }
    }
    return retVal;
}


bool CnxrmRMCApp::ProjectAcceptInvitation(const std::wstring& id, const std::wstring& code, std::wstring& projectId)
{

    auto res = theApp.m_pSession->ProjectAcceptInvitation(id, code, projectId);
    if (!res) {
        //TRACE(OutputError(res, theApp.LoadString(IDS_FAILED_ACCEPT_INVITE).GetBuffer()/*L"Fail to accept invitation"*/).c_str());
        OutputError(res, L"Failed to accept the invitation");
    }
    else
    {
		if (m_bHideProjectMenu) {
			ReloadMainMenu(m_mainhWnd);
			m_bHideProjectMenu = FALSE;
		}
        return true;
    }

    return false;
}

bool CnxrmRMCApp::ProjectDeclineInvitation(const std::wstring& id, const std::wstring& code, const std::wstring& reason)
{
    auto res = theApp.m_pSession->ProjectDeclineInvitation(id, code, reason);
    if (!res) {
        //TRACE(OutputError(res, theApp.LoadString(IDS_FAILED_DECLINE_INVITATION).GetBuffer()/*L"Fail to decline invitation"*/).c_str());
        TRACE(OutputError(res, L"Fail to decline invitation").c_str());
    }
    else
    {
        return true;
    }
    return false;
}

bool CnxrmRMCApp::ProjectFindPendingInvitations(const std::wstring & projectId, const wstring &keyword, std::vector<NX::RmProjectInvitation>& invites)
{
    //std::vector<NX::RmProjectMember> tmpMembers;
    std::vector<NX::RmProjectInvitation> invitations;
    NX::RmFinder finder(100, std::vector<int>{NX::OrderByDisplayNameAscend}, L"", L"email,name", keyword);
    NX::Result res = m_pSession->ProjectGetFirstPagePendingInvitations(finder, projectId, invitations);
    if (!res)
    {
        TRACE(OutputError(res, L"ProjectGetFirstPagePendingInvitations call failed").c_str());
        return false;
    }
    do {
        std::for_each(invitations.begin(), invitations.end(), [&](const NX::RmProjectInvitation& invite) {
            invites.push_back(invite);
        });
        invitations.clear();
    } while (res = m_pSession->ProjectGetNextPagePendingInvitations(finder, projectId, invitations));
    return true;
}

bool CnxrmRMCApp::ProjectFindPendingMembers(const CProjData &projData, const wstring & keyword, vector<ProjMember>& members)
{
    bool retVal = false;
    std::vector<NX::RmProjectInvitation> invites;
    if (ProjectFindPendingInvitations(projData.m_nID, keyword, invites))
    {
        for (auto &m : invites)
        {
            ProjMember memberData;
            memberData.projID = projData.m_nID;
            memberData.memberID = m.GetId();
            memberData.bProjByMe = (projData.m_flag == NXRMC_PROJ_BY_ME);
            memberData.bPending = true;
            memberData.bOwner = false;
            
            memberData.memberName = m.GetInviteeEmail();
            memberData.emailAddr = m.GetInviteeEmail();
            memberData.invitorName = m.GetInviterDisplayName();
            memberData.dt = NX::time::datetime::from_unix_time(m.GetInviteTime());
            members.push_back(memberData);
        }
        retVal = true;
    }
    return retVal;
}




bool CnxrmRMCApp::FindProjectMembers(const CProjData &projData, const wstring &keyWord, const wstring ownerId, vector<ProjMember> &members)
{
    NX::RmFinder finder(100, std::vector<int>({ NX::OrderByDisplayNameAscend }), L"", L"name,email", keyWord);
    vector<NX::RmProjectMember> projMembers;
    //ProjectGetMemberInfo
    auto res = m_pSession->ProjectGetFirstPageMembers(finder, projData.m_nID, false, projMembers);
    if (!res) {
        TRACE(OutputError(res, L"Fail to list 1st page of members").c_str());
        return false;

    }
    else
    {
        do
        {
            for_each(projMembers.begin(), projMembers.end(), [&](NX::RmProjectMember m)
            {
                ProjMember memberData;
                NX::RmProjectMember metaMember;
                m_pSession->ProjectGetMemberInfo(projData.m_nID, m.GetUserId(), metaMember);
                memberData.projID = projData.m_nID;
                memberData.memberID = m.GetUserId();
                memberData.bProjByMe = (projData.m_flag == NXRMC_PROJ_BY_ME);
                memberData.bPending = false;
                memberData.bOwner = (m.GetUserId() == ownerId);
                memberData.memberName = m.GetDisplayName();
                memberData.emailAddr = m.GetEmail();
                auto invitorName = metaMember.GetInviterDisplayName();
                if (invitorName == L"")
                {
                    invitorName = metaMember.GetInviterEmail();
                }
                memberData.invitorName = invitorName;
                memberData.dt = NX::time::datetime::from_unix_time(m.GetCreationTime());
                
                auto timeText = GetFileTimeString(memberData.dt);

                wstringstream txtOut;
                txtOut << timeText.GetBuffer();
                memberData.desc = txtOut.str();
                members.push_back(memberData);
            });
            projMembers.clear();
        } while (res = m_pSession->ProjectGetNextPageMembers(finder, projData.m_nID, false, projMembers));
    }
    return true;
}

bool CnxrmRMCApp::InvitePeopleToProject(const CProjData &curProjData)
{
    bool retVal = false;
    NXProjData projData = { L"", L"" };

    lstrcpy(projData.szProjName, curProjData.m_projName);
    wstring projDesc = curProjData.m_desc;
    lstrcpy(projData.szProjDesc, projDesc.c_str());
    projData.projFlag = NX_PROJ_INVITE;
    LPWSTR pAddresses;
    if (!RmuShowProjDialog(m_mainhWnd, projData, &pAddresses))
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
            //InvitePeople here
            std::vector<std::wstring> alreadyInvited;
            std::vector<std::wstring> nowInvited;
            std::vector<std::wstring> alreadyMembers;

            auto res = theApp.m_pSession->ProjectInvite(curProjData.m_nID,
                peopleArr,
                alreadyInvited, nowInvited, alreadyMembers);
            if (!res) {
               TRACE(OutputError(res, L"Fail to invite people:").c_str());
            }
            else {
                size_t total = alreadyInvited.size();
                if (total)
                {
                    if (total == 1)
                    {
                        wstringstream txtOut;
                        txtOut << alreadyInvited[0].c_str() << theApp.LoadString(IDS_IS_ALREADY_INVITED_SUFFIX).GetBuffer()/* L" is already invited." */<< endl;
                        theApp.ShowTrayMsg(txtOut.str().c_str());
                        retVal = false;
                    }
                    else
                    {
                        wstringstream txtOut;
                        txtOut << alreadyInvited[0].c_str() << theApp.LoadString(IDS_MORE_ALREADY_INVITED_SUFFIX).GetBuffer()/*L" and more people are already invited."*/ << endl;
                        theApp.ShowTrayMsg(txtOut.str().c_str());
                        retVal = false;
                    }
                }
                total = alreadyMembers.size();
                if (total)
                {
                    if (total == 1)
                    {
                        wstringstream txtOut;
                        txtOut << alreadyMembers[0].c_str() << theApp.LoadString(IDS_IS_ALREDY_MEMBER_SUFFIX).GetBuffer()/* L" is already a member."*/ << endl;
                        theApp.ShowTrayMsg(txtOut.str().c_str());
                        retVal = false;
                    }
                    else
                    {
                        wstringstream txtOut;
                        txtOut << alreadyMembers[0].c_str() << theApp.LoadString(IDS_IS_ALREDY_MEMBER_SUFFIX).GetBuffer()/*L" and more people are already members."*/ << endl;
                        theApp.ShowTrayMsg(txtOut.str().c_str());
                        retVal = false;
                    }
                }
                total = nowInvited.size();
                if (nowInvited.size())
                {

                    if (total == 1)
                    {
                        wstringstream txtOut;
                        txtOut << nowInvited[0].c_str() << theApp.LoadStringW(IDS_IS_INVITED).GetBuffer() << endl;
                        theApp.ShowTrayMsg(txtOut.str().c_str());
                        retVal = true;
                    }
                    else
                    {
                        wstringstream txtOut;
                        txtOut << nowInvited[0].c_str() << theApp.LoadStringW(IDS_ARE_INVITED).GetBuffer() << endl;
                        theApp.ShowTrayMsg(txtOut.str().c_str());
                        retVal = true;
                    }

                }
            }
        }
        RmuFreeResource(pAddresses);
    }
    return retVal;
}

void CnxrmRMCApp::LogoutUser(void)
{
    if (IsWindowEnabled(m_mainhWnd))
    {
        theApp.m_pSession->Logout();
        if (m_mainhWnd != NULL) {
            DestroyMenu(theApp.m_hMainMenu);
            theApp.m_hMainMenu = LoadMenuW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_NXRMC_SHORT));
            ::SetMenu(theApp.m_mainhWnd, theApp.m_hMainMenu);
            theApp.m_dlgRepoPage.ResetData();
            theApp.ShowPage(MD_INTRODUCTION);
            CWnd::FromHandle(theApp.m_mainhWnd)->InvalidateRect(NULL);
        }
    }
}

void CnxrmRMCApp::ReloadMainMenu(HWND hwnd)
{
	if(NULL != theApp.m_hMainMenu)
		DestroyMenu(theApp.m_hMainMenu);
	theApp.m_hMainMenu = LoadMenuW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_NXRMC));

	if (0 == GetProjectCount()) {
		RemoveMenu(theApp.m_hMainMenu, 3, MF_BYPOSITION);
		m_bHideProjectMenu = TRUE;
	}
	else {
		m_bHideProjectMenu = FALSE;
	}
	::SetMenu(hwnd, theApp.m_hMainMenu);
}

BOOL CnxrmRMCApp::CreateProjectFolder(const wstring &projID, const std::wstring path, const std::wstring &newfolder)
{
    NX::RmRepoFile newFolder;
    auto res = m_pSession->ProjectCreateFolder(projID, path, newfolder, false, newFolder);
    if (!res) {
        if(res.GetCode() == 4001)
        {
            ShowTrayMsg(theApp.LoadString(IDS_FAILED_CREATE_FOLDER_ERRNAME)/*L"Failed to create the folder (invalid folder name)."*/);
        }
        else
        {
            ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_CREATE_PROJ_FOLDER).GetBuffer()/*L"Fail to create project folder"*/).c_str());
        }
    }
    else
    {
        CString txtOut;
        txtOut.Format(IDS_FMT_S_FOLDER_CREATED, newfolder.c_str());
        //txtOut << L"The folder " << newfolder << L" is created.";
        theApp.ShowTrayMsg(txtOut);

    }
    return (res);
}

BOOL CnxrmRMCApp::DeleteProjectFile(const wstring &projID, const std::wstring &path)
{
    NX::RmRepoFile newFolder;
    auto res = m_pSession->ProjectDeleteFile(projID, path);
    if (!res) {
        theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILE_TO_DELETE_PROJ_FILE).GetBuffer()/*L"Fail to delete project file"*/).c_str());
    }
    return (res);
}

void DownloadPackageProc(BOOL bSilent, StatusCallBackFunc func /* = NULL */)
{
	auto res = theApp.m_pSession->DownloadUpgradeInstaller(theApp.m_productUpgradeInfo);
	if (res) {
		ASSERT(!theApp.m_productUpgradeInfo.GetDownloadedFile().empty());
		if (!theApp.m_productUpgradeInfo.GetDownloadedFile().empty() && func)
		{
			(*g_dwninfo.func)(g_dwninfo.callbackMessage);
		}
	}
	else {
		theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_DOWNLOAD_NEW_VER).GetBuffer()/*L"Failed to download the new version."*/).c_str());
	}
	if(!bSilent)
		g_progressFuncs.closeProgress();
}


std::wstring CnxrmRMCApp::CheckForUpdate(BOOL *bUptoData)
{
	if (!m_pSession) {
		if (bUptoData)
			*bUptoData = FALSE;
		return L"";
	}
	m_productUpgradeInfo = m_pSession->CheckForUpgrade();
	if (!m_productUpgradeInfo.Empty()) {
		if (bUptoData)
			*bUptoData = TRUE;
		return m_productUpgradeInfo.GetVersion();
	}

	if (bUptoData)
		*bUptoData = FALSE;
	return L"";
}

void CnxrmRMCApp::DownloadNewVersion(BOOL bSilent, StatusCallBackFunc func /* = NULL */, MSG * message /* = NULL */)
{
	if (!bSilent) {
		g_progressFuncs.titleText = theApp.LoadString(IDS_DOWNLOAD_PKG).GetBuffer()/*L"Downloading Package..."*/;
		if (!RmuOpenProgressWindow(theApp.m_mainhWnd, g_progressFuncs, true)) {
			ShowTrayMsg(theApp.LoadString(IDS_FAILED_LAUNCH_THREAD).GetBuffer()/*L"Failed to launch the thread."*/);
			return;
		}
	}
	if (g_packagethrd.joinable())
	{
		g_packagethrd.join();
	}
	memset(&g_dwninfo.callbackMessage, 0, sizeof(g_dwninfo.callbackMessage));
	if (NULL != message) {
		g_dwninfo.callbackMessage = *message;
	}
	g_dwninfo.func = func;
	std::thread DownloadTh(DownloadPackageProc, bSilent, func);
	g_packagethrd = move(DownloadTh);
}

void CnxrmRMCApp::InstallNewVersion() 
{
	NX::RmUpgrade upgrader(m_productUpgradeInfo);
	wstringstream txtOut;
	txtOut << "Installing the new version ..." << std::endl;
	auto res = upgrader.InstallNewVersion();
	if (!res) {
		ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_INSTALL_NVIEW_VER).GetBuffer()/*L"Failed to install the new version."*/).c_str());
	}
	else
		txtOut << "Done" << std::endl;
	TRACE(L"%s",txtOut.str().c_str());
}


DWORD CALLBACK DownloadProjFileProc(_In_opt_ PVOID lpData)
{
	wstring fullPath;
    auto res = theApp.m_pSession->ProjectDownloadFile(g_dwninfo.key, g_dwninfo.filepath, g_dwninfo.destpath, false, fullPath, &g_bCancel);
    if (!res) {
        theApp.ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_DOWNLOAD_PROJ_FILE).GetBuffer()/*L"Failed to download the project file."*/).c_str());

    }
    else
    {
		CString outMsg = theApp.LoadString(IDS_FILE_DOWNLODED_TO_SUFFIX)/*L"The file has been downloaded to \n"*/;
		outMsg.Append(fullPath.c_str());
        theApp.ShowTrayMsg(outMsg);
    }
    g_progressFuncs.closeProgress();

	return 0;
}


void CnxrmRMCApp::DownloadProjectFile(const wstring & projID, const wstring source, wstring targetFolder)
{
	if (IsProgressDlgRunning())
		return ;
	CloseHandle(g_progThread);
	g_progThread = INVALID_HANDLE_VALUE;

	g_dwninfo.key = projID;
	g_dwninfo.filepath = source;
	g_dwninfo.destpath = targetFolder;
    g_progressFuncs.titleText = theApp.LoadString(IDS_DOWNLOAD).GetBuffer();//L"Download...";
    if (RmuOpenProgressWindow(theApp.m_mainhWnd, g_progressFuncs, true))
    {
		g_bCancel = FALSE;
		g_progThread = ::CreateThread(NULL, 0, DownloadProjFileProc, &g_dwninfo, 0, NULL);
    }
}



void CnxrmRMCApp::ShowProjectFileProperty(const wstring &projID, const ProjFileData &fileinfo)
{
    BOOL bret = FALSE;
    CDlgFileInfo dlg(CWnd::FromHandle(m_mainhWnd));
    NX::RmProjectFileMetadata metadata;
    auto res = m_pSession->ProjectGetFileMetadata(projID, fileinfo.filePath, metadata);
    if (!res)
    {
        ShowTrayMsg(OutputError(res, theApp.LoadString(IDS_FAILED_TO_GET_FILE_INFO).GetBuffer() /*L"Fail to get File Information"*/).c_str());
        return;
    }
    else
    {
        dlg.m_fileRights = metadata.GetRights();
    }
    dlg.m_strFileName = fileinfo.fileName;
    dlg.m_bProjectFile = TRUE;
    dlg.m_strFileSize = GetSpaceString(fileinfo.fileSize).c_str();
    dlg.m_strFileModify = GetFileTimeString(fileinfo.dt);
    dlg.m_strFilePath = GetParentFolder(fileinfo.filePath).c_str();
    dlg.m_bRepositoryFile = true;
    auto ownerID = m_pSession->GetCurrentUser().GetId();
    auto projFileOwnerID = fileinfo.repoFile.GetOwner().GetUserId();
    if (ownerID == projFileOwnerID) {
        dlg.m_bOwner = TRUE;
    }
    if (dlg.DoModal() == IDOK) {
    }
}

bool CnxrmRMCApp::IsProjectFileDownloadable(const wstring &projID, const ProjFileData &fileinfo)
{
    BOOL bret = FALSE;
    NX::RmProjectFileMetadata metadata;
    auto res = m_pSession->ProjectGetFileMetadata(projID, fileinfo.filePath, metadata);
    if (!res)
    {
        return false;
    }
    else
    {
        //dlg.m_fileRights = metadata.GetRights();
        CString currUserEmailAddress = theApp.m_pSession->GetCurrentUser().GetEmail().c_str();
        if (fileinfo.ownerEmail != currUserEmailAddress)
        {
            auto fileRights = metadata.GetRights();
            for each (auto x in fileRights)
            {
                if (x == L"DOWNLOAD")
                {
                    return true;
                }
            }
        }
        else
        {
            return true;
        }
    }
    return false;
}



BOOL CnxrmRMCApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
	if (m_pSession != nullptr) {
		if (theApp.m_pSession->ValidateCredential()) { //only handle the message after user is login
			if (TranslateAccelerator(theApp.m_mainhWnd, theApp.m_hAccel, lpMsg)) {
				return TRUE;
			}
		}
	}
	return CWinApp::ProcessMessageFilter(code, lpMsg);
}

CString CnxrmRMCApp::LoadString(int nID)
{
    wchar_t szString[MAX_PATH];
    AfxLoadString(nID, szString, MAX_PATH);
    return szString;
}

bool CnxrmRMCApp::RemoveProjMember(const std::wstring& projid, std::wstring& memberId)
{

    auto res = theApp.m_pSession->ProjectRemoveMember(projid, memberId);
    if (!res) {
       TRACE( OutputError(res, theApp.LoadString(IDS_FAILED_REMOVE_MEMBER).GetBuffer()/*L"Fail to remove member"*/).c_str());
    }
    else
    {
        return true;
    }

    return false;
}


bool CnxrmRMCApp::ResendInvitation(const std::wstring& invitationId, const std::wstring emailAddr)
{

    auto res = theApp.m_pSession->ProjectSendInvitationReminder(invitationId);
    if (!res) {
       TRACE( OutputError(res, theApp.LoadString(IDS_FAILED_RESEND_INVITE).GetBuffer()/*L"Fail to resend invitation"*/).c_str());
    }
    else
    {
        std::wstring outText = theApp.LoadString(IDS_INVITATION_RESENT_PREFIX).GetBuffer()/*L"The invitation has been re-sent to "*/ + emailAddr;
        ShowTrayMsg(outText.c_str());
        return true;
    }

    return false;
}

bool CnxrmRMCApp::RevokeInvitation(const std::wstring& invitationId)
{

    auto res = theApp.m_pSession->ProjectRevokeInvitation(invitationId);
    if (!res) {
       TRACE( OutputError(res, theApp.LoadString(IDS_FAILED_REVOKE).GetBuffer()/*L"Fail to revoke invitation"*/).c_str());
    }
    else
    {
        return true;
    }

    return false;
}


bool CnxrmRMCApp::IsExtension(CString fName, CString ext)
{
    int pos = fName.ReverseFind(L'.');
    auto len = fName.GetLength();
    CString fExtension = fName.Right(len - pos);
    return !fExtension.CompareNoCase(ext);
}

void CnxrmRMCApp::SetMyDriveCurrentPath(std::wstring path)
{
	lock_guard<mutex> lock(m_mutMyDriveFiles);
	if (0 == path.compare(m_strMyDrivePath))
		return;
	m_MyDriveLastQueryTime = 0;
	m_strMyDrivePath = path;
}

// void CnxrmRMCApp::RefreshMyVaultFileList(void)
// {
// 	std::vector<NX::RmRepoFile> files;
// 	NX::Result nres;
// 
// 	nres = m_pSession->MyVaultGetFolderMetadata(m_MyVaultLastQueryTime, &g_bExitFlag, files);
// 
// 	if (!nres) {
// 		if (nres.GetCode() != 304) {
// 			OutputError(nres, L"RefreshMyDrive get Metadata Error:");
// 		}
// 		return;
// 	}
// 
// 	for (auto x : files) {//need update when API changed.
// 		if (x.GetLastModifiedTime() > m_MyVaultLastQueryTime)
// 			m_MyVaultLastQueryTime = x.GetLastModifiedTime();
// 	}
// 
// 	if (IsWindow(m_dlgRepoPage.m_hWnd) && m_dlgRepoPage.m_selBtnIdx != NX_BTN_MYDRIVE && m_dlgRepoPage.m_selBtnIdx != NX_BTN_OTHERREPOS) {
// 		::PostMessage(m_dlgRepoPage.m_hWnd, WM_NX_REFRESh_MYVALUT_LIST, 0, 0);
// 	}
// 
// 	return;
// }

void CnxrmRMCApp::SetCurrentProjInfo(wstring projID,wstring currFilePathId, long long lastModifiedTime)
{
    lock_guard<mutex> lock(m_mutProject);
    m_curProjID = projID;
    m_currFolderId = currFilePathId;
    m_lastModifiedTime = lastModifiedTime;
}

wstring CnxrmRMCApp::GetCurrentProjID()
{
    return m_curProjID;
}

BOOL CnxrmRMCApp::RefreshCurrentProject(void)
{
    if (!m_curProjID.size())
    {
        return TRUE;
    }
    BOOL bUpdateRecent = FALSE;
    BOOL bUpdateFiles = FALSE;
    BOOL bUpdateMembers = FALSE;
    std::vector<NX::RmProjectFile> files;
    std::vector<NX::RmProjectFile> folderFiles;
    vector<NX::RmProjectMember> projMembers;

    do
    {
        NX::RmFinder finder(10, std::vector<int>({ OrderByCreationTimeDescend }), L"", L"", L"");
        bool bcancel = false;
        auto res = theApp.m_pSession->ProjectGetFirstPageFiles(finder, &bcancel, m_curProjID, L"", NULL, NULL, NULL, files);
        if (!res) {
           TRACE( OutputError(res, theApp.LoadString(IDS_FAILED_LIST_1ST_PAGE_FILE).GetBuffer()/*L"Fail to list 1st page of files"*/).c_str());
            return FALSE;
        }
        else
        {
            if (m_recentFiles.size() != files.size())
            {
                bUpdateRecent = TRUE;
            }
            else
            {
                for (size_t i = 0; i < files.size(); i++)
                {
                    if (m_recentFiles[i].GetLastModifiedTime() != files[i].GetLastModifiedTime())
                    {
                        bUpdateRecent = TRUE;
                    }
                }
            }
        }
        
        NX::RmFinder finder1(100, std::vector<int>({ OrderByCreationTimeDescend }), L"", L"", L"");
        //ProjectGetMemberInfo
        res = m_pSession->ProjectGetFirstPageMembers(finder1, m_curProjID, false, projMembers);
        if (!res) {
            TRACE(OutputError(res, theApp.LoadString(IDS_FAILED_LIST_1ST_PAGE_MEMBERS).GetBuffer()/*L"Fail to list 1st page of members"*/).c_str());
            return FALSE;
        }
        else
        {
            if (m_recentMembers.size() != projMembers.size())
            {
                bUpdateMembers = TRUE;
            }
            else
            {
                for (size_t i = 0; i < projMembers.size(); i++)
                {
                    if (m_recentMembers[i].GetCreationTime() != projMembers[i].GetCreationTime())
                    {
                        bUpdateMembers = TRUE;
                    }
                }
            }
        }

        NX::RmFinder finder2(100, std::vector<int>({ OrderByCreationTimeDescend }), L"", L"", L"");
        res = m_pSession->ProjectGetFolderFirstPageMetadata(finder2, false, m_curProjID, m_currFolderId, m_lastModifiedTime, NULL, NULL, folderFiles);
        //NX::time::datetime dt = NX::time::datetime::from_unix_time(m_lastModifiedTime);
        //auto timeStr = GetFileTimeString(dt);
        //TRACE(L"Test Modified time%s\n", timeStr);
        if (!res) {
            if (res.GetCode() == 304)
            {
                break;
            }
            else
            {
                TRACE(OutputError(res, L"Fail to list 1st page of members").c_str());
                return FALSE;
            }
        }
        else
        {
            bUpdateFiles = true;
        }
    } while (false);
    if (bUpdateRecent || bUpdateMembers || bUpdateFiles )
    {//Lets just refresh all pages at this time.
        m_recentFiles.clear();
        m_recentMembers.clear();
        m_recentFiles = files;
        m_recentMembers = projMembers;
        if (IsWindow(m_dlgProjMain.m_hWnd)) {
            ::PostMessage(m_dlgProjMain.m_hWnd, WM_NX_TIMED_REFRESH_ONE_PROJ, 0, 0);
        }
    }

    return TRUE;
}

BOOL CnxrmRMCApp::RefreshProjectList(void)
{
	//To optimize the performance, refresh invitation should be separate from refresh project.
	RefreshInvatationList();
	size_t nprojbyme;
	size_t nprojbyother;

	{
		lock_guard<mutex> lock(m_mutProject);
		nprojbyme = m_ProjectByMeCount;
		nprojbyother = m_ProjectByOthersCount;
	}

	NX::RmFinder finder(5, std::vector<int>({ NX::OrderByLastActionTimeDescend , NX::OrderByNameAscend }), L"", L"", L"");
	std::vector<NX::RmProject> projects;
	std::vector<NX::RmProject> allprojects;
	BOOL bupdate = FALSE;
	auto res = m_pSession->ProjectListFirstPageProjects(finder, projects, &g_bExitFlag);
	if (!res) {
		TRACE(OutputError(res, L"Fail to list projects").c_str());
		return false;
	}
	else
	{
		size_t nTotalCount = (size_t)finder.GetTotalRecords();
		for(auto project: projects) {
			allprojects.push_back(project);
		}
		while (nTotalCount > allprojects.size()) {
			projects.clear();
			res = m_pSession->ProjectListNextPageProjects(finder, projects, &g_bExitFlag);
			if (!res) {
				TRACE(OutputError(res, L"Fail to list projects").c_str());
				return false;
			}
			for (auto project : projects) {
				allprojects.push_back(project);
			}
		}
		
		if (allprojects.size() == nprojbyme + nprojbyother) {//compare the list
			lock_guard<mutex> lock(m_mutProject);
			size_t i;
			//for (j = 0; j < allprojects.size(); j++) {
				for (i = 0; i < m_AllProjects.size(); i++) {
					if (0 == allprojects[i].GetId().compare(m_AllProjects[i].GetId())) {//found the project
						if (allprojects[i].GetFileCount() != m_AllProjects[i].GetFileCount()) {
							bupdate = TRUE;
							break;
						}
                        else
                        {//Compare files

                        }
						if (allprojects[i].GetMemberCount() != m_AllProjects[i].GetMemberCount()) {

							bupdate = TRUE;
							break;
						} 
                        else
                        {//Compare members

                        }
					}
                    else
                    {
                        bupdate = TRUE;
                        break;
                    }

				if (i == m_AllProjects.size() || bupdate) {
					bupdate = TRUE;
					break;
				}
			}
		}
		else {
			bupdate = TRUE;
		}
	}

    if (bupdate)
    {
        lock_guard<mutex> lock(m_mutProject);
        m_AllProjects.clear();
        m_ProjectByOthersCount = 0;
        m_ProjectByMeCount = 0;
        for (auto project : allprojects) {
            m_AllProjects.push_back(project);
            if (project.OwnedByMe())
                m_ProjectByMeCount++;
            else
                m_ProjectByOthersCount++;
        };
		m_TotalProjectByMe = m_ProjectByMeCount;
		m_TotalProjectByOthers = m_ProjectByOthersCount;
        //std::sort(m_AllProjects.begin(), m_AllProjects.end(),
        //	[](NX::RmProject &a, const NX::RmProject &b)
        //{
        //	return a.GetCreationTime() > b.GetCreationTime();
        //});

        if (IsWindow(m_homePage.m_hWnd)) {
            ::PostMessage(m_homePage.m_hWnd, WM_NX_TIMED_REFRESH_PROJLIST, 0, 0);
        }
        if (IsWindow(m_dlgProjPage.m_hWnd)) {
            ::PostMessage(m_dlgProjPage.m_hWnd, WM_NX_TIMED_REFRESH_PROJLIST, 0, 0);
        }
    }
    
	return bupdate;
}

BOOL CnxrmRMCApp::RefreshInvatationList(void)
{
	size_t ninvitenum;

	{
		lock_guard<mutex> lock(m_mutInvitation);
		ninvitenum = m_InvatationList.size();
	}

	NX::Result nres = RESULT(2);
	std::vector<NX::RmProjectInvitation> invitations;
	BOOL bupdate = FALSE;
	nres = theApp.m_pSession->ProjectGetUserPendingInvitations(invitations, &g_bExitFlag);
	if (!nres) {

		TRACE(OutputError(nres, L"Fail to list project invites").c_str());
		return FALSE;
	}
	else if (ninvitenum != invitations.size()) {
		bupdate = TRUE;
	}else{//compare the list
		lock_guard<mutex> lock(m_mutInvitation);
		size_t i, j;
		for (j = 0; j < invitations.size(); j++) {
			for (i = 0; i < m_InvatationList.size(); i++) {
				if (0 == invitations[j].GetId().compare(m_InvatationList[i].GetId())) {//found the invitation
					break;
				}
			}

			if (i == m_InvatationList.size()) {
				bupdate = TRUE;
				break;
			}
		}
	}

	if (!bupdate)
		return TRUE;

	lock_guard<mutex> lock(m_mutInvitation);
	m_InvatationList.clear();
	std::for_each(invitations.begin(), invitations.end(), [&](const NX::RmProjectInvitation& invitation) {
		m_InvatationList.push_back(invitation);
	});

	std::sort(m_InvatationList.begin(), m_InvatationList.end(),
		[](NX::RmProjectInvitation &a, const NX::RmProjectInvitation &b)
	{
		return a.GetInviteTime() > b.GetInviteTime();
	});

	if (IsWindow(m_homePage.m_hWnd)) {
		::PostMessage(m_homePage.m_hWnd, WM_NX_TIMED_REFRESH_PROJLIST, 0, 0);
	}
	if (IsWindow(m_dlgProjPage.m_hWnd)) {
		::PostMessage(m_dlgProjPage.m_hWnd, WM_NX_TIMED_REFRESH_PROJLIST, 0, 0);
	}

	return TRUE;
}

BOOL CnxrmRMCApp::RefreshMyDriveFileList(void)
{
	NX::RmRepository rmrepo;
	std::wstring path;
	{
		lock_guard<mutex> lock(m_mutMyDriveFiles);
		if (0 == (int)m_RepositoryList.size())
			return TRUE;//this should not happen. return TRUE to avoid next refresh in short period
		rmrepo = m_RepositoryList[0];
		path = m_strMyDrivePath;
		if (path.length() == 0)
			path = L"/";
	}

	NX::Result nres = RESULT(2);
	std::vector<NX::RmRepoFile> files;
	if (!(IsWindow(m_dlgRepoPage.m_hWnd) && m_curShowedPage == MD_REPORITORYHOME && m_dlgRepoPage.m_selBtnIdx == NX_BTN_MYDRIVE)) {//only refresh when myDrive is show to user
		return FALSE;
	}
	if (m_MyDriveLastQueryTime == 0) {//force refresh for first time. this need be enhanced in future when local cache has last modify time
		__int64 itime;
		nres = m_pSession->RepoGetFolderFileListFromCache(rmrepo, path, files, &itime);
		files.clear();
		m_MyDriveLastQueryTime = itime;
	}

	nres = m_pSession->MyDriveGetFolderMetadata(path, m_MyDriveLastQueryTime, &g_bExitFlag, files);

	if (!nres) {
		if (nres.GetCode() != 304) {
			TRACE(OutputError(nres, L"RefreshMyDrive get Metadata Error:").c_str());
		}
		return TRUE;
	}

	for (auto x : files) {//need update when API changed.
		if (x.GetLastModifiedTime() > m_MyDriveLastQueryTime)
			m_MyDriveLastQueryTime = x.GetLastModifiedTime();
	}

	//fresh local cache
	nres = m_pSession->RepoGetFolderFileListFromRemote(rmrepo, path, files);

	if (!nres) {
		TRACE(OutputError(nres, L"RefreshMyDrive get filelist Error:").c_str());
		return TRUE;
	}

	if (IsWindow(m_dlgRepoPage.m_hWnd) && m_dlgRepoPage.m_selBtnIdx == NX_BTN_MYDRIVE) {
		::PostMessage(m_dlgRepoPage.m_hWnd, WM_NX_REFRESH_FILE_LIST, 0, 0);
	}

	return TRUE;
}

void CnxrmRMCApp::ListRecentProjectFiles(CProjData &projData, size_t maxFiles)
{
    projData.m_files.clear();
    projData.m_Dirs.clear();
    //m_fileData.clear();
    NX::RmFinder finder(100, std::vector<int>({ OrderByCreationTimeDescend }), L"", L"", L"");
    std::vector<NX::RmProjectFile> files;
    //std::wstring parentDir = currentPath;
    //if (L'/' != parentDir[parentDir.length() - 1])
    //    parentDir.append(L"/");
    bool bcancel = false;
    auto res = theApp.m_pSession->ProjectGetFirstPageFiles(finder, &bcancel, projData.m_nID, L"", NULL, NULL, NULL, files);
    if (!res) {
       TRACE( OutputError(res, L"Fail to list 1st page of files").c_str());
    }
    else {
        bool bFull = false;

        do {

            for each(auto file in files) 
            {
                if (!file.IsFolder())
                {
                    projData.m_files.push_back(file);
                    if (maxFiles <= projData.m_files.size())
                    {
                        bFull = true;
                        break;
                    }
                }
            }
            if (bFull)
            {
                break;
            }
        } while (res = theApp.m_pSession->ProjectGetNextPageFiles(finder, &bcancel, projData.m_nID, L"", files));
    }
}

bool CnxrmRMCApp::IsSkyDRMUser()
{
    return m_pSession->GetCurrentUser().GetIdpType() == NX::SKYDRM;
}

wstring CnxrmRMCApp::ReplaceTimestamp(const std::wstring& s)
{
    //wchar_t szNewFileName[MAX_PATH];
    wstring resultStr;
    static const std::wregex pattern(L"-\\d{4}-\\d{2}-\\d{2}-\\d{2}-\\d{2}-\\d{2}", std::regex_constants::icase);
    bool result = false;
    try {
        SYSTEMTIME st = { 0 };

        GetSystemTime(&st);
        // -YYYY-MM-DD-HH-MM-SS
        const std::wstring sTimestamp(NX::FormatString(L"-%04d-%02d-%02d-%02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond));
        resultStr = std::regex_replace(s, pattern, sTimestamp);

    }
    catch (...) {
        return s;
    }
    return resultStr;
}


void CnxrmRMCApp::ListProjectFiles(wstring  currentPath, CProjData &projData)
{
    projData.m_files.clear();
    projData.m_Dirs.clear();
    //m_fileData.clear();
    NX::RmFinder finder(100, std::vector<int>({}), L"", L"", L"");
    std::vector<NX::RmProjectFile> files;
    std::wstring parentDir = currentPath;
    if (L'/' != parentDir[parentDir.length() - 1])
        parentDir.append(L"/");
    bool bcancel = false;
    long long lastModifiedTime;
    auto res = theApp.m_pSession->ProjectGetFirstPageFiles(finder, &bcancel, projData.m_nID, parentDir, NULL, NULL, &lastModifiedTime, files);
    if (!res) {
        OutputError(res, L"Failed to display the first page of the list of files");
    }
    else {
        
        SetCurrentProjInfo(projData.m_nID, parentDir, lastModifiedTime);
        //NX::time::datetime dt = NX::time::datetime::from_unix_time(m_lastModifiedTime);
        //auto timeStr = GetFileTimeString(dt);
        //TRACE(L"set last modify tiem = %s\n", timeStr.GetBuffer());

        do {

            std::for_each(files.cbegin(), files.cend(), [&](const NX::RmProjectFile& file) {
                if (file.IsFolder())
                {
                    projData.m_Dirs.push_back(file);
                }
                else
                {
                    projData.m_files.push_back(file);
                }

            });

        } while (res = theApp.m_pSession->ProjectGetNextPageFiles(finder, &bcancel, projData.m_nID, parentDir, files));

    }

}

void CnxrmRMCApp::CenterHomePage(void)
{
	RECT wRect;
	if (GetWindowRect(m_mainhWnd, &wRect)) {
		CRect homePgRect;
		theApp.m_homePage.GetWindowRect(&homePgRect);
		theApp.m_homePage.ScreenToClient(&homePgRect);
		theApp.m_homePage.MoveWindow((wRect.right - wRect.left - homePgRect.Width()) / 2, homePgRect.top, homePgRect.Width(), homePgRect.Height(), TRUE);
	}
}


void CnxrmRMCApp::MessageYield()
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
          TranslateMessage(&msg);
           DispatchMessage(&msg);
    }
}