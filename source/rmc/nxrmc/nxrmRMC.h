
// nxrmRMC.h : main header file for the PROJECT_NAME application
//

#pragma once
#include "RepoPage.h"
#include "IntroPage.h"
#include "weblogondlg.hpp"
#include "ProjPage.h"
#include "ProjData.h"
#include "ProjectMain.h"
#include "NXHomePage.h"
#include "ActivityLogDlg.h"

#include <nx\rmc\session.h>
#include <nx\common\stringex.h>
#include <nx\common\time.h>
#include <nx\rmc\upgrade.h>

#include <mutex>

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CnxrmRMCApp:
// See nxrmRMC.cpp for the implementation of this class
//
#define NX_NEXTLABS_STANDARD_BLACK      RGB(0, 0, 0)
#define NX_NEXTLABS_STANDARD_WHITE      RGB(255, 255, 255)
#define NX_NEXTLABS_STANDARD_GREEN      RGB(57, 151, 74)
#define NX_NEXTLABS_STANDARD_RED        RGB(235, 87, 87)
#define NX_NEXTLABS_STANDARD_GRAY		RGB(240, 240, 240)	    //UX 242,243,245
#define NX_NEXTLABS_STANDARD_BLUE		RGB(60, 100, 255)		
#define NX_NEXTLABS_PROJ_USER_RED		RGB(239,102,69)		//UX 242,243,245
#define NX_NEXTLABS_STANDARD_GRAY_BORDER		RGB(225, 225, 225)	  




//custom tray message 
#define WM_TRAY             (WM_APP + 199)
#define WM_NX_REFRESH_PROJ_LIST (WM_APP + 299)
#define WM_NX_REFRESH_REPO_LIST (WM_APP + 300)
#define WM_NX_REFRESH_FILE_LIST	(WM_APP + 301)
#define WM_NX_REFRESH_HOME_PAGE     (WM_APP + 302)
#define WM_NX_REFRESh_MYVALUT_LIST	(WM_APP + 303)
#define WM_NX_TIMED_REFRESH_PROJLIST    (WM_APP + 304)
#define WM_NX_TIMED_REFRESH_ONE_PROJ    (WM_APP + 305)
#define WM_NX_UPDATE_WINDOW_SIZE        (WM_APP + 306)
#define WM_SHOWVIEWER             (WM_APP + 399)

//Tray IDni
enum UI_ID
{
	//Tray's ID
	TRAY_ID = 1
};

typedef struct {
    OthersRepoType storType;
    int nIcoSelID;
    int nIcoID;
    int nImgID;
    CString driveName;
    CString emailAddress;
    //DirStruct fileStruct;
}   RepoData;

typedef void (*StatusCallBackFunc)(MSG wmMsg);

enum AddDlgStyle { NX_ADD_BUTTON_HOMEPAGE, NX_ADD_BUTTON_PROJPAGE, NX_VIEW_ALL_PROJ };


void UploadProc(int index, StatusCallBackFunc * func);

#define MAX_LOADSTRING 100

enum MAINUI_DIAGLOGLIST
{
	MD_UNKNOWN = 0,
	MD_INTRODUCTION,
	MD_LOGIN,
	MD_SIGNUP,
	MD_HOMEPAGE,
	MD_REPORITORYHOME,
	MD_PROJECTSHOME,
    MD_PROJECTMAIN,
    MD_PROJECTINTRO
};

class CnxrmRMCApp : public CWinApp
{
public:
    std::wstring GetRouter();
    std::wstring GetTenant();
	void RefreshMyVaultInfo(); //this function will fresh MyDrive usage info and set page to refresh.
	void RefreshHomePageMyDriveUsage();//this function will fresh MyDrive usage info
	void RestAPIRefreshUserInfo();
	void RestAPIRefreshUsage();
	wstring RestAPIGetCurrentUserID();

    CnxrmRMCApp();

    CNXHomePage m_homePage;
    CRepoPage  m_dlgRepoPage;
    CProjPage m_dlgProjPage;
    CProjectMain m_dlgProjMain;
    CIntroPage m_dlgProjIntro;      //Add this for project introduction


