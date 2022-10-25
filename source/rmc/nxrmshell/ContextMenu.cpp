#include "stdafx.h"
#include "ContextMenu.h"
#include "resource.h"
#include <winternl.h>
#include "nxrmshellglobal.h"
#include "nxlrights.h"
#include "UIConnector.h"
#include "nxrmshell.h"
#include "commondlg.hpp"
#include <nx\winutil\path.h>

#ifdef __cplusplus
extern "C" {
#endif

	extern SHELL_GLOBAL_DATA Global;

	extern 	BOOL init_rm_section_safe(void);

#ifdef __cplusplus
}
#endif


WCHAR Default_MenuCommandNameW[MAX_MENU_ITEM][Max_Menu_Text_Len]; //= { L"Launch SkyDRM"/*IDS_LAUNCH_SKYDRM*/, L"Protect...", L"Remove Protection...",L"View", L"Share...",	L"Properties..." };
const int MenuIDs[MAX_MENU_ITEM] = {
    IDS_LAUNCH_SKYDRM,
    IDS_PROTECT,
    IDS_REMOVE_PROTECTION,
    IDS_VIEW,
    IDS_SHARE,
    IDS_PROPERTIES
};

#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)

typedef NTSTATUS (WINAPI *ZWQUERYEAFILE) (
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_Out_writes_bytes_(Length) PVOID Buffer,
	_In_ ULONG Length,
	_In_ BOOLEAN ReturnSingleEntry,
	_In_reads_bytes_opt_(EaListLength) PVOID EaList,
	_In_ ULONG EaListLength,
	_In_opt_ PULONG EaIndex,
	_In_ BOOLEAN RestartScan
	);

typedef NTSTATUS (WINAPI *ZWSETEAFILE) (
	_In_ HANDLE FileHandle,
	_Out_ PIO_STATUS_BLOCK IoStatusBlock,
	_In_reads_bytes_(Length) PVOID Buffer,
	_In_ ULONG Length
	);


//// {B4B506CC-112F-4739-B93B-F4F3869B6E3E}
extern "C" const GUID CLSID_IRmCtxMenu =	{0xb4b506cc, 0x112f, 0x4739, {0xb9, 0x3b, 0xf4, 0xf3, 0x86, 0x9b, 0x6e, 0x3e}};

static const WCHAR MenuGroupName[] = L"NextLabs"; 

#define MESSAGE_CLIPBOARD_READY	0
#define MESSAGE_SHARE_ERROR		1
#define MESSAGE_PROTECT_ERROR	2
#define MESSAGE_UNPROTECT_ERROR	3

static BOOL get_classify_ui(WCHAR *FileName, WCHAR *GroupName);
static BOOL IsRemoteEvaluation(const WCHAR *FileName);

static HBITMAP BitmapFromIcon(HICON hIcon)
{
	ICONINFO IconInfo = {0};
	HBITMAP hBitmap = NULL;

	do 
	{
		if (!hIcon)
		{
			break;
		}

		if (!GetIconInfo(hIcon, &IconInfo))
		{
			break;
		}

		hBitmap = (HBITMAP)CopyImage(IconInfo.hbmColor, IMAGE_BITMAP, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_CREATEDIBSECTION);

	} while (FALSE);

	return hBitmap;
}

BOOL GetServiceRunningInfo(std::wstring &memberId, std::wstring &installpath, std::wstring &username, std::wstring &email)
{
	BOOL bret = FALSE;
	try {
		std::wstring servresp;
		std::wstring servreq;

	}
	catch (const std::exception  & e) {
		UNREFERENCED_PARAMETER(e);
	}
	
	return bret;
}

#define InitialMenuIndex() for(int i = 0; i < MAX_MENU_ITEM; i++)	m_iMenuIndex[i] = INVALID_MENU_INDEX;

int GetCurrentDPI()
{
	int nDPI = 0;
	HDC hdc = GetDC(0);

	nDPI = GetDeviceCaps(hdc, LOGPIXELSY);

	ReleaseDC(0, hdc);
	return nDPI;
}


