#pragma once

#include <string>
#include <vector>
#include <regex>
#include "NxMsgIDs.h"
#include <map>

#ifndef CACHE_LINE
#define CACHE_LINE	64
#endif

#ifndef CACHE_ALIGN
#define CACHE_ALIGN	__declspec(align(CACHE_LINE))
#endif


#define NXRMCOREUI_CM_UI_DLL_NAME			L"nxrmcmui.dll"
#define NXRMSHELL_DLL_NAME					L"nxrmshell.dll"

#define BYPASSFILTERREGEX					L"^[c-zC-Z]{1}:\\\\windows\\\\.*|.*\\.exe$|.*\\.dll$|.*\\.ttf$|.*\\.zip$|.*\\.rar"



#pragma pack(push, 4)

typedef struct _FILE_GET_EA_INFORMATION {
	ULONG NextEntryOffset;
	UCHAR EaNameLength;
	CHAR EaName[1];
} FILE_GET_EA_INFORMATION, *PFILE_GET_EA_INFORMATION;

typedef struct _FILE_FULL_EA_INFORMATION {
	ULONG NextEntryOffset;
	UCHAR Flags;
	UCHAR EaNameLength;
	USHORT EaValueLength;
	CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

#pragma pack(pop)

#define DIALOGSHARE_TYPE		0
#define DIALOGPROTECT_TYPE		1

typedef enum _MenuCommand {
    CmdLaunchNxrmc = 0,
    CmdProtect,
    CmdUnprotect,
    CmdView,
    CmdShare,
    CmdProperties,
    CmdMenuCount //for counting purpose. Define all menu ID before it
} MenuCommand;


#define MAX_MENU_ITEM		_MenuCommand::CmdMenuCount
#define INVALID_MENU_INDEX	(-1)
const int Max_Menu_Text_Len = 129;

extern WCHAR Default_MenuCommandNameW[MAX_MENU_ITEM][Max_Menu_Text_Len];
static const char * Default_MenuCommandNameA[MAX_MENU_ITEM] = { "Protect...", "Remove Protection...", "View", "Share...",	"Properties..." };
static const std::map<int, int> NxMsgMap = { 
    { CmdLaunchNxrmc,   NxCmdLaunchNxrmc },
    { CmdProtect,       NxCmdProtect },
    { CmdUnprotect,     NxCmdUnprotect },
    { CmdView,          NxCmdView },
    { CmdShare,         NxCmdShare },
    { CmdProperties,    NxCmdProperties }
};

class IRmCtxMenu : public IContextMenu, public IShellExtInit
{
public:
	IRmCtxMenu();
	~IRmCtxMenu();

	STDMETHODIMP QueryInterface(REFIID riid, void **ppobj);

	STDMETHODIMP_(ULONG) AddRef();

	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP QueryContextMenu(_In_  HMENU hmenu, _In_  UINT indexMenu, _In_  UINT idCmdFirst, _In_  UINT idCmdLast, _In_  UINT uFlags);

	LRESULT SendMenuMessageToClient(int cmdMsg);

	STDMETHODIMP InvokeCommand(_In_  CMINVOKECOMMANDINFO *pici);

	STDMETHODIMP GetCommandString(_In_  UINT_PTR idCmd, _In_  UINT uType, _Reserved_  UINT *pReserved, _Out_writes_bytes_((uType & GCS_UNICODE) ? (cchMax * sizeof(wchar_t)) : cchMax) _When_(!(uType & (GCS_VALIDATEA | GCS_VALIDATEW)), _Null_terminated_)  CHAR *pszName, _In_  UINT cchMax);

	STDMETHODIMP Initialize(_In_opt_  PCIDLIST_ABSOLUTE pidlFolder, _In_opt_  IDataObject *pdtobj, _In_opt_  HKEY hkeyProgID);

protected:
    void LoadStringResource();
    void SetDefaultStringResource();
	void LoadCtxMenuRegExFilter();
	void OpenProtectDlg(std::wstring fileName);
    std::vector<std::wstring> query_selected_file(IDataObject *pdtobj);

    // Process Command
    void OnProtect();
    void OnUnprotect();
    void OnCheckPermission();
    void OnClassify();
    void OnHelp();
	HRESULT OnShare(_In_ const unsigned int type, _Out_ std::wstring & nxlfile, _Out_ std::vector<std::pair<std::wstring, std::wstring>>& tags);

	void PopupMessage(_In_opt_ std::wstring parastr1);

private:
    ULONG					    m_uRefCount;
    HICON					    m_hMainIcon;
    HBITMAP					    m_hMainBitmap;
    std::vector<std::wstring>   m_MenuTextW;
    std::vector<std::string>    m_MenuTextA;
	std::vector<std::wstring>	m_NotifyMessage;

	std::wregex				m_regBypassedFilter;
	std::wregex				m_regCtxMenuFilter;
    std::wstring            m_strCurrentDir;

    std::wstring            m_strSelectedFile;
    ULONG					m_lastPolicySN;
    ULONGLONG               m_ullrights;
	BOOL					m_bIsNXLFile;
	unsigned long			m_ulNXLFileVersion;
	std::wstring			m_strMembershipID;
	std::wstring			m_strUserName;
	std::wstring			m_strUserEmail;

	int						m_iMenuIndex[MAX_MENU_ITEM];
	NX::utility::CRmuObject	m_rmuObject;
};

HWND GetNxrmchWnd();
