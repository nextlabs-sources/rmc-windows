#pragma once
#include "FlatDlgBase.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "ListBoxCH.h"
#include "BtnClass.h"
#include "BtnClass.h"
#include "SwitchProj.h"
#include "PromptEdit.h"
#include <vector>
#include "resource.h"
#include "commondlg.hpp"
#include <map>
#include <nx\rmc\session.h>
#include <nx\common\stringex.h>
#include <nx\common\time.h>
#include "NxMFCListCtrl.h"
#include "OwnerDrawStatic.h"
#include "..\common\inc\BtnClass.h"

// CRepoPage dialog


enum NxListType { NX_RMC_FOLDER, NX_RMC_FILE, };

using namespace std;

enum IconType { NX_DOC_TYPE, NX_PDF_TYPE, };

enum OthersRepoType { REPO_GOOGLE, REPO_NEXTLABS, REPO_ONEDRIVE, REPO_DROPBOX,REPO_SHAREPOINT };

enum RepositoryType {NX_BTN_OTHERREPOS = 0, NX_BTN_MYVAULT, NX_BTN_MYDRIVE, NX_BTN_DELETEFILES, NX_BTN_SHAREFILES,  NX_BTN_FAVORITE, NX_BTN_OFFLINE};

enum MyVaultFilterType { MVF_ACTIVESHARES = -3,  MVF_ALLSHARES = -2, MVF_UNKNOWN = -1, MVF_ALLFILES = 0, MVF_SHAREDFILES, MVF_PROTECTED, MVF_DELETEFILES, MVF_REVOKED };//This type matches Myvault filter control data sequence


typedef struct {
	OthersRepoType storType;
	int nIcoID;
	int nImgID;
	CString captionText;
}   DriveTypes;

extern vector<DriveTypes> drvTypeArray;

typedef struct {
	NX::RmRepository::REPO_TYPE type;
	std::wstring id;
	std::wstring nxrmPath;
	std::wstring dispPath;
} RepositoryData;


typedef struct _Repository_FileInfo{
	int		iRepoIndex;
	std::wstring strFileName;
	std::wstring strFileExtension;
	BOOL		bIsNXL;
	std::wstring strPath;
	std::wstring strDisplayPath;
	std::wstring strFileDate;
	__int64		 lFileDate;
	std::vector<std::pair<std::wstring, std::wstring>> strFilePropertyArr;
	__int64		iFileSize;
	std::wstring strFileSize;
	BOOL		bIsDirectory;
	BOOL		bIsOffline; //Local variable for UI only
	BOOL		bIsFavorite;//Local variable for UI only
} RepositoryFileInfo;

typedef struct _MyVault_FileInfo {
	RepositoryFileInfo rFileInfo;
	std::wstring strDuid;
	std::wstring strOriginalPath;
	std::wstring strSharedWith;
	std::vector<std::wstring> strSharedArr; //this information will be sync up after user want to see the detail file information
	BOOL	bIsRevoked;
	BOOL	bIsShared;
	BOOL	bIsDeleted;
}MyVaultFileInfo;

typedef struct _REPOSITORY_INFO {
	NX::RmRepository::REPO_TYPE Type;
	std::wstring Id;
	std::wstring Name;
	std::wstring AccountName;
	std::wstring AccountId;
	std::wstring TypeName;
	int iDataIndex;//this is internal variable. it is pointed to RepoPage data structure for each repository
}RepositoryInfo;

class CRepoPage : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CRepoPage)

public:
	CRepoPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRepoPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    void ShowSearchUI(bool bShow);

	void RepoSearchFile(int nIndex, const std::wstring & keywords, std::wstring showpath = L"", BOOL brefresh = FALSE);
	void MyVaultListFiles(const std::wstring & keywords);

    virtual BOOL OnInitDialog();

    CToolTipCtrl m_ToolTip;
	DECLARE_MESSAGE_MAP()