IRmCtxMenu::IRmCtxMenu()
	: m_uRefCount(1),
	m_ullrights(0),
	m_lastPolicySN(0),
	m_bIsNXLFile(FALSE),
	m_ulNXLFileVersion(0),
	m_regCtxMenuFilter(std::tr1::wregex(L".*", std::tr1::regex_constants::icase))
{
	wchar_t InstallPath[MAX_PATH];

	int nWidth = 16;
	int nDPI = GetCurrentDPI();
	if(nDPI >= 192)
	{
        nWidth = 32;
	}
	else if (nDPI >= 168)
	{
        nWidth = 28;
	}
	else if (nDPI == 144)
	{
        nWidth = 24;
	} 
	else if (nDPI == 120)
	{
        nWidth = 20;
	}
	else 
	{ // 96 DPI
        nWidth = 16;
	}
	m_hMainIcon = ((HICON)LoadImage(Global.hModule, MAKEINTRESOURCEW(IDI_ICON_MENU), IMAGE_ICON, nWidth, nWidth, 0));
	m_hMainBitmap = (BitmapFromIcon(m_hMainIcon));


	memset(InstallPath, 0, sizeof(InstallPath));
	GetCurrentDirectory(sizeof(InstallPath), InstallPath);
	std::wstring dll_path = InstallPath;
	dll_path.append(L"\\");
	dll_path.append(NXRMCOREUI_CM_UI_DLL_NAME);
    m_rmuObject.Initialize(dll_path.data());
    m_rmuObject.RmuInitialize();

	m_regBypassedFilter = std::tr1::wregex(BYPASSFILTERREGEX, std::tr1::regex_constants::icase);
	m_lastPolicySN = 0;

	InitialMenuIndex();
	

    // Load resource
    LoadStringResource();
}

IRmCtxMenu::~IRmCtxMenu()
{
	if (m_hMainIcon) {
		DestroyIcon(m_hMainIcon);
		m_hMainIcon = NULL;
	}

	if (m_hMainBitmap) {
		DeleteObject(m_hMainBitmap);
		m_hMainIcon = NULL;
	}

    m_rmuObject.RmuDeinitialize();
    m_rmuObject.Clear();
}

void IRmCtxMenu::LoadStringResource()
{
    WCHAR wzResDll[MAX_PATH+1] = {0};
    std::wstring wsMenuText;

    for (int i=0; i < MAX_MENU_ITEM; i++)
    {//Load Menu Text from resource
        int x = MenuIDs[i];
        LoadString(Global.hModule, x, Default_MenuCommandNameW[i], Max_Menu_Text_Len);
    }

    GetModuleFileNameW(Global.hModule, wzResDll, MAX_PATH);
    WCHAR* pos = wcsrchr(wzResDll, L'\\');
    if(pos == NULL) {
        SetDefaultStringResource();
        return;
    }

    *(pos+1) = L'\0';
    wcsncat_s(wzResDll, MAX_PATH, L"nxrmres.dll", _TRUNCATE);

    HMODULE hRes = ::LoadLibraryW(wzResDll);
    if(NULL == hRes) {
        SetDefaultStringResource();
        return;
    }

	//Add all menu items into Text vector


    ::FreeLibrary(hRes);
    hRes = NULL;
}