	CIntroPage m_dlgIntro;
	CMainDlg *m_plogonDlg;
	HWND m_mainhWnd;
    std::shared_ptr<NX::RmSession> m_pSession;
	WCHAR szTitle[MAX_LOADSTRING];

	NOTIFYICONDATA m_nid;                           // Tray icon

	HMENU	m_hMainMenu;
	BOOL	m_bHideProjectMenu;
	HACCEL  m_hAccel;
	MAINUI_DIAGLOGLIST m_curShowedPage;
    //This is required by home page.
    wstring             m_usrName;
    wstring             m_emailAddress;
    //Usage information
    __int64             m_total = 0;
    __int64             m_used = 0;
    __int64             m_myvaultused = 0;


#define MAX_INTEGER 108000 
SIZE m_maxSize = { MAX_INTEGER,MAX_INTEGER };

public:
	//App utility functions
	std::wstring FormatFileSizeString(__int64 size);
	std::wstring FormatFileTimeString(SYSTEMTIME &st);
	std::wstring GetFileExtension(std::wstring filename, BOOL *bIsNXL=NULL);

	int GetFileExtensionImageIndex(std::wstring fileext, BOOL bEncrypt);
	int GetFolderImageIndex(void) { return m_fileImageList.GetImageCount()-2; }
	int GetUpFolderImageIndex(void) { return m_fileImageList.GetImageCount() - 1; }
	CImageList * GetFileExtensionImageList(void) { return &m_fileImageList; }
protected:
	ULONG_PTR m_gdiplusToken;
	CImageList m_fileImageList;

	ULONGLONG StringRightsToUll(const NXSharingData &dataOut);
    std::vector<std::wstring> StringRightsToVector(const NXSharingData &dataOut);

	RepositoryFileInfo SetFileInfo(const NX::RmRepoFile &repoFile);
	MyVaultFileInfo SetFileInfo(const NX::RmMyVaultFile &mvFile);

	BOOL CreateIconList(void);

	std::wstring m_strArgOpenFileName;//the is the name passed in.

protected: //data from Restful API
	std::mutex	m_mutRepoData;
	std::vector<NX::RmRepository> m_RepositoryList;
	int GetRepositoryIndex(const std::wstring &repoid);
	BOOL UpdateRepository(std::vector<NX::RmRepository> &repolist);
	NX::RmUpgradeInfo m_productUpgradeInfo;

	std::mutex m_mutInvitation;
	std::vector<NX::RmProjectInvitation> m_InvatationList;
	std::mutex m_mutProject;
	std::vector<NX::RmProject> m_AllProjects;
    std::vector<NX::RmProjectFile> m_recentFiles;
    vector<NX::RmProjectMember> m_recentMembers;
    std::wstring m_curProjID;
	size_t m_ProjectByMeCount;
	size_t m_ProjectByOthersCount;

	size_t m_TotalProjectByMe; //total project numbers return from server
	size_t m_TotalProjectByOthers; //total project members return from server

	wstring m_currFolderId;
	long long m_lastModifiedTime;
public:
	//Repository List related functions
	BOOL RefreshRepositoryList(BOOL bRefresh = TRUE);
	BOOL GetRepositoryInfo(int index, RepositoryInfo & repo);
	BOOL RenameRepository(std::wstring orgName, std::wstring newName);
	BOOL RemoveRepository(std::wstring orgName);
	size_t GetRepositoryArray(std::vector<RepositoryInfo> & repoArr, BOOL bRefresh = FALSE);//return total repository numbers
	size_t GetRepositoryCount(void) { lock_guard<mutex> lock(m_mutRepoData); return m_RepositoryList.size(); }

	size_t GetInvitationCount(void) { lock_guard<mutex> lock(m_mutInvitation); return m_InvatationList.size(); }
	BOOL GetInvitationInfo(int index, CProjData &data);
	size_t GetInvitationArray(std::vector<CProjData> & inviteArr);

	size_t GetProjectByMeCount(void) {	lock_guard<mutex> lock(m_mutProject); return m_ProjectByMeCount;}
	size_t GetProjectByOthersCount(void) { lock_guard<mutex> lock(m_mutProject); return m_ProjectByOthersCount; }