public:
    
    virtual BOOL PreTranslateMessage(MSG * pMsg);
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedButtonBack();
    afx_msg void OnIdok();
    afx_msg void OnIdcancel();
    afx_msg void OnClickedButtonEdit();
    // Repository File list
	CNxMFCListCtrl m_fileList;
    // Selected repository name
    ::CStatic m_stcRepoPath;
    ::CComboBox m_filterList;
    afx_msg void OnDestroy();
    afx_msg void OnLbnSelchangeListRepo();
    void DoRefreshRepoFileList(int nSelIdx, BOOL brefresh = FALSE);
    ::CStatic m_RepositoryName;
    ::CStatic m_TitleText;
    CBtnClass m_searchBtn;
    afx_msg void OnBnClickedButtonSearch();

	// Repository list
	CListBoxCH m_RepoList;

	void GotoRepository(RepositoryType rtype, std::wstring reponame = L"");
	void ShowAddRepository();

	void ResetData(void);
	void RefreshPage(void);
	void RefreshRepoList();
	void SetMyVaultRefreshFlag(BOOL brefresh = TRUE) { m_bRefreshMyVault = brefresh; }
	void SortColumn(int columnindex, BOOL bRefresh = FALSE);
	BOOL IsBackAvailable(void) { if (m_bSwitchpage)return FALSE; return (m_uHistoryPos != m_uHistoryHead); }
	BOOL IsForwardAvailable(void) { if (m_bSwitchpage)return FALSE; return (m_uHistoryPos != m_uHistoryTail); }
	BOOL IsFileItemSelected(void) {	if (m_bSwitchpage) return FALSE; if (m_fileList.GetSelectionMark() == -1)return FALSE; if (GetFileListRepoDataIndex() == -1)return FALSE; return TRUE;	}
	BOOL IsFileSelected(void) { if (m_bSwitchpage)return FALSE;  if (m_fileList.GetSelectionMark() == -1)return FALSE; if (GetFileListRepoDataIndex() == -1)return FALSE; return !m_fileData[GetFileListRepoDataIndex()].bIsDirectory; }
	int  GetSelectRepositoryIndex(void) { return m_RepoList.GetCurSel(); }
	BOOL IsUploadAvailable(void);
	BOOL IsShareAvailable(void);
	BOOL IsProtectAvailable(void);
	BOOL IsFileInfoAvailable(void);
	BOOL IsFileDeleteAvailable(void);
	BOOL IsFileRevokeAvailable(void);

	void ResetMySpaceButton(void);
    BOOL IsSwitchPageOpen() { return m_bSwitchpage; }
protected:
	HBRUSH m_hGray;
	HBRUSH m_hGreen;

	std::vector<RepositoryInfo> m_RepositoryArr;

	vector<RepositoryData> m_RepoDataArr;	//This is the current directories for each repo.

	vector <RepositoryFileInfo> m_fileData;
	vector <MyVaultFileInfo> m_MyVaultFileInfo;
	BOOL m_bRefreshFileList;
	BOOL m_bRefreshMyVault;
	BOOL m_bRefreshMyFavorite;
	vector<std::pair<int, vector <RepositoryFileInfo>>> m_OfflineFiles;
	vector<std::pair<int, vector <RepositoryFileInfo>>> m_FavoriteFiles;
	wstring m_strCurRepoParentPath;
	NX::SORTORDER m_sortedType;
	
	//Those two variables are use to reserved the string buffer to pass in thread
	std::wstring m_strDownloadFullPath;
	std::wstring m_strUploadFullPath;
	std::wstring m_strUploadTargetName;

	void OnHideAllCtrl(BOOL bhide = TRUE);
private:
	BOOL IsOfflineFile(const RepositoryFileInfo &file);
	BOOL IsFavoriteFile(const RepositoryFileInfo &file);
	void UpdateRepoFileList(std::vector<RepositoryFileInfo> filelist, BOOL bAppend = FALSE);
	void UpdateRepoFileList(vector<std::pair<int, vector <RepositoryFileInfo>>> &files);
	void UpdateMyVaultFileList(vector <MyVaultFileInfo> &filelist, MyVaultFilterType filter= MVF_UNKNOWN);
	
	void InitializeRepositoryFileListControl(void);
	void InitializeMyVaultFileListControl(void);
	void InitializeRepositoryList(void);

	inline int GetReposDataIndex(int idx) { if (idx < 0 || idx >= (int)m_RepositoryArr.size()) return -1;  return m_RepositoryArr[idx].iDataIndex; }

	BOOL m_bSwitchpage;

	vector<RepositoryData> m_BrowseHistory;
	unsigned m_uHistoryHead;
	unsigned m_uHistoryTail;
	unsigned m_uHistoryPos;
	void AddHistoryInfo(RepositoryData &repodata);
	void UpdateBrowserButton(void);
	BOOL GotoHistoryPosition(void);

	int m_iCurSelectedListColumn;
	BOOL m_bColumnAscend;

	void RepoOpenFileOrFolder(RepositoryFileInfo &pItemData, int idx);
	void RefreshRepoOnly();
	void ListFavoriteFilesForAllRepos(BOOL brefresh = FALSE);
	void ListOfflineFilesForAllRepos();
	void ChangeRepo(RepositoryType idx);

	void ResetSearch();
	void RefreshSearchList();

	int GetFileListRepoDataIndex(int nsel = -1);
	int GetFileListMyVaultDataIndex(int nsel = -1);

	void MoveWindowPos(CWnd * pwnd, int cx, int cy, BOOL bKeepLeft = FALSE);
	void ResizeRepoFileListControl(void);
	void ResizeMyVaultFileListcontrol(void);