void IRmCtxMenu::SetDefaultStringResource()
{
	//Add all menu items into Text vector
    // Unicode
	m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdLaunchNxrmc]);
	m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdProtect]);
    m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdUnprotect]);
	m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdView]);
	m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdShare]);
	m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdProperties]);
 //   m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdHelp]);
	//m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdShareEmail]);
	//m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdShareClipboard]);
	//m_MenuTextW.push_back(Default_MenuCommandNameW[MenuCommand::CmdShareMenu]);
	// ANSI
 //   m_MenuTextA.push_back(Default_MenuCommandNameA[MenuCommand::CmdProtect]);
 //   m_MenuTextA.push_back(Default_MenuCommandNameA[MenuCommand::CmdUnprotect]);
	//m_MenuTextA.push_back(Default_MenuCommandNameA[MenuCommand::CmdView]);
	//m_MenuTextA.push_back(Default_MenuCommandNameA[MenuCommand::CmdShare]);
 //   m_MenuTextA.push_back(Default_MenuCommandNameA[MenuCommand::CmdProperties]);
	//m_MenuTextA.push_back(Default_MenuCommandNameA[MenuCommand::CmdHelp]);
	//m_MenuTextA.push_back(Default_MenuCommandNameA[MenuCommand::CmdShareEmail]);
	//m_MenuTextA.push_back(Default_MenuCommandNameA[MenuCommand::CmdShareClipboard]);
	//m_MenuTextA.push_back(Default_MenuCommandNameA[MenuCommand::CmdShareMenu]);
}

STDMETHODIMP IRmCtxMenu::QueryInterface(REFIID riid, void **ppobj)
{
	HRESULT hRet = S_OK;

	void *punk = NULL;

	*ppobj = NULL;

	do 
	{
		if(IID_IUnknown == riid || IID_IShellExtInit == riid) 
		{
			punk = (IShellExtInit *)this;
		}
		else if (IID_IContextMenu == riid)
		{
			punk = (IContextMenu*)this;
		}
		else
		{
			hRet = E_NOINTERFACE;
			break;
		}

		AddRef();

		*ppobj = punk;

	} while (FALSE);

	return hRet;
}

STDMETHODIMP_(ULONG) IRmCtxMenu::AddRef()
{
	m_uRefCount++;

	return m_uRefCount;
}

STDMETHODIMP_(ULONG) IRmCtxMenu::Release()
{
	ULONG uCount = 0;

	if(m_uRefCount)
		m_uRefCount--;

	uCount = m_uRefCount;

	if(!uCount)
	{
		delete this;
		InterlockedDecrement(&Global.ContextMenuInstanceCount);
	}

	return uCount;
}

STDMETHODIMP IRmCtxMenu::GetCommandString(_In_ UINT_PTR idCmd, _In_ UINT uType, _Reserved_ UINT *pReserved, _Out_writes_bytes_((uType & GCS_UNICODE) ? (cchMax * sizeof(wchar_t)) : cchMax) _When_(!(uType & (GCS_VALIDATEA | GCS_VALIDATEW)), _Null_terminated_) CHAR *pszName, _In_ UINT cchMax)
{
	HRESULT hr = S_OK;

	do 
	{
		if (uType & GCS_HELPTEXT)
		{
			if (idCmd >= MAX_MENU_ITEM)
			{
				hr = E_INVALIDARG;
				break;
			}

			if (uType & GCS_UNICODE)
			{
                wcsncpy_s((LPWCH)pszName, cchMax, m_MenuTextW[idCmd].c_str(), _TRUNCATE);
			}
			else
			{
                strncpy_s(pszName, cchMax, m_MenuTextA[idCmd].c_str(), _TRUNCATE);
			}
		}
		else
		{
			hr = E_INVALIDARG;
		}

	} while (FALSE);

	return hr;
}

HRESULT IRmCtxMenu::OnShare(_In_ const unsigned int type, _Out_ std::wstring & nxlfile, _Out_ std::vector<std::pair<std::wstring, std::wstring>>& tags)
{
	WCHAR ClassifyUIXMLFileName[MAX_PATH] = { 0 };
	WCHAR GroupName[MAX_PATH] = { 0 };
	std::wstring setting;
	std::wstring filepolicy;
	HRESULT hresult = E_UNEXPECTED;

	assert(!m_bIsNXLFile || m_ulNXLFileVersion >= 0x00030000);

	switch (type) {
		case DIALOGSHARE_TYPE:
			break;
		case DIALOGPROTECT_TYPE:
			break;
		default:
			assert(true);
			return E_NOINTERFACE;
	}

	if (S_OK != m_rmuObject.RmuShowShareDialog(GetActiveWindow(), m_strSelectedFile.c_str(), filepolicy.data(), GetUserDefaultLangID(), tags, setting)) {
		return E_ABORT; //user canceled
	}

	std::wstring jpolicy;
	//check the return setting is correct

	//call sharing API
	if (DIALOGPROTECT_TYPE == type) {
	}
	else {//Share dialog
	}

	if (m_bIsNXLFile && m_ulNXLFileVersion >= 0x00030000) {
		//file already encrypted. it is ready to share.
	}
	else {
		//pass the request to UI for sharing
	}

	//append .nxl extension to target file.
	nxlfile += NXL_FILE_EXTENTIONW;

	return hresult;
}