	size_t GetTotalProjectbyMeCount(void) { return m_TotalProjectByMe; }//Note when windows initialized, TotalProjectByMe may not be same as ProjectByMe count
	size_t GetTotalProjectByOthersCount(void) { return m_TotalProjectByOthers; }//Note when windows initialized, TotalProjectByMe may not be same as ProjectByMe count

	BOOL GetProjectByMeInfo(int index, CProjData &data);
	BOOL GetProjectByOthersInfo(int index, CProjData &data);
    CProjData AssignProjData(NX::RmProject &project);
	size_t GetProjectByMeArray(std::vector<CProjData> &projArr);
	size_t GetProjectByOthersArray(std::vector<CProjData> &projArr);
private:
	//thread for synchronize the file list, repository from RMS
	HANDLE	m_thrdRepoList;
	HANDLE	m_thrdMyDriveFile;
	std::mutex m_mutMyDriveFiles;
	__int64 m_MyDriveLastQueryTime;
	std::wstring m_strMyDrivePath;



	HANDLE	m_thrdInviteList;
	HANDLE	m_thrdProjectList;

	BOOL	m_bExitThread;	//flag for all synchronize thread to exit.
	BOOL	m_bPauseThread;	//flag for all synchronize thread to pause

    HANDLE m_hSingleInstanceMutex;

	BOOL RefreshMyDriveFileList(void);
//	void RefreshMyVaultFileList(void);

	void SetCurrentProjInfo(wstring projID, wstring currFilePathId, long long lastModifiedTime);

	bool InitializeProjData(void);
	
	BOOL RefreshInvatationList(void);
	void StopRunningThread(HANDLE h);
public:
	friend DWORD CALLBACK SyncRepoList(_In_opt_ PVOID lpData);
	friend DWORD CALLBACK SyncInvitationList(_In_opt_ PVOID lpData);
	friend DWORD CALLBACK SyncProjectList(_In_opt_ PVOID lpData);
	friend DWORD CALLBACK SyncMyDriveFile(_In_opt_ PVOID lpData);
	friend DWORD CALLBACK UploadFileProc(_In_opt_ PVOID lpData);
	friend DWORD CALLBACK DownloadMyValutFileListProc(_In_opt_ PVOID lpData);
	friend DWORD CALLBACK DownloadMyVaultFirstPageProc(_In_opt_ PVOID lpData);
	friend DWORD CALLBACK DownloadMyVaultNextPageProc(_In_opt_ PVOID lpData);
	friend DWORD CALLBACK DownloadRepFileProc(_In_opt_ PVOID lpData);
	friend void DownloadPackageProc(BOOL bSilent, StatusCallBackFunc func /* = NULL */);
public:
	// Overrides

	//MyVault and third party repository functions
	BOOL GetMyValutFileList(std::wstring keyword, vector<MyVaultFileInfo> *pfilelist, StatusCallBackFunc func = NULL, MSG * message = NULL);
	BOOL GetMyValutFirstPage(NX::RmFinder * pfinder, vector<MyVaultFileInfo> *pfilelist, StatusCallBackFunc func = NULL, MSG * message = NULL);
	BOOL GetMyVaultMoreData(NX::RmFinder * pfinder, vector<MyVaultFileInfo> *pfilelist, StatusCallBackFunc func = NULL, MSG * message = NULL);//return True when success

	BOOL DeleteMyValutFile(MyVaultFileInfo & fileinfo);
    BOOL RefreshHomePage();
	BOOL DownloadMyValutFile(RepositoryFileInfo &fileinfo, std::wstring localpath);
	BOOL GetRepositoryFileList(int index, std::wstring path, vector<RepositoryFileInfo> & filelist, std::wstring keyword = L"", BOOL brefresh = FALSE);
	BOOL DeleteRepositoryFile(int index, std::wstring path, std::wstring filename);
	BOOL GetRepoNXLFileUid(int index, std::wstring path, std::wstring& uid);
	BOOL DownloadRepositoryFile(int index, std::wstring path, std::wstring localpath, BOOL bnxl);
    bool GetProjectInfo(const std::wstring & projectId, CProjData & project);
    bool SetProjectInfoInData(CProjData & proj);
    BOOL UploadFileToRepository(HWND hparentwnd, int index, std::wstring path, std::wstring Name, BOOL bProtectAfter = FALSE, StatusCallBackFunc func = NULL, MSG * message = NULL);
    BOOL UploadFileToProject(HWND hparentwnd, const wstring & projID, std::wstring destDir, std::wstring path, std::wstring Name, StatusCallBackFunc func, MSG * message);
	BOOL CreateRepositoryFolder(int index, std::wstring path, std::wstring newFolderName);
	BOOL DeleteRepositoryFolder(int index, std::wstring path, std::wstring foldername);