public:

	//Store Data for search
	bool m_searchMode;
    // Current Selected file
	RepositoryType m_selBtnIdx;

	CEdit m_searchEditBox;
	CString m_strSearch;
	vector<CBtnClass*> m_btnArr;
	CBtnClass m_btnAllFiles;
	CBtnClass m_btnFavFiles;
	CBtnClass m_btnOfLnfiles;
	CBtnClass m_btnMyVault;
	CBtnClass m_btnRefresh;
	CBtnClass m_btnUpload;
	CBtnClass m_resetSearchBtn;
	CBtnClass m_btnAddDir;
	CBtnClass m_btnMyDrive;
	CBtnClass m_btnGoBack;
	CBtnClass m_btnForward;
	CStatic m_navBand;

	COwnerDrawStatic m_searchGroup;
	CStatic m_totalSize;
	CStatic m_userSize;
	CStatic m_myDriveSize;
	CStatic m_myVaultSize;
    CSwitchProj m_switchDlg;


	afx_msg void OnFilecontextmenuDownload();
	afx_msg void OnFilecontextmenuView();

	afx_msg void OnNMDblclkListRepoFiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFilecontextmenuFileproperties();
	afx_msg void OnFilecontextmenuProtect();
	afx_msg void OnFilecontextmenuShare();
	afx_msg void OnBnClickedRadioAllFiles();
	afx_msg void OnBnClickedRadioFavorite();
	afx_msg void OnBnClickedRadioMydrive();
	afx_msg void OnBnClickedRadioOffline();
	afx_msg void OnBnClickedRadioMyvault();
	afx_msg void OnFilecontextmenuDelete();
	
	afx_msg void OnBnClickedButtonRefresh();
    virtual void DropOneFile(CString folderToSave, CString fileToSave);
   
	afx_msg void OnBnClickedButtonUpload();
	afx_msg void OnBnClickedButtonResetSearch();
	virtual void OnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void PreSubclassWindow();
    afx_msg void OnFilecontextmenuFavorite();
    afx_msg void OnFilecontextmenuOffline();
	afx_msg void OnClickedButtonGoback();
	afx_msg void OnClickedButtonForward();
	afx_msg void OnBnClickedButtonNewdir();
	afx_msg void OnRclickListRepofiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboMyvalutfilter();
	CString m_strMyvalutFilter;
	CComboBox m_cmbMyvaultFilter;
	CBtnClass m_btnLoadMore;
	NX::RmFinder m_finder;

	afx_msg void OnItemchangedListRepofiles(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnManage();
	afx_msg void UpdateFileListData(); //internal message handle function to refresh filelist on callback.
	void ResizeWindow(int cx, int cy);
	CStatic m_stcHorizLine;
	CStatic m_stcVerticalLine;
	CStatic m_stcVertiSeprator;
//	CButton m_btnResetSearch;
	CBtnClass m_btnDeletedFiles;
	CBtnClass m_btnSharedFiles;
	afx_msg void OnClickedRadioDeletefiles();
	afx_msg void OnClickedRadioSharedfiles();
	afx_msg void OnFilecontextmenuViewactivity();
	CStatic m_stcHomeText;
	CStatic m_stcMySpaceText;
	CStatic m_stcTitleBar;
	CBtnClass m_btnUpDown;
	afx_msg void OnBnClickedButtonUpdown();
	afx_msg void OnStnClickedHomeText();
	CComboBox m_cmbProjects;
	afx_msg void OnCbnSelchangeComboProjects();
	afx_msg void OnSelendcancelComboProjects();

	afx_msg LRESULT OnRefreshRepoList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshFileList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshMyVaultFileList(WPARAM wParam, LPARAM lParam);
	CStatic m_stcBtnHorizLine;
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClickedButtonLoadmore();
};