void IRmCtxMenu::OnProtect()
{
	std::wstring nxlfile;
	std::vector<std::pair<std::wstring, std::wstring>> tags;

	HRESULT hresult = OnShare(DIALOGPROTECT_TYPE, nxlfile, tags);
	std::wstring msg;

	if (E_ABORT == hresult)//user canceled sharing
		return;

	if (S_OK != hresult)
	{
		msg = m_NotifyMessage[MESSAGE_PROTECT_ERROR];
//		NX::utility::Replaceall_string(msg, std::wstring(L"%e"), NX::conversion::to_wstring(hresult));
		if (!msg.empty()) {//show message to user
			CUIConnector ui;
			ui.SendNotify(msg);
		}
	}
	return;
}

void IRmCtxMenu::OnUnprotect()
{
    assert(BUILTIN_RIGHT_DECRYPT == (m_ullrights & BUILTIN_RIGHT_DECRYPT));
    assert(!m_strSelectedFile.empty());

	//TODO
	//pass the file to UI for decrypted file
	HRESULT hr = E_FAIL;
    if (SUCCEEDED(hr)) {
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATH, m_strSelectedFile.c_str(), NULL);
	}
	else {
		std::wstring	msg = m_NotifyMessage[MESSAGE_UNPROTECT_ERROR];
//		NX::fs::dos_filepath fpath(m_strSelectedFile);
//		NX::utility::Replaceall_string(msg, std::wstring(L"%s"), fpath.file_name().full_name());

		CUIConnector ui;
		ui.SendNotify(msg);
	}
}

void IRmCtxMenu::OnCheckPermission()
{
    m_rmuObject.RmuShowPermissionPropPage(GetActiveWindow(), m_strSelectedFile.c_str(), m_ullrights, NULL);
}

void IRmCtxMenu::OnClassify()
{
    WCHAR ClassifyUIXMLFileName[MAX_PATH] = { 0 };
    WCHAR GroupName[MAX_PATH] = { 0 };
    HANDLE h = INVALID_HANDLE_VALUE;


}

void IRmCtxMenu::OnHelp()
{
    return;
}




std::wstring GetNxrmcPath()
{
	const int MAXPATH = 2048;
	TCHAR szEXEPath[MAXPATH];
	GetModuleFileName(Global.hModule, szEXEPath, MAXPATH);
	WCHAR* pos = wcsrchr(szEXEPath, L'\\');
	*(pos + 1) = L'\0';
	wcsncat_s(szEXEPath, MAXPATH, L"nxrmc.exe", _TRUNCATE);
	return move(std::wstring(szEXEPath));
}


COPYDATASTRUCT g_copyData = { 0 };

LRESULT IRmCtxMenu::SendMenuMessageToClient(int cmdMsg)
{
	HWND hwnd = GetNxrmchWnd();
	HWND hActiveWnd = GetActiveWindow();
	g_copyData.dwData = NxMsgMap.at(cmdMsg);
	g_copyData.cbData = (DWORD)(m_strSelectedFile.size() + 1) * 2;
	g_copyData.lpData = (PVOID)m_strSelectedFile.c_str();
	return SendMessage(hwnd, WM_COPYDATA, (WPARAM)hActiveWnd, (LPARAM)&g_copyData);

}