	BOOL GetFileActivityLog(std::wstring filename, std::wstring uid, vector<FileActivity> & fileactivities);

	void SetMyDriveCurrentPath(std::wstring path);
    //Project related functions
    bool AddProjecFile(const HWND hWnd, const CProjData & projData, const wstring fullPath, const wstring fileName);
	BOOL RefreshProjectList(void);
	size_t GetProjectCount(void) { lock_guard<mutex> lock(m_mutProject); return m_ProjectByMeCount + m_ProjectByOthersCount; }
    bool CreateProject(NXProjData & projData, vector<wstring>& addrArr, AddDlgStyle dlgStyle, wstring & projID);
    bool FindProj(const wstring projID, CProjData * pProjData);
    bool AddNewProject(HWND hwnd);
    bool ProjectAcceptInvitation(const std::wstring & id, const std::wstring & code, std::wstring & projectId);
    bool ProjectDeclineInvitation(const std::wstring & id, const std::wstring & code, const std::wstring & reason);
    bool ProjectFindPendingInvitations(const std::wstring & projectId, const wstring & keyword, std::vector<NX::RmProjectInvitation>& invites);
    bool ProjectFindPendingMembers(const CProjData & projData, const wstring & keyword, vector<ProjMember>& members);
    //bool ProjectFindPendingMembers(const std::wstring & projectId, const wstring & keyword, std::vector<ProjMember>& members);
    //bool FindProjectMembers(const wstring & projID, const wstring & keyWord, const wstring ownerId, vector<ProjMember>& members);
    bool FindProjectMembers(const CProjData & projData, const wstring & keyWord, const wstring ownerId, vector<ProjMember>& members);
    bool InvitePeopleToProject(const CProjData & curProjData);

    BOOL CreateProjectFolder(const wstring & projID, const std::wstring path, const std::wstring & newfolder);
    BOOL DeleteProjectFile(const wstring & projID, const std::wstring & path);

    void DownloadProjectFile(const wstring & projID, const wstring source, wstring targetFolder);

    void ShowProjectFileProperty(const wstring & projID, const ProjFileData & fileinfo);

    bool IsProjectFileDownloadable(const wstring & projID, const ProjFileData & fileinfo);

    bool RemoveProjMember(const std::wstring & projid, std::wstring & memberId);

    bool ResendInvitation(const std::wstring & invitationId, const std::wstring emailAddr);

    bool RevokeInvitation(const std::wstring & invitationId);

    bool IsExtension(CString fName, CString ext);

    void ListRecentProjectFiles(CProjData & projData, size_t maxFiles);

    bool IsSkyDRMUser();

    wstring ReplaceTimestamp(const std::wstring & s);

	void ListProjectFiles(wstring currentPath, CProjData & projData);

	wstring GetCurrentProjID();
    BOOL RefreshCurrentProject(void);

   



	//Favorite repository functions
	BOOL SetFileToFavorite(int index, std::wstring parentPath, RepositoryFileInfo &file);
	BOOL DeleteFavoriteFile(RepositoryFileInfo &fileinfo);
	BOOL GetFavoriteFileList(std::vector<std::pair<int, std::vector<RepositoryFileInfo>>>& files, BOOL brefresh = FALSE);

	//Offline repository functions
	std::wstring GetOfflineFilePath(RepositoryFileInfo fileinfo);
	std::wstring GenerateOfflineFilePath(RepositoryFileInfo fileinfo);
	BOOL GetOfflineFileList(std::vector<std::pair<int, std::vector<RepositoryFileInfo>>>& files);
	BOOL SetFileToOffline(std::vector<std::pair<int, std::vector<RepositoryFileInfo>>>& files);
	