STDMETHODIMP IRmCtxMenu::InvokeCommand(_In_  CMINVOKECOMMANDINFO *pici)
{
    HRESULT hr = S_OK;

    // Verb is a string? ignore this
    if (0 != HIWORD(pici->lpVerb)) {
        return E_INVALIDARG;
    }

    assert(!m_strSelectedFile.empty());

    if (m_strSelectedFile.empty()) {
        return E_INVALIDARG;
    }

	do 
	{	
		auto menuMsg = m_iMenuIndex[LOWORD(pici->lpVerb)];
		switch (menuMsg)
		{
		case CmdLaunchNxrmc:
			ShellExecute(NULL, L"open", GetNxrmcPath().c_str(), L"", NULL, SW_SHOW);
			break;

		case CmdProtect:
		{
			SendMenuMessageToClient(menuMsg);
			break;
		}
		case CmdUnprotect:
		{
			SendMenuMessageToClient(menuMsg);
			break;
		}
		case CmdView:
			SendMenuMessageToClient(menuMsg);
			break;
		case CmdShare:
		{
			SendMenuMessageToClient(menuMsg);
			break;
		}
		break;
		case CmdProperties:
			SendMenuMessageToClient(menuMsg);
			break;

		case INVALID_MENU_INDEX:
			assert(L"Error: Invalid Menu Index!");
			//go default routine
		default:
			hr = E_INVALIDARG;
			break;
		}

	} while (FALSE);

	return hr;
}

STDMETHODIMP IRmCtxMenu::QueryContextMenu(_In_  HMENU hmenu, _In_  UINT indexMenu, _In_  UINT idCmdFirst, _In_  UINT idCmdLast, _In_  UINT uFlags)
{
	HRESULT hr = S_OK;

	HMENU   hSubMenu = NULL;
	HMENU	hShareSubMenu = NULL;
	BOOL    bFolder  = FALSE;
	UINT    uSubMenuFlags = 0;
	DWORD	dwAttrs = 0;
	UINT	uMenuInd = 0; //The total number of added menus
	UINT	uMenuPos = 0; //The Position of added menu

    // If Flags contains CMF_DEFAULTONLY, ignore it
    if ((uFlags & CMF_DEFAULTONLY) || m_strSelectedFile.empty()) {
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
    }


	do 
    {
		bool brights = true;
			
		// Insert START separator
		InsertMenuW(hmenu, indexMenu++, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);

		// Create/Insert Sub Menus
		hSubMenu = CreateMenu();
		if (NULL == hSubMenu)  {
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}
		hShareSubMenu = CreateMenu();
		if (NULL == hShareSubMenu) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}


		//
		// Add Sub menus below. 
		// Increase uMenuInd for menu item which need be handled
		// Increase uMenuPos for every menu item including MF_SEPARATOR etc.
		//
		InitialMenuIndex();

		if (!GetNxrmchWnd())
		{ //Only view nxl file
			uSubMenuFlags = MF_STRING | MF_BYPOSITION;
			InsertMenuW(hSubMenu, uMenuPos, uSubMenuFlags, idCmdFirst + uMenuInd, m_MenuTextW[CmdLaunchNxrmc].c_str());
			SetMenuItemBitmaps(hSubMenu, 0, MF_BYPOSITION, NULL, NULL); // m_bmpUser, m_bmpLock);
			m_iMenuIndex[uMenuInd] = CmdLaunchNxrmc;
			uMenuInd++;
			uMenuPos++;
			brights = false;
		}

		// Insert sub menu item: Protect ...
		uSubMenuFlags = MF_STRING | MF_BYPOSITION;
		uSubMenuFlags |= brights ? 0 : MF_GRAYED;
		if (!m_bIsNXLFile) {
			InsertMenuW(hSubMenu, uMenuPos, uSubMenuFlags, idCmdFirst + uMenuInd, m_MenuTextW[CmdProtect].c_str());
			SetMenuItemBitmaps(hSubMenu, uMenuPos, MF_BYPOSITION, NULL, NULL); // m_bmpUser, m_bmpLock);
			m_iMenuIndex[uMenuInd] = CmdProtect;
			uMenuInd++;
			uMenuPos++;

		}
		//remove unprotect menu from this release
// 		else
// 		{
// 			InsertMenuW(hSubMenu, uMenuPos, uSubMenuFlags, idCmdFirst + uMenuInd, m_MenuTextW[CmdUnprotect].c_str());
// 			SetMenuItemBitmaps(hSubMenu, uMenuPos, MF_BYPOSITION, NULL, NULL); // m_bmpUser, m_bmpLock);
// 			m_iMenuIndex[uMenuInd] = CmdUnprotect;
// 			uMenuInd++;
// 			uMenuPos++;
// 		}

		{//insert share submenu
			if (m_bIsNXLFile)
			{ //Only view nxl file
				uSubMenuFlags = MF_STRING | MF_BYPOSITION;
				uSubMenuFlags |= brights ? 0 : MF_GRAYED;
				InsertMenuW(hSubMenu, uMenuPos, uSubMenuFlags, idCmdFirst + uMenuInd, m_MenuTextW[CmdView].c_str());
				SetMenuItemBitmaps(hSubMenu, 0, MF_BYPOSITION, NULL, NULL); // m_bmpUser, m_bmpLock);
				m_iMenuIndex[uMenuInd] = CmdView;
				uMenuInd++;
				uMenuPos++;
			}

			// Insert sub menu item: Share with Email...
			uSubMenuFlags = MF_STRING | MF_BYPOSITION;
			uSubMenuFlags |= brights ? 0 : MF_GRAYED;
			InsertMenuW(hSubMenu, uMenuPos, uSubMenuFlags, idCmdFirst + uMenuInd, m_MenuTextW[CmdShare].c_str());
			SetMenuItemBitmaps(hSubMenu, 0, MF_BYPOSITION, NULL, NULL); // m_bmpUser, m_bmpLock);
			m_iMenuIndex[uMenuInd] = CmdShare;
			uMenuInd++;
			uMenuPos++;

			//if (m_bIsNXLFile)
			//{ //Only view nxl file
			//	uSubMenuFlags = MF_STRING | MF_BYPOSITION;
			//	uSubMenuFlags |= brights ? 0 : MF_GRAYED;
			//	InsertMenuW(hSubMenu, uMenuPos, uSubMenuFlags, idCmdFirst + uMenuInd, m_MenuTextW[CmdProperties].c_str());
			//	SetMenuItemBitmaps(hSubMenu, 1, MF_BYPOSITION, NULL, NULL); // m_bmpUser, m_bmpLock);
			//	m_iMenuIndex[uMenuInd] = CmdProperties;
			//	uMenuInd++;
			//	uMenuPos++;
			//}
		}

		// Insert Sub Menus
		InsertMenuW(hmenu, indexMenu, MF_STRING | MF_POPUP | MF_BYPOSITION, (UINT_PTR)hSubMenu, MenuGroupName);
        SetMenuItemBitmaps(hmenu, indexMenu, MF_BYPOSITION, m_hMainBitmap, m_hMainBitmap);

		// Finally, let Windows Explorer know how many menu items have been added. Count all parent/child menu
		hr = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, uMenuInd + 2);

	} while (FALSE);

	return hr;
}

std::vector<std::wstring> IRmCtxMenu::query_selected_file(IDataObject *pdtobj)
{
    std::vector<std::wstring> files;
    FORMATETC   FmtEtc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM   Stg = { 0 };
    HDROP       hDrop = NULL;

    memset(&Stg, 0, sizeof(Stg));
    Stg.tymed = CF_HDROP;

    // Find CF_HDROP data in pDataObj
    if (FAILED(pdtobj->GetData(&FmtEtc, &Stg))) {
        return files;
    }

    // Get the pointer pointing to real data
    hDrop = (HDROP)GlobalLock(Stg.hGlobal);
    if (NULL == hDrop) {
        return files;
    }

    // How many files are selected?
    const int nFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
    if (0 == nFiles) {
        return files;
    }

    for (int i = 0; i < nFiles; i++) {
		wchar_t s[MAX_PATH];
        if (0 != DragQueryFileW(hDrop, i, s, MAX_PATH)) {
			//push all files in MenuFilter will be checked later
            files.push_back(s);
        }
    }

    return files;
}