	//UI Related
	BOOL OpenProtectDlg(const HWND hWnd, int index, const RepositoryFileInfo &repoFile);
	void ShowRepositoryFileProperty(const HWND hWnd, const RepositoryFileInfo &fileinfo, HICON fileicon);
	BOOL ShowMyVaultFileProperty(const HWND hWnd, MyVaultFileInfo & fileinfo, HICON fileicon, BOOL bviewonly = FALSE);// return true to refresh myvault file list
	BOOL ShareRepositoryFile(const HWND hwnd, const RepositoryFileInfo &repoFile);
	BOOL ReShareMyValutFile(const HWND hwnd, HICON fileicon, const MyVaultFileInfo &mvFile);
	BOOL OpenAddRepositoryUI(const HWND hwnd = NULL); //return true if new repository is added.
	BOOL OpenRenameRepositoryUI(int repoindex, const HWND hwnd = NULL); //return true if the name is changed
	BOOL OpenDeleteRepositoryUI(int repoindex, const HWND hwnd = NULL); //return true if the repository is delete.
	BOOL ViewRepositoryFile(int index, std::wstring path, std::wstring filename);
	BOOL ShowEditUserProfile(const HWND hwnd = NULL);
	BOOL ViewMyVaultFile(std::wstring path, std::wstring filename);
	BOOL ViewLocalFile(RepositoryFileInfo file);
    void OpenProjFileFunc(const CProjData & projData, const wstring fileName, wstring dispName);
    bool LocalFileView(const std::wstring & file, const std::wstring & displayName = L"");


	void LogoutUser(void);
	void ReloadMainMenu(HWND hwnd);

	void CenterHomePage(void);

    void MessageYield();

	std::wstring CheckForUpdate(BOOL * bUpToData);//return empty or latest version string
	void DownloadNewVersion(BOOL bSilent, StatusCallBackFunc func = NULL, MSG * message = NULL);
	BOOL IsNewVersionAvailable(void) { return (!m_productUpgradeInfo.GetDownloadedFile().empty()); }
	void InstallNewVersion(void);
    CString LoadString(int nID);

public:
	std::wstring GetParentFolder(const std::wstring &folder);
	std::wstring SerilaizeEmail(const std::vector<std::wstring> &emailArr);
	std::vector<std::wstring>DeserializeEmail(const std::wstring &emailstr); //seperate with ;

	BOOL IsProgressDlgRunning(void);
	BOOL IsDialogPopup(void);
	BOOL DestroyTrayIcon(HWND hMainWnd);
	bool InitTrayIcon(HWND hMainWnd, HINSTANCE hInstance);

	void ShowTrayMsg(LPCWSTR wzInfo, LPCWSTR wzTitle = NULL);

	void ShowPage(MAINUI_DIAGLOGLIST pageid);
	void ShowSignup(void);
	void ShowLogin(void);
	virtual BOOL InitInstance();
    virtual BOOL ExitInstance();

	BOOL InitializeWindows(CWnd * parentWindow = NULL);
	BOOL InitializeRestfulData(void);

	std::wstring GetArgOpenFileName() { return m_strArgOpenFileName; }
	void ClearArgOpenFile() { m_strArgOpenFileName.clear(); }
    void OpenProjFile(const CProjData & projData, const wstring fileName, wstring dispName);

    //void OpenProjFile(const CProjData & projData, const wstring fileName);
    //bool LocalFileView(const std::wstring & file);

    DECLARE_MESSAGE_MAP()
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
    //Resource related functions:
};


extern CnxrmRMCApp theApp;

extern int GetRepoSelIconID(const int repoType);

extern int GetRepoIconID(const int repoType);

extern int GetRepoImageID(const int repoType);

extern std::wstring GetDriveTypeName(const int repoType);

extern void DisplayMyVaultInfo(const NX::RmFileShareResult& shareResult, BOOL bshared);

std::wstring OutputError(NX::Result &res, wstring errMsg);