HWND GetNxrmchWnd()
{
	auto hMainWnd = FindWindow(L"NXSD-NextLabsSkyDRM", L"SkyDRM");
	return hMainWnd;
}
//check the selected file (support one file only) and make sure the extension is correct
//check if the file is in exclude filter or in client installation directory.
//assumption: the xnl file passing in should has no .nxl extension.
STDMETHODIMP IRmCtxMenu::Initialize(_In_opt_  PCIDLIST_ABSOLUTE pidlFolder, _In_opt_  IDataObject *pdtobj, _In_opt_  HKEY hkeyProgID)
{
	HRESULT hr = S_OK;
    
	do 
	{
        std::vector<std::wstring> selected_files;


        selected_files = query_selected_file(pdtobj);
        if (selected_files.size() != 1) {
            break;
        }

		//bypass directory
		DWORD dwAttrs = GetFileAttributesW(m_strSelectedFile.c_str());
		if (FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & dwAttrs) && dwAttrs != INVALID_FILE_ATTRIBUTES) {
			m_strSelectedFile.clear();
			break;
		}

		m_bIsNXLFile = FALSE;
		m_ulNXLFileVersion = 0;
        m_strSelectedFile = selected_files[0];

		//check if it is a valid file
		//TODO
// 		NX::fs::dos_filepath file_path(m_strSelectedFile);
// 		if (file_path.empty() || !NX::fs::is_dos_path(file_path.path())) {
// 			assert(true);//should not hit at any condition
// 			m_strSelectedFile.clear();
// 			break;
// 		}

		std::wstring retstr;
		std::wstring installpath;
		std::wstring username;
		std::wstring email;

		wchar_t CurrentDir[MAX_PATH];

		BOOL is_service_active = true;

		GetCurrentDirectory(MAX_PATH, CurrentDir);
		installpath = CurrentDir;
		if (!is_service_active) {
			m_strSelectedFile.clear();
			break;
		}
		if (0 == m_strSelectedFile.find(installpath.data())) {
			//ignore the file under installation folder
			m_strSelectedFile.clear();
			break;
		}

		m_strMembershipID = retstr;
		m_strUserName = username;
		m_strUserEmail = email;


		//check if it is a NXL file
		//TODO


		//check the extison at last after append .nxl extension.
		LoadCtxMenuRegExFilter();
		// check extension bypass filter and Menu filter
		if (std::regex_match(m_strSelectedFile, m_regBypassedFilter) || (!std::regex_match(m_strSelectedFile, m_regCtxMenuFilter))) {
			m_strSelectedFile.clear();
			break;
		}

		auto pos = m_strSelectedFile.find_last_of(L".");
		if (pos != -1)
		{
			if (!lstrcmpi(L".nxl", m_strSelectedFile.substr(pos, 4).c_str()))
			{
				m_bIsNXLFile = TRUE;
			}
		}

	} while (FALSE);

	return S_OK;
}


void IRmCtxMenu::LoadCtxMenuRegExFilter()
{

	ULONG PolicySN = 0;
	wchar_t CtxMenuRegEx[1024];
	memset(CtxMenuRegEx, 0, sizeof(CtxMenuRegEx));

	do 
	{
		//Todo query file filter string from policy
		try {
            if (L'\0' == CtxMenuRegEx[0]) {
                m_regCtxMenuFilter = std::tr1::wregex(L".*", std::tr1::regex_constants::icase);
            }
            else {
                m_regCtxMenuFilter = std::tr1::wregex(CtxMenuRegEx, std::tr1::regex_constants::icase);
            }
		}
		catch (const std::regex_error) {
            ;
		}

		m_lastPolicySN = PolicySN;

	} while (FALSE);

	return;
}

