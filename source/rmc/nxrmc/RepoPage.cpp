// RepoPage.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "RepoPage.h"
#include "afxdialogex.h"
#include <vector>
#include <string>
#include "RepoEditor.h"
#include "ConnectToRepo.h"
#include "commondlg.hpp"
#include <iostream>
#include <sstream>
#include <thread>


#include <nx\rmc\repo.h>
#include "ProjData.h"
#include "NewFolderDlg.h"
#include "UploadFileDlg.h"                            
#include "DeleteDlg.h"
#include "ActivityLogDlg.h"

using namespace std;

using namespace NX;

extern void ResizeRepoPage(HWND hParentWnd, int width, int height);


#define MAX_BROWSEHISTORY_NUM	10
#define MYVALUT_IDSTRING		L"MYVAULT"
#define OFFLINE_IDSTRING		L"OFFLINEFILES"
#define FAVORITE_IDSTRING		L"FAVORITEFILES"
#define DELETEDFILE_IDSTRING	L"DELETEDFILES"
#define SHAREDFILE_IDSTRING		L"SHAREDFILES"


void ProgressCallBack(MSG wmdata)
{
	::PostMessage(wmdata.hwnd, wmdata.message, wmdata.wParam, wmdata.lParam);
}

// CRepoPage dialog

IMPLEMENT_DYNAMIC(CRepoPage, CFlatDlgBase)

void CRepoPage::ResetData()
{
	m_bRefreshMyFavorite = FALSE;
	m_bRefreshFileList = FALSE;
	m_bRefreshMyVault = TRUE; //refresh my vault for first time

	m_strMyvalutFilter = _T("");
	m_strSearch = _T("");
	m_bSwitchpage = FALSE;
	m_selBtnIdx = NX_BTN_MYVAULT;

	m_RepoDataArr.clear();

	m_fileData.clear();
	m_MyVaultFileInfo.clear();
	m_OfflineFiles.clear();
	m_FavoriteFiles.clear();
}

CRepoPage::CRepoPage(CWnd* pParent /*=NULL*/)
    : CFlatDlgBase(IDD_FILE_LIST, pParent)
    , m_selBtnIdx(NX_BTN_OTHERREPOS)
    , m_sortedType(NX::OrderByCreationTimeDescend)
{
	ResetData();
}

CRepoPage::~CRepoPage()
{
}

void CRepoPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REPO, m_RepoList);
	DDX_Control(pDX, IDC_LIST_REPOFILES, m_fileList);
	DDX_Control(pDX, IDC_STATIC_REPO_PATH, m_stcRepoPath);
	DDX_Control(pDX, IDC_STATIC_PROJECT_NAME, m_RepositoryName);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_searchBtn);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_searchEditBox);
	DDX_Control(pDX, IDC_RADIO_ALL_FILES, m_btnAllFiles);
	DDX_Control(pDX, IDC_RADIO_FAVORITE, m_btnFavFiles);
	DDX_Control(pDX, IDC_RADIO_OFFLINE, m_btnOfLnfiles);
	DDX_Control(pDX, IDC_RADIO_MYVAULT, m_btnMyVault);
	DDX_Control(pDX, IDC_BUTTON_REFRESH, m_btnRefresh);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, m_btnUpload);
	DDX_Control(pDX, IDC_BUTTON_RESET_SEARCH, m_resetSearchBtn);
	DDX_Control(pDX, IDC_BUTTON_NEWDIR, m_btnAddDir);
	DDX_Control(pDX, IDC_RADIO_MYDRIVE, m_btnMyDrive);
	DDX_Control(pDX, IDC_BUTTON_GOBACK, m_btnGoBack);
	DDX_Control(pDX, IDC_BUTTON_FORWARD, m_btnForward);
	DDX_Control(pDX, IDC_STATIC_NAVIGATION_BAND, m_navBand);
	DDX_CBString(pDX, IDC_COMBO_MYVALUTFILTER, m_strMyvalutFilter);
	DDX_Control(pDX, IDC_COMBO_MYVALUTFILTER, m_cmbMyvaultFilter);
	DDX_Text(pDX, IDC_EDIT_SEARCH, m_strSearch);
	DDV_MaxChars(pDX, m_strSearch, 30);
	DDX_Control(pDX, IDC_STATIC_HORIZ, m_stcHorizLine);
	DDX_Control(pDX, IDC_STATIC_VIRTICAL_LINE, m_stcVerticalLine);
	DDX_Control(pDX, IDC_STATIC_VERTI_SEPRATOR, m_stcVertiSeprator);
	DDX_Control(pDX, IDC_RADIO_DELETEFILES, m_btnDeletedFiles);
	DDX_Control(pDX, IDC_RADIO_SHAREDFILES, m_btnSharedFiles);
	DDX_Control(pDX, IDC_HOME_TEXT, m_stcHomeText);
	DDX_Control(pDX, IDC_STATIC_CAPTION_NAME, m_stcMySpaceText);
	DDX_Control(pDX, IDC_STATIC_HIGHTLITE, m_stcTitleBar);
	DDX_Control(pDX, IDC_BUTTON_UPDOWN, m_btnUpDown);
	DDX_Control(pDX, IDC_COMBO_PROJECTS, m_cmbProjects);
	DDX_Control(pDX, IDC_STATIC_HORIZBTNSEPERATOR, m_stcBtnHorizLine);
	DDX_Control(pDX, IDC_BUTTON_LOADMORE, m_btnLoadMore);
}



BEGIN_MESSAGE_MAP(CRepoPage, CDialogEx)
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BUTTON_BACK, &CRepoPage::OnBnClickedButtonBack)
    ON_COMMAND(IDOK, &CRepoPage::OnIdok)
    ON_COMMAND(IDCANCEL, &CRepoPage::OnIdcancel)
    ON_BN_CLICKED(IDC_BUTTON_EDIT, &CRepoPage::OnClickedButtonEdit)
    ON_WM_DESTROY()
    ON_LBN_SELCHANGE(IDC_LIST_REPO, &CRepoPage::OnLbnSelchangeListRepo)
    ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CRepoPage::OnBnClickedButtonSearch)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_REPOFILES, &CRepoPage::OnNMDblclkListRepoFiles)
    ON_COMMAND(ID_FILECONTEXTMENU_FILEPROPERTIES, &CRepoPage::OnFilecontextmenuFileproperties)
    ON_COMMAND(ID_FILECONTEXTMENU_PROTECT, &CRepoPage::OnFilecontextmenuProtect)
    ON_COMMAND(ID_FILECONTEXTMENU_SHARE, &CRepoPage::OnFilecontextmenuShare)
    ON_BN_CLICKED(IDC_RADIO_ALL_FILES, &CRepoPage::OnBnClickedRadioAllFiles)
    ON_BN_CLICKED(IDC_RADIO_FAVORITE, &CRepoPage::OnBnClickedRadioFavorite)
    ON_BN_CLICKED(IDC_RADIO_OFFLINE, &CRepoPage::OnBnClickedRadioOffline)
    ON_BN_CLICKED(IDC_RADIO_MYVAULT, &CRepoPage::OnBnClickedRadioMyvault)
    ON_COMMAND(ID_FILECONTEXTMENU_DELETE, &CRepoPage::OnFilecontextmenuDelete)
    ON_COMMAND(ID_FILECONTEXTMENU_DOWNLOAD, &CRepoPage::OnFilecontextmenuDownload)
    ON_COMMAND(ID_FILECONTEXTMENU_VIEW, &CRepoPage::OnFilecontextmenuView)
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CRepoPage::OnBnClickedButtonRefresh)
    ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CRepoPage::OnBnClickedButtonUpload)
    ON_BN_CLICKED(IDC_BUTTON_RESET_SEARCH, &CRepoPage::OnBnClickedButtonResetSearch)
    ON_COMMAND(ID_FILECONTEXTMENU_FAVORITE, &CRepoPage::OnFilecontextmenuFavorite)
    ON_COMMAND(ID_FILECONTEXTMENU_OFFLINE, &CRepoPage::OnFilecontextmenuOffline)
    ON_BN_CLICKED(IDC_RADIO_MYDRIVE, &CRepoPage::OnBnClickedRadioMydrive)
	ON_BN_CLICKED(IDC_BUTTON_GOBACK, &CRepoPage::OnClickedButtonGoback)
	ON_BN_CLICKED(IDC_BUTTON_FORWARD, &CRepoPage::OnClickedButtonForward)
	ON_BN_CLICKED(IDC_BUTTON_NEWDIR, &CRepoPage::OnBnClickedButtonNewdir)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_REPOFILES, &CRepoPage::OnRclickListRepofiles)
	ON_CBN_SELCHANGE(IDC_COMBO_MYVALUTFILTER, &CRepoPage::OnCbnSelchangeComboMyvalutfilter)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REPOFILES, &CRepoPage::OnItemchangedListRepofiles)
	ON_COMMAND(ID_FILECONTEXTMENU_MANAGE, &CRepoPage::OnManage)
	ON_COMMAND(ID_FILECONTEXTMENU_REFRESH, &CRepoPage::UpdateFileListData)
	ON_BN_CLICKED(IDC_RADIO_DELETEFILES, &CRepoPage::OnClickedRadioDeletefiles)
	ON_BN_CLICKED(IDC_RADIO_SHAREDFILES, &CRepoPage::OnClickedRadioSharedfiles)
	ON_COMMAND(ID_FILECONTEXTMENU_VIEWACTIVITY, &CRepoPage::OnFilecontextmenuViewactivity)
	ON_BN_CLICKED(IDC_BUTTON_UPDOWN, &CRepoPage::OnBnClickedButtonUpdown)
	ON_STN_CLICKED(IDC_HOME_TEXT, &CRepoPage::OnStnClickedHomeText)
	ON_CBN_SELCHANGE(IDC_COMBO_PROJECTS, &CRepoPage::OnCbnSelchangeComboProjects)
	ON_CBN_SELENDCANCEL(IDC_COMBO_PROJECTS, &CRepoPage::OnSelendcancelComboProjects)
	ON_MESSAGE(WM_NX_REFRESH_REPO_LIST, OnRefreshRepoList)
	ON_MESSAGE(WM_NX_REFRESH_FILE_LIST, OnRefreshFileList)
	ON_MESSAGE(WM_NX_REFRESh_MYVALUT_LIST, OnRefreshMyVaultFileList)
    ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_LOADMORE, &CRepoPage::OnClickedButtonLoadmore)
END_MESSAGE_MAP()


void CRepoPage::ShowSearchUI(bool bShow)
{
    auto showFlag = bShow ? SW_SHOW : SW_HIDE;
    m_searchBtn.ShowWindow(showFlag);
    m_searchEditBox.ShowWindow(showFlag);
    m_resetSearchBtn.ShowWindow(m_strSearch.GetLength()>0 ? showFlag: SW_HIDE);
}

BOOL CRepoPage::IsOfflineFile(const RepositoryFileInfo &file)
{
	if (m_bSwitchpage)
		return FALSE;

	for (size_t s1 = 0; s1 < m_OfflineFiles.size(); s1++) {
		if (m_OfflineFiles[s1].first != file.iRepoIndex) {
			continue;
		}
		for (size_t s2 = 0; s2 < m_OfflineFiles[s1].second.size(); s2++) {
			if (m_OfflineFiles[s1].second[s2].strPath.compare(file.strPath) == 0)
				return TRUE;
		}
	}

	return FALSE;
}
BOOL CRepoPage::IsFavoriteFile(const RepositoryFileInfo &file)
{
	if (m_bSwitchpage)
		return FALSE;

	for (size_t s1 = 0; s1 < m_FavoriteFiles.size(); s1++) {
		if (m_FavoriteFiles[s1].first != file.iRepoIndex) {
			continue;
		}
		for (size_t s2 = 0; s2 < m_FavoriteFiles[s1].second.size(); s2++) {
			if (m_FavoriteFiles[s1].second[s2].strPath.compare(file.strPath) == 0)
				return TRUE;
		}
	}

	return FALSE;
}

void CRepoPage::UpdateRepoFileList(vector<std::pair<int, vector<RepositoryFileInfo>>> &files)
{
	m_fileData.clear();
	m_fileList.DeleteAllItems();
	InitializeRepositoryFileListControl();
	for_each(files.cbegin(), files.cend(), [&](const std::pair<int, std::vector<RepositoryFileInfo>> filelist) {
		UpdateRepoFileList(filelist.second, TRUE);
	});
}

void CRepoPage::UpdateRepoFileList(std::vector<RepositoryFileInfo> filelist, BOOL bAppend)
{
	if (!bAppend) {
		m_fileData.clear();
		m_fileList.DeleteAllItems();
		InitializeRepositoryFileListControl();
		if (m_strCurRepoParentPath.compare(L"/") != 0) {//add up
			LVITEM listItem;
			m_fileList.InsertItem(0, L"");

			memset((void*)&listItem, 0, sizeof(LVITEM));
			listItem.mask = LVIF_IMAGE | LVIF_TEXT;
			listItem.pszText = L"";
			listItem.iItem = 0;
			listItem.iSubItem = 0;
			listItem.iImage = theApp.GetUpFolderImageIndex();
			m_fileList.SetItem(&listItem);
			m_fileList.SetItemData(0, -1);
		}
	}

	std::for_each(filelist.cbegin(), filelist.cend(), [&](const RepositoryFileInfo & file) {
		LVITEM listItem;
		int i = m_fileList.GetItemCount();
		m_fileList.InsertItem(i, L"");

		memset((void*)&listItem, 0, sizeof(LVITEM));
		listItem.mask = LVIF_IMAGE | LVIF_TEXT;
		CString fName = file.strFileName.c_str();
		listItem.pszText = fName.GetBuffer();
		listItem.iItem = i;
		listItem.iSubItem = 0;


		if (file.bIsDirectory) {
			listItem.iImage = theApp.GetFolderImageIndex();
			m_fileList.SetItem(&listItem);
			if (file.lFileDate > 60*60*1000) {
				m_fileList.SetItemText(i, 1, file.strFileDate.c_str());
			}
			else {
				m_fileList.SetItemText(i, 1, L"-");
			}
			m_fileList.SetItemText(i, 2, theApp.LoadString(IDS_FOLDER)/*L"Folder"*/);
			wstringstream txtOut;
			txtOut.str(L"");
			switch (file.iRepoIndex) {
			case 0:
				txtOut << theApp.LoadStringW(IDS_MYDRIVE).GetBuffer()/*"MyDrive"*/;
				break;
			default:
				if (file.iRepoIndex > 0) {
					txtOut << m_RepositoryArr[file.iRepoIndex].Name;
				}
				break;
			}
			m_fileList.SetItemText(i, 4, txtOut.str().c_str());
		}
		else {
			listItem.iImage = theApp.GetFileExtensionImageIndex(file.strFileExtension, file.bIsNXL);
			m_fileList.SetItem(&listItem);

			m_fileList.SetItemText(i, 1, file.strFileDate.c_str());

			wstringstream txtOut;

			txtOut.str(L"");
			std::wstring strtype = file.strFileExtension;
			strtype.erase(0, 1);
			std::transform(strtype.cbegin(), strtype.cend(), strtype.begin(), ::towupper);
			txtOut.str(L"");
			txtOut << strtype << theApp.LoadStringW(IDS_TYPE_SUFFIX).GetBuffer()/*L" Type"*/;
			m_fileList.SetItemText(i, 2, txtOut.str().c_str());
			m_fileList.SetItemText(i, 3, file.strFileSize.c_str());
			txtOut.str(L"");
			switch (file.iRepoIndex) {
			case 0:
				txtOut << theApp.LoadStringW(IDS_MYDRIVE).GetBuffer()/*"MyDrive"*/;
				break;
			default:
				if (file.iRepoIndex > 0) {
					txtOut << m_RepositoryArr[file.iRepoIndex].Name;
				}
				break;
			}
			m_fileList.SetItemText(i, 4, txtOut.str().c_str());
		}
		RepositoryFileInfo fileinfo = file;
		fileinfo.bIsOffline = IsOfflineFile(file);
		fileinfo.bIsFavorite = IsFavoriteFile(file);
		m_fileData.push_back(fileinfo);
		m_fileList.SetItemData(i, m_fileData.size()-1);
	});
}

void CRepoPage::UpdateMyVaultFileList(vector <MyVaultFileInfo> &filelist, MyVaultFilterType filter)
{
	BOOL bAppend = FALSE;
	int ncurSel = -1;
	if (m_fileList.GetItemCount() >= 25 && (int) filelist.size() > m_fileList.GetItemCount()) {
		bAppend = TRUE;
	}
	if (!bAppend) {
		m_fileData.clear();
		m_fileList.DeleteAllItems();
		InitializeMyVaultFileListControl();
	}
	else {
		ncurSel = m_fileList.GetSelectionMark();//save the current selection.
	}

	size_t s = 0;
	if (bAppend) {
		s = m_fileList.GetItemCount();
	}
	for (; s < filelist.size(); s++) {
// 		if (filter == MVF_UNKNOWN)
// 			filter = (MyVaultFilterType)m_cmbMyvaultFilter.GetCurSel();
// 		switch (filter)
// 		{
// 		case MVF_ACTIVESHARES:
// 			if (!filelist[s].bIsShared || filelist[s].bIsDeleted || filelist[s].bIsRevoked)
// 				continue;
// 			break;
// 		case MVF_ALLSHARES:
// 			if (!filelist[s].bIsShared)
// 				continue;
// 			break;
// 		case MVF_PROTECTED:
// 			if (filelist[s].bIsShared)
// 				continue;
// 			break;
// 		case MVF_REVOKED:
// 			if (!filelist[s].bIsRevoked)
// 				continue;
// 			break;
// 		case MVF_DELETEFILES:
// 			if (!filelist[s].bIsDeleted)
// 				continue;
// 			break;
// 		case MVF_SHAREDFILES:
// 			if (!filelist[s].bIsShared || filelist[s].bIsRevoked || filelist[s].bIsDeleted)
// 				continue;
// 			break;
// 		default:
// 			break;
// 		}
		//Add item to the list
		LVITEM listItem;
		int i = m_fileList.GetItemCount();
		m_fileList.InsertItem(i, L"");
		memset((void*)&listItem, 0, sizeof(LVITEM));
		listItem.mask = LVIF_IMAGE | LVIF_TEXT;
		CString fName;
		if (filelist[s].bIsDeleted)
			fName = theApp.LoadString(IDS_DELETED_PREFIX)/*"(DELETED) "*/;
		else if (filelist[s].bIsRevoked) {
			fName = theApp.LoadString(IDS_REVOKE_PREFIX)/*"(REVOKED) "*/;
		}
		fName += filelist[s].rFileInfo.strFileName.c_str();
		listItem.pszText = fName.GetBuffer();
		listItem.iItem = i;
		listItem.iSubItem = 0;


		listItem.iImage = theApp.GetFileExtensionImageIndex(filelist[s].rFileInfo.strFileExtension, filelist[s].rFileInfo.bIsNXL);
		m_fileList.SetItem(&listItem);
		m_fileList.SetItemText(i, 1, filelist[s].strSharedWith.c_str());
		m_fileList.SetItemText(i, 2, filelist[s].rFileInfo.strFileDate.c_str());

		RepositoryFileInfo fileinfo = filelist[s].rFileInfo;
		m_fileData.push_back(fileinfo);
		DWORD data = (int)s;
		data = data << 16;
		data += (DWORD)m_fileData.size() - 1;
		m_fileList.SetItemData(i, data);
	}

	if (bAppend) {
		m_fileList.SetSelectionMark(ncurSel);
	}

	if (!m_finder.NoMoreData() && theApp.m_curShowedPage == MD_REPORITORYHOME && !m_bSwitchpage)
		m_btnLoadMore.ShowWindow(SW_SHOW);
	else
		m_btnLoadMore.ShowWindow(SW_HIDE);
}

void CRepoPage::RepoSearchFile(int nIndex, const std::wstring& keywords, std::wstring pathid, BOOL brefresh)
{
	int dataidx = GetReposDataIndex(nIndex);
	if (dataidx == -1) {
		theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_WHILE_SEARCH)/*L"An error occurred while performing search."*/);
		return;
	}
    wstring disppath = m_RepoDataArr[dataidx].dispPath;
	wstring path;
	if (pathid.length() == 0) {
		path = m_RepoDataArr[dataidx].nxrmPath;
	}
	else {
		path = pathid;
	}

	if (keywords.length()) {
		m_RepositoryName.SetWindowTextW(theApp.LoadString(IDS_SEARCH)/*L"Search"*/);
		path = L"/";
		m_stcRepoPath.SetWindowTextW(L"");
	}else{
		auto repoName = m_RepositoryArr[nIndex].Name;
		m_RepositoryName.SetWindowTextW(repoName.c_str());
		m_stcRepoPath.SetWindowTextW(disppath.c_str());
	}
    std::vector<RepositoryFileInfo> m_files;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    m_files.clear();
	if (theApp.GetRepositoryFileList(nIndex, path, m_files, keywords, brefresh)) {
		m_strCurRepoParentPath = path;
		if (m_selBtnIdx == NX_BTN_MYDRIVE && keywords.length() == 0) {//set MyDrive path for app thread to refresh
			theApp.SetMyDriveCurrentPath(m_strCurRepoParentPath);
		}
		UpdateRepoFileList(m_files);
	}
	else {
		CString errorMsg;
		errorMsg.FormatMessage(IDS_FMT_REFRESH_MSG, m_RepositoryArr[nIndex].Name.c_str());
		//auto strerr = L"An error occurred while refreshing the file list from " + m_RepositoryArr[nIndex].Name;
		theApp.ShowTrayMsg(errorMsg);
	}
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CRepoPage::MyVaultListFiles(const std::wstring& keywords)
{
	std::wstring strfilter;
	int sortTypes;

	switch (m_selBtnIdx)
	{
	case NX_BTN_MYVAULT:
	{
		MyVaultFilterType filter = (MyVaultFilterType)m_cmbMyvaultFilter.GetCurSel();
		switch (filter)
		{
		case MVF_ALLFILES:
			strfilter = L"allFiles";
			break;
		case MVF_SHAREDFILES:
			strfilter = L"activeTransaction";
			break;
		case MVF_PROTECTED:
			strfilter = L"protected";
			break;
		case MVF_DELETEFILES:
			strfilter = L"deleted";
			break;
		case MVF_REVOKED:
			strfilter = L"revoked";
			break;
		case MVF_ACTIVESHARES:
		case MVF_ALLSHARES:
		case MVF_UNKNOWN:
		default:
			ASSERT(TRUE);
			return;
		}

	}
		break;
	case NX_BTN_DELETEFILES:
		strfilter = L"deleted";
		break;
	case NX_BTN_SHAREFILES:
		strfilter = L"activeTransaction";
		break;
	case NX_BTN_OTHERREPOS:
	case NX_BTN_MYDRIVE:
	case NX_BTN_FAVORITE:
	case NX_BTN_OFFLINE:
	default:
		ASSERT(TRUE);
		return;
	}

	if (m_iCurSelectedListColumn == 0) {
		sortTypes = (m_bColumnAscend) ? NX::OrderByFileNameAscend : NX::OrderByFileNameDescend;
	}
	else if (m_iCurSelectedListColumn == 2) {
		sortTypes = (m_bColumnAscend) ? NX::OrderByCreationTimeAscend : NX::OrderByCreationTimeDescend;
	}
	else {
		sortTypes = NX::OrderByCreationTimeDescend;
	}
	m_MyVaultFileInfo.clear();
	m_fileList.DeleteAllItems();
	std::wstring searchfield = L"fileName";
	if (keywords.length() == 0) {
		searchfield.clear();
	}
	NX::RmFinder finder(25, std::vector<int>({ sortTypes }), strfilter, searchfield, keywords);
	m_finder = finder;
	MSG msg;
	msg.hwnd = m_hWnd;
	msg.message = WM_COMMAND;
	msg.wParam = ID_FILECONTEXTMENU_REFRESH;
	msg.lParam = 0;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	theApp.GetMyValutFirstPage(&m_finder, &m_MyVaultFileInfo, ProgressCallBack, &msg);
}

void CRepoPage::ResizeRepoFileListControl()
{
	CRect rcList;
	m_fileList.GetWindowRect(&rcList);
	int width = rcList.Width();
	m_fileList.SetColumnWidth(0, width * 7 / 12);
	m_fileList.SetColumnWidth(1, width * 1 / 6);
	m_fileList.SetColumnWidth(2, width / 12);
	m_fileList.SetColumnWidth(3, width / 12);
	m_fileList.SetColumnWidth(4, width / 12);
}

void CRepoPage::InitializeRepositoryFileListControl()
{
	while (m_fileList.DeleteColumn(0));//delete all column;
	CRect rcList;
	m_fileList.GetWindowRect(&rcList);
	int width = rcList.Width();
	m_fileList.InsertColumn(0, theApp.LoadString(IDS_NAME)/*L"Name"*/, LVCFMT_LEFT, width * 7 / 12);
	m_fileList.InsertColumn(1, theApp.LoadString(IDS_DATE_MODIFIED)/*L"Date modified"*/, LVCFMT_LEFT, width * 1 / 6);
	m_fileList.InsertColumn(2, theApp.LoadString(IDS_TYPE)/*L"Type"*/, LVCFMT_LEFT, width / 12);
	m_fileList.InsertColumn(3, theApp.LoadString(IDS_SIZE)/*L"Size"*/, LVCFMT_LEFT, width / 12);
	m_fileList.InsertColumn(4, theApp.LoadString(IDS_REPOSITORY)/*L"Repository"*/, LVCFMT_LEFT, width / 12);

	m_fileList.SetExtendedStyle(LVS_EX_SUBITEMIMAGES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EDITLABELS);
	m_fileList.SetSortColumn(-1);
}

void CRepoPage::ResizeMyVaultFileListcontrol()
{
	CRect rcList;
	m_fileList.GetWindowRect(&rcList);
	int width = rcList.Width();
	m_fileList.SetColumnWidth(0, width * 1 / 3);
	m_fileList.SetColumnWidth(1, width * 1 / 2);
	m_fileList.SetColumnWidth(2, width / 6);
}

void CRepoPage::InitializeMyVaultFileListControl()
{
	while (m_fileList.DeleteColumn(0));//delete all column;
	CRect rcList;
	m_fileList.GetWindowRect(&rcList);
	int width = rcList.Width();
	m_fileList.InsertColumn(0, theApp.LoadString(IDS_NAME)/*L"Name"*/, LVCFMT_LEFT, width * 1 /3);
	m_fileList.InsertColumn(1, theApp.LoadString(IDS_SHARED_WITH)/*L"Shared with"*/, LVCFMT_LEFT, width * 1/2);
	m_fileList.InsertColumn(2, theApp.LoadString(IDS_DATE_MODIFIED)/*L"Date modified"*/, LVCFMT_LEFT, width / 6);

	m_fileList.SetExtendedStyle(LVS_EX_SUBITEMIMAGES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EDITLABELS);
}

void CRepoPage::RefreshPage()
{
	InitializeRepositoryList();

	int idx = 0;
	m_RepoList.SetCurSel(-1);


	m_uHistoryHead = 0;
	m_uHistoryPos = -1;
	m_uHistoryTail = 0;
	m_BrowseHistory.clear();
	m_BrowseHistory.resize(MAX_BROWSEHISTORY_NUM);
	UpdateBrowserButton();

	for (size_t i = 0; i < m_btnArr.size(); i++) {
		m_btnArr[i]->SetBtnCheck(BST_UNCHECKED);
	}
	m_btnMyVault.SetBtnCheck(BST_CHECKED);
	m_selBtnIdx = NX_BTN_MYVAULT;

	UpdateBrowserButton();

	m_fileList.DeleteAllItems();
	m_fileList.SetImageList(theApp.GetFileExtensionImageList(), LVSIL_SMALL);
	m_iCurSelectedListColumn = -1;
	m_bColumnAscend = TRUE;

	m_cmbMyvaultFilter.SetCurSel(MVF_ALLFILES);
	m_bRefreshFileList = FALSE;
	m_bRefreshMyVault = TRUE;
	m_bRefreshMyFavorite = TRUE;

	m_searchEditBox.SendMessage(EM_SETCUEBANNER, 0, (LPARAM)(LPCWSTR)theApp.LoadStringW(IDS_SEARCH_FILE_FOLDER)/*L"Search File/Folder"*/);

//  disable refresh favorite and offline list for current release.
//	theApp.GetOfflineFileList(m_OfflineFiles);
//	theApp.GetFavoriteFileList(m_FavoriteFiles);
}

BOOL CRepoPage::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_hGray = CreateSolidBrush(NX_NEXTLABS_STANDARD_GRAY);
	m_hGreen = CreateSolidBrush(NX_NEXTLABS_STANDARD_GREEN);

    m_btnAllFiles.EnableWindow(FALSE);



    static CBitmap bmp;
    if (bmp.GetSafeHandle())
    {
        bmp.DeleteObject();
    }


	m_btnUpDown.SetImages(IDB_PNG_WHITE_GODOWN, 0, 0, NX_NEXTLABS_STANDARD_GREEN);
	AddFontSize(80, m_stcRepoPath, true);
    AddFontSize(80, m_RepositoryName, false);
	AddFontSize(80, m_stcHomeText, false);
	AddFontSize(80, m_stcMySpaceText, false);

	m_btnLoadMore.SetColors(RGB(255, 255, 255), NX_NEXTLABS_STANDARD_BLUE);
	m_cmbProjects.ShowWindow(SW_HIDE);

    m_btnAllFiles.SetImagetText(IDB_PNG_ALLFILEDISABLE, IDB_PNG_ALLFILEDISABLE, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_GREEN);
    m_btnMyVault.SetImagetText(IDB_PNG_MYVAULTBLACK, IDB_PNG_MYVAULTGRAY, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_GREEN);
	m_btnSharedFiles.SetImagetText(IDB_PNG_SHAREFILEBLACK, IDB_PNG_SHAREFILEGRAY, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_GREEN);
	m_btnDeletedFiles.SetImagetText(IDB_PNG_DELETEFILEBLACK, IDB_PNG_DELETEFILEGRAY, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_GREEN);
	m_btnFavFiles.SetImagetText(IDB_PNG_FAVBLACK, IDB_PNG_FAVGRAY, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_GREEN);
	m_btnOfLnfiles.SetImagetText(IDB_PNG_OFFLNBLACK, IDB_PNG_OFFLNGRAY, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_GREEN);
	auto normalID = GetRepoIconID(NX::RmRepository::MYDRIVE);
    auto selectedID = GetRepoSelIconID(NX::RmRepository::MYDRIVE);

	m_btnAllFiles.EnableWindow(FALSE);

    m_btnMyDrive.SetImagetText(IDB_PNG_MYDRIVE_BLACK, IDB_PNG_MYDRIVE_GRAY, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_GREEN);
    m_btnAddDir.SetImages(IDB_PNG_ADD_FOLDER, IDB_PNG_ADD_FOLDER, 0, NX_NEXTLABS_STANDARD_WHITE);

	//adding button sequence need match with RepositoryType 
    m_btnArr.push_back(&m_btnAllFiles);
    m_btnArr.push_back(&m_btnMyVault);
    m_btnArr.push_back(&m_btnMyDrive);
	m_btnArr.push_back(&m_btnDeletedFiles);
	m_btnArr.push_back(&m_btnSharedFiles);
//	m_btnArr.push_back(&m_btnFavFiles);
//	m_btnArr.push_back(&m_btnOfLnfiles);

	//disable offline&favorite button for current release.
	m_btnOfLnfiles.ShowWindow(SW_HIDE);
	m_btnOfLnfiles.EnableWindow(FALSE);
	m_btnFavFiles.ShowWindow(SW_HIDE);
	m_btnFavFiles.EnableWindow(FALSE);


    m_ToolTip.Create(this);

	m_switchDlg.Create(IDD_PROJECT_SWITCH, this);

	m_resetSearchBtn.SetImages(IDB_PNG_CLOSE, 0, 0);

    m_ToolTip.AddTool(&m_resetSearchBtn, IDS_CLEAR_SEARCH/*_T("Clear search")*/);
    m_searchBtn.SetImages(IDB_PNG_SEARCH, 0, 0);
    auto ret = m_ToolTip.AddTool(&m_searchBtn, IDS_SEARCH_FILES/*_T("Search files")*/);

    m_btnGoBack.SetImages(IDB_PNG_GOBACK, 0, 0, NX_NEXTLABS_STANDARD_GRAY);
    m_btnForward.SetImages(IDB_PNG_GOFORWARD, 0, 0, NX_NEXTLABS_STANDARD_GRAY);

    m_ToolTip.AddTool(&m_btnRefresh, IDS_REFRESH_FILE_LIST/*_T("Refresh files list")*/);
	m_btnRefresh.SetImages(IDB_PNG_REFRESH, 0, 0, NX_NEXTLABS_STANDARD_GRAY);
    m_ToolTip.AddTool(&m_btnUpload, IDS_UPLOAD_FILE/*_T("Upload file")*/);
	m_btnUpload.SetImages(IDB_PNG_ADDFILE, IDB_PNG_ADDFILE_DISABLED, 0, NX_NEXTLABS_STANDARD_WHITE);
	m_btnUpload.EnableWindow(TRUE);

 //   ret = m_ToolTip.AddTool(&m_goBack, _T("Go to parent directory"));

    m_ToolTip.Activate(TRUE);

    SetBackColor(NX_NEXTLABS_STANDARD_WHITE);
	m_stcRepoPath.SetWindowTextW(L"");

	RefreshPage();
    return 0;
}


// CRepoPage message handlers

BOOL CRepoPage::PreTranslateMessage(MSG* pMsg)
{
    m_ToolTip.RelayEvent(pMsg);

    return CDialog::PreTranslateMessage(pMsg);
}


void CRepoPage::OnPaint()
{
    CFlatDlgBase::OnPaint();
}


HBRUSH CRepoPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (nCtlColor == CTLCOLOR_LISTBOX)
    {
        return (HBRUSH)GetStockObject(WHITE_BRUSH);
    }
    else if (nCtlColor == CTLCOLOR_EDIT)
    {
        pDC->SetBkColor(RGB(255, 255, 255));
        return (HBRUSH)GetStockObject(WHITE_BRUSH);
    }
    else if (nCtlColor == CTLCOLOR_STATIC)
    {
        if (pWnd->GetDlgCtrlID() == IDC_STATIC_NAVIGATION_BAND)
        {
            return m_hGray;
        } 
        else if (pWnd->GetDlgCtrlID() == IDC_STATIC_REPO_PATH)
        {
            pDC->SetBkColor(NX_NEXTLABS_STANDARD_GRAY);
            return m_hGray;
		}
		else if (pWnd->GetDlgCtrlID() == IDC_STATIC_HIGHTLITE)
		{
			pDC->SetBkColor(NX_NEXTLABS_STANDARD_GREEN);
			return m_hGreen;
		}
		else if (pWnd->GetDlgCtrlID() == IDC_STATIC_CAPTION_NAME || pWnd->GetDlgCtrlID() == IDC_HOME_TEXT)
		{
			pDC->SetTextColor(NX_NEXTLABS_STANDARD_WHITE);
			pDC->SetBkColor(NX_NEXTLABS_STANDARD_GREEN);
			return m_hGreen;
		}
    }

    return  CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);
}


void CRepoPage::OnBnClickedButtonBack()
{

    int idx = m_RepoList.GetCurSel()+1;
	int dataidx = GetReposDataIndex(idx);
	if (dataidx == -1) {
		theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_OCCURRED_IN_OP)/*L"An error occurred while performing this operation."*/);
		return;
	} 
	auto & newPath = theApp.GetParentFolder(m_RepoDataArr[dataidx].nxrmPath);
    auto & dispPath = theApp.GetParentFolder(m_RepoDataArr[dataidx].dispPath);

	m_RepoDataArr[dataidx].nxrmPath = newPath;
	m_RepoDataArr[dataidx].dispPath = dispPath;
	m_stcRepoPath.SetWindowTextW(dispPath.c_str());
	m_btnAddDir.ShowWindow(IsUploadAvailable() ? SW_SHOW : SW_HIDE);
	m_btnUpload.ShowWindow(IsUploadAvailable() ? SW_SHOW : SW_HIDE);
	m_fileList.DeleteAllItems();
    RepoSearchFile(idx, L"");
	AddHistoryInfo(m_RepoDataArr[dataidx]);
	ResetSearch();
}


void CRepoPage::OnIdok()
{
	UpdateData(TRUE);

	if (GetFocus()->GetSafeHwnd() == m_fileList.m_hWnd) {
		if (!IsFileDeleteAvailable() || !IsFileRevokeAvailable())
			return;
		int idx = GetFileListRepoDataIndex();
		if (-1 == idx)
			return;
		RepositoryFileInfo ItemData = m_fileData[idx];
		int nIdx = m_RepoList.GetCurSel() + 1;

		RepoOpenFileOrFolder(ItemData, nIdx);
		return;
	}

	int nIdx = m_RepoList.GetCurSel() + 1;

	CString searchstr = m_strSearch;
	searchstr.Trim();
	if (searchstr.GetLength() == 0)
		return;
	switch (m_selBtnIdx)
	{
	case NX_BTN_FAVORITE:
	case NX_BTN_OFFLINE:
		break;
	case NX_BTN_DELETEFILES:
	case NX_BTN_SHAREFILES:
	case NX_BTN_MYVAULT:
		MyVaultListFiles(searchstr.GetBuffer());
		break;
	case NX_BTN_MYDRIVE:
	case NX_BTN_OTHERREPOS:
		RepoSearchFile(nIdx, searchstr.GetBuffer(), L"", TRUE);
		break;
	default:
		break;
	}

	m_resetSearchBtn.ShowWindow(m_strSearch.GetLength() > 0 ? SW_SHOW : SW_HIDE);
}


void CRepoPage::OnIdcancel()
{
    // TODO: Add your command handler code here
}

int CRepoPage::GetFileListMyVaultDataIndex(int nsel)
{
	if(nsel == -1)
		nsel = m_fileList.GetSelectionMark();
	if (-1 == nsel)
		return -1;
	auto data = (int)m_fileList.GetItemData(nsel);

	data = data >> 16;
	return data;
}

int CRepoPage::GetFileListRepoDataIndex(int nsel)
{
	if (nsel == -1)
		nsel = m_fileList.GetSelectionMark();
	if (-1 == nsel)
		return -1;
	int data = (int)m_fileList.GetItemData(nsel);

	if (-1 == data)
		return data;

	data &= 0xFFFF;
	return data;
}

void CRepoPage::OnClickedButtonEdit()
{
    // TODO: Add your control notification handler code here
    CRepoEditor dlg(CWnd::FromHandle(theApp.m_mainhWnd));
    dlg.DoModal();
}


void CRepoPage::OnDestroy()
{
    CFlatDlgBase::OnDestroy();
	m_RepoList.ReleaseItems();//release self allocated memory
    DeleteObject(m_hGray);
	DeleteObject(m_hGreen);
}


void CRepoPage::DoRefreshRepoFileList(int nSelIdx, BOOL brefresh)
{
	if (nSelIdx >= 0 && nSelIdx <= (int)m_RepositoryArr.size())
	{
		auto typeID = m_RepositoryArr[nSelIdx].Type;
		auto repoName = m_RepositoryArr[nSelIdx].Name;
		m_RepositoryName.SetWindowTextW(repoName.c_str());
		RepoSearchFile(nSelIdx, L"", L"", brefresh);
	}
}

void CRepoPage::OnBnClickedButtonSearch()
{
    m_searchEditBox.SetFocus();
    CString searchText;
    m_searchEditBox.GetWindowText(searchText);
    if(searchText.GetLength())
    {
        OnIdok();
	}
	else if(m_strSearch.Compare(searchText)){//reset search
		OnBnClickedButtonResetSearch();
	}
}

void CRepoPage::UpdateBrowserButton()
{
	m_btnGoBack.ShowWindow(IsBackAvailable()? SW_SHOW: SW_HIDE);
	m_btnForward.ShowWindow(IsForwardAvailable()?SW_SHOW: SW_HIDE);
}

void CRepoPage::AddHistoryInfo(RepositoryData &repodata)
{
	if (m_uHistoryPos != -1 && m_uHistoryPos != m_uHistoryHead) {//the list is not empty
	//compare if the same repository is selected.
		if (0 == repodata.id.compare(m_BrowseHistory[m_uHistoryPos].id) &&
			0 == repodata.dispPath.compare(m_BrowseHistory[m_uHistoryPos].dispPath)) {
			return;
		}
	}
	if (m_uHistoryPos == -1) {
		m_uHistoryPos = m_uHistoryHead;
	}
	else {
		m_uHistoryPos++;
	}
	if (m_uHistoryPos == MAX_BROWSEHISTORY_NUM) {
		m_uHistoryPos = 0;
	}
	m_BrowseHistory[m_uHistoryPos] = repodata;

	if (m_uHistoryHead == m_uHistoryPos && m_uHistoryPos != m_uHistoryTail) {
		m_uHistoryHead++;
		if (m_uHistoryHead == MAX_BROWSEHISTORY_NUM)
			m_uHistoryHead = 0;
	}
	if (m_uHistoryTail != m_uHistoryPos) {
		m_uHistoryTail = m_uHistoryPos;
	}
	UpdateBrowserButton();
}

void CRepoPage::OnNMDblclkListRepoFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here

    if (pNMItemActivate->iItem == -1)
    {
        return;
    }
	int idx = GetFileListRepoDataIndex();
	if (idx == -1) {//go parent folder
		OnBnClickedButtonBack();
		return;
	}
	RepositoryFileInfo ItemData = m_fileData[idx];
    //Combine
	switch (m_selBtnIdx) {
		case NX_BTN_MYVAULT:
		case NX_BTN_SHAREFILES:
		{
			int dataidx = GetFileListMyVaultDataIndex();
			ASSERT(-1 != dataidx);
			if (-1 == dataidx)
				return;
			MyVaultFileInfo mvinfo;
			mvinfo = m_MyVaultFileInfo[dataidx];
			if (mvinfo.bIsDeleted)
				break;
		}
		case NX_BTN_OFFLINE:
		case NX_BTN_FAVORITE:
			OnFilecontextmenuView();
			break;
		case NX_BTN_MYDRIVE:
		case NX_BTN_OTHERREPOS:
			int nIdx = m_RepoList.GetCurSel() + 1;
			RepoOpenFileOrFolder(ItemData, nIdx);
			*pResult = 0;
			break;
	}
           
    return;
}

void CRepoPage::RepoOpenFileOrFolder(RepositoryFileInfo &repoFileInfo, int nIdx)
{
    if (!repoFileInfo.bIsDirectory)
    {
        OnFilecontextmenuView();
    }
    else
    {
        bool isFolder = false;
        std::wstring displayPath;
		int dataidx = GetReposDataIndex(nIdx);
		if (dataidx == -1) {
			theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_AT_OPENING_FILE)/*L"An error occurred while opening the file."*/);
			return;
		}
		m_RepoDataArr[dataidx].nxrmPath = repoFileInfo.strPath;;
		m_RepoDataArr[dataidx].dispPath = repoFileInfo.strDisplayPath;
		m_stcRepoPath.SetWindowTextW(repoFileInfo.strDisplayPath.c_str());
		m_btnAddDir.ShowWindow(IsUploadAvailable() ? SW_SHOW : SW_HIDE);
		m_btnUpload.ShowWindow(IsUploadAvailable() ? SW_SHOW : SW_HIDE);
        m_fileList.DeleteAllItems();
        RepoSearchFile(nIdx, L"");
		RepositoryData rdata = m_RepoDataArr[dataidx];
		AddHistoryInfo(rdata);
		ResetSearch();
	}
}


void CRepoPage::OnFilecontextmenuFileproperties()
{
	int idx;
	int imgIdx;
//	ASSERT(m_selBtnIdx != NX_BTN_MYVAULT);
	switch (m_selBtnIdx)
	{
	case NX_BTN_OTHERREPOS:
	case NX_BTN_MYDRIVE:
		idx = GetFileListRepoDataIndex();
		ASSERT(-1 != idx);
		if (-1 == idx)
			return;
		imgIdx = theApp.GetFileExtensionImageIndex(m_fileData[idx].strFileExtension, m_fileData[idx].bIsNXL);
		theApp.ShowRepositoryFileProperty(GetSafeHwnd(), m_fileData[idx], theApp.GetFileExtensionImageList()->ExtractIconW(imgIdx));
		break;
	case NX_BTN_MYVAULT:
	case NX_BTN_DELETEFILES:
	case NX_BTN_SHAREFILES:
		idx = GetFileListMyVaultDataIndex();
		ASSERT(-1 != idx);
		if (-1 == idx)
			return;
		imgIdx = theApp.GetFileExtensionImageIndex(m_MyVaultFileInfo[idx].rFileInfo.strFileExtension, m_MyVaultFileInfo[idx].rFileInfo.bIsNXL);
		theApp.ShowMyVaultFileProperty(GetSafeHwnd(), m_MyVaultFileInfo[idx], theApp.GetFileExtensionImageList()->ExtractIconW(imgIdx), TRUE);
		break;
	default:
		break;
	}
}


void CRepoPage::OnFilecontextmenuProtect()
{
	int idx = GetFileListRepoDataIndex();
	ASSERT(-1 != idx);
	if (-1 == idx)
		return;
	if (theApp.OpenProtectDlg(GetSafeHwnd(), m_fileData[idx].iRepoIndex, m_fileData[idx]))
		m_bRefreshMyVault = TRUE;
}


void CRepoPage::OnFilecontextmenuShare()
{
	int idx;

	if (m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_SHAREFILES)
	{//MyVault is open.
		idx = GetFileListMyVaultDataIndex();
		ASSERT(-1 != idx);
		if (-1 == idx)
			return;
		MyVaultFileInfo fileinfo;
		fileinfo = m_MyVaultFileInfo[idx];

		int imgIdx = theApp.GetFileExtensionImageIndex(fileinfo.rFileInfo.strFileExtension, fileinfo.rFileInfo.bIsNXL);
		if (theApp.ReShareMyValutFile(GetSafeHwnd(), theApp.GetFileExtensionImageList()->ExtractIconW(imgIdx), fileinfo)) {
			OnBnClickedButtonRefresh();
		}
	}
	else
	{
		idx = GetFileListRepoDataIndex();
		ASSERT(-1 != idx);
		if (-1 == idx)
			return;
		if (theApp.ShareRepositoryFile(GetSafeHwnd(), m_fileData[idx]))
			m_bRefreshMyVault = TRUE;
	}
}

void CRepoPage::InitializeRepositoryList()
{
	m_RepoList.ResetContent();
	m_RepoList.ReleaseItems();
	m_RepoDataArr.clear();

	size_t total_repo = theApp.GetRepositoryArray(m_RepositoryArr);

	m_RepoDataArr.resize(total_repo);

	for (size_t i = 0; i < total_repo; i++)
	{
		auto a = m_RepositoryArr[i];
		if (i != 0) {//skip Mydrive
			m_RepoList.AddItem(a.Name.c_str(), GetRepoIconID(a.Type), GetRepoSelIconID(a.Type), a.AccountName.c_str());
		}
		m_RepositoryArr[i].iDataIndex = (int)i;
		m_RepoDataArr[i].id = a.Id;
		m_RepoDataArr[i].type = a.Type;
		m_RepoDataArr[i].dispPath = L"/";	//Initialize directory for each repo
		m_RepoDataArr[i].nxrmPath = L"/";	//Initialize directory for each repo
	}
}

LRESULT CRepoPage::OnRefreshRepoList(WPARAM wParam, LPARAM lParam)
{
	RefreshRepoList();
	return LRESULT();
}

void CRepoPage::RefreshRepoList()
{
	std::wstring selRepoName = L"";

	if (m_selBtnIdx == NX_BTN_OTHERREPOS) {
		int idx = m_RepoList.GetCurSel();
		selRepoName = m_RepositoryArr[idx+1].Name;
	}
    RefreshRepoOnly();
	if (selRepoName.length() != 0) {//find the selected repository
		for (int i = 1; i < (int)m_RepositoryArr.size(); i++) {
			if (0 == m_RepositoryArr[i].Name.compare(selRepoName)) {
				m_RepoList.SetCurSel(i - 1);
				return;
			}
		}
		GotoRepository(NX_BTN_MYDRIVE);
	}
}

void CRepoPage::RefreshRepoOnly()
{
    size_t total_repo = theApp.GetRepositoryArray(m_RepositoryArr);

	if (0 == (int)total_repo) {
		theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_AT_GETTING_REPO_LIST)/*L"An error occurred while getting the repository list."*/, theApp.LoadString(IDS_ERROR)/*L"Error"*/);
		return;
	}
	m_RepoList.ResetContent();
	m_RepoList.ReleaseItems();
	m_RepositoryArr[0].iDataIndex = 0;//Set MyDrive index
	for (size_t i = 1; i < total_repo; i++)//go through all repositories except MyDrive
    {
        auto a = m_RepositoryArr[i];

		int idx = m_RepoList.AddItem(a.Name.c_str(), GetRepoIconID(a.Type), GetRepoSelIconID(a.Type), a.AccountName.c_str());
		size_t nsearch;
		for (nsearch = 0; nsearch < m_RepoDataArr.size(); nsearch++) {
			if (0 == m_RepoDataArr[nsearch].id.compare(a.Id)) {
				//found entry
				break;
			}
		}
		if (nsearch == m_RepoDataArr.size()) {
			RepositoryData rdata;
			rdata.type = a.Type;
			rdata.id = a.Id;
			rdata.dispPath = L"/";
			rdata.nxrmPath = L"/";
			m_RepoDataArr.push_back(rdata);
		}
		m_RepositoryArr[i].iDataIndex = (int)nsearch;
	}
}

void CRepoPage::ListFavoriteFilesForAllRepos(BOOL brefresh)
{
	theApp.GetFavoriteFileList(m_FavoriteFiles,brefresh);
	UpdateRepoFileList(m_FavoriteFiles);
}

void CRepoPage::ListOfflineFilesForAllRepos()
{
	theApp.GetOfflineFileList(m_OfflineFiles);
	UpdateRepoFileList(m_OfflineFiles);
}

void CRepoPage::ChangeRepo(RepositoryType idx)
{
	if (m_selBtnIdx != NX_BTN_OTHERREPOS)
	{
		m_btnArr[m_selBtnIdx]->SetBtnCheck(BST_UNCHECKED);
		m_btnArr[m_selBtnIdx]->InvalidateRect(NULL);
	}
	if (idx != NX_BTN_OTHERREPOS) {
		m_btnArr[idx]->SetBtnCheck(BST_CHECKED);
		m_btnArr[idx]->InvalidateRect(NULL);
		m_RepoList.SetCurSel(-1);
	}
	if(idx != m_selBtnIdx)//clear the file list if reposiitory is changed.
		m_fileList.DeleteAllItems();

	m_selBtnIdx = idx;

	OnHideAllCtrl(FALSE);

    if(m_selBtnIdx == NX_BTN_MYDRIVE || m_selBtnIdx == NX_BTN_OTHERREPOS)
    { 
		m_fileList.DragAcceptFiles((m_selBtnIdx == NX_BTN_MYDRIVE)?TRUE:FALSE);//disable dragdrop for other repository

		int nIndex = m_RepoList.GetCurSel() + 1;
		int dataidx = GetReposDataIndex(nIndex);
		if (dataidx == -1) {
			theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_OCCURRED_IN_OP)/*L"An error occurred while performing this operation."*/);
			return;
		}
		m_stcRepoPath.SetWindowTextW(m_RepoDataArr[dataidx].dispPath.c_str());
		m_strCurRepoParentPath = m_RepoDataArr[dataidx].nxrmPath;
	}
    else
    {
        m_fileList.DragAcceptFiles(FALSE);
		m_stcRepoPath.SetWindowTextW(L"");
		m_strCurRepoParentPath = L"/";
    }
	
	CString wndText;
    m_btnArr[m_selBtnIdx]->GetWindowText(wndText);
    m_RepositoryName.SetWindowTextW(wndText);


	ResetSearch();
	UpdateData(FALSE);
}

void CRepoPage::ShowAddRepository(void)
{
	if (theApp.OpenAddRepositoryUI()) {
		//No need refresh UI. add other code if needed.
	}
}

void CRepoPage::GotoRepository(RepositoryType rtype, std::wstring reponame /* = L"" */)
{
	switch (rtype) {
	case NX_BTN_MYVAULT:
		OnBnClickedRadioMyvault();
		break;
	case NX_BTN_OFFLINE:
		OnBnClickedRadioOffline();
		break;
	case NX_BTN_MYDRIVE:
		OnBnClickedRadioMydrive();
		break;
	case NX_BTN_FAVORITE:
		OnBnClickedRadioFavorite();
		break;
	case NX_BTN_OTHERREPOS:
		for (size_t i = 1; i < m_RepositoryArr.size(); i++) {//Ignore My drive
			if (0 == reponame.compare(m_RepositoryArr[i].Name)) {
				m_RepoList.SetCurSel((int)(i-1));
				OnLbnSelchangeListRepo();
			}
		}
		break;
	case NX_BTN_SHAREFILES:
		OnClickedRadioSharedfiles();
		break;
	case NX_BTN_DELETEFILES:
		OnClickedRadioDeletefiles();
		break;
	}
}

void CRepoPage::OnLbnSelchangeListRepo()
{
	// TODO: Add your control notification handler code here
	OnBnClickedRadioAllFiles();
}

void CRepoPage::OnBnClickedRadioAllFiles()
{
    // TODO: Add your control notification handler code here
    ChangeRepo(NX_BTN_OTHERREPOS);
	int nSelIdx = m_RepoList.GetCurSel() + 1;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	DoRefreshRepoFileList(nSelIdx, m_bRefreshFileList);
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	m_bRefreshFileList = FALSE;
	RepositoryData rdata;
	int dataidx = GetReposDataIndex(nSelIdx);
	if (dataidx == -1) {
		theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_OCCURRED_IN_OP)/*L"An error occurred while performing this operation."*/);
		return;
	}
	rdata = m_RepoDataArr[dataidx];
	AddHistoryInfo(rdata);
}


void CRepoPage::OnBnClickedRadioFavorite()
{
    // TODO: Add your control notification handler code here
    ChangeRepo(NX_BTN_FAVORITE);
	if(!m_bRefreshFileList && !m_bRefreshMyFavorite)
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
    ListFavoriteFilesForAllRepos(m_bRefreshFileList || m_bRefreshMyFavorite);
	if (!m_bRefreshFileList && !m_bRefreshMyFavorite)
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	m_bRefreshFileList = FALSE;
	m_bRefreshMyFavorite = FALSE;
	RepositoryData rdata;
	rdata.id = FAVORITE_IDSTRING;
	rdata.dispPath = L"";
	rdata.nxrmPath = L"";
	AddHistoryInfo(rdata);
}

void CRepoPage::OnBnClickedRadioOffline()
{
    // TODO: Add your control notification handler code here
    ChangeRepo(NX_BTN_OFFLINE);
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	ListOfflineFilesForAllRepos();
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	RepositoryData rdata;
	rdata.id = OFFLINE_IDSTRING;
	rdata.dispPath = L"";
	rdata.nxrmPath = L"";
	AddHistoryInfo(rdata);
}


void CRepoPage::OnBnClickedRadioMyvault()
{
	ChangeRepo(NX_BTN_MYVAULT);
	m_iCurSelectedListColumn = -1;
	m_bColumnAscend = TRUE;
	RefreshSearchList(); //alway refresh MyVault

	RepositoryData rdata;
	rdata.id = MYVALUT_IDSTRING;
	rdata.dispPath = L"";
	rdata.nxrmPath = L"";
	AddHistoryInfo(rdata);
}

void CRepoPage::OnBnClickedRadioMydrive()
{
	if (m_RepoDataArr.size() == 0)
		return;
    ChangeRepo(NX_BTN_MYDRIVE);
    RepoSearchFile(0, L"", L"", m_bRefreshFileList);
	m_bRefreshFileList = FALSE;
	RepositoryData rdata;
	rdata = m_RepoDataArr[0];
	AddHistoryInfo(rdata);
}

void CRepoPage::OnFilecontextmenuDelete()
{
	// TODO: Add your command handler code here
	int idx;
	RepositoryFileInfo repoFileInfo;
	MyVaultFileInfo mvFileInfo;
	wstringstream txtOut;
	std::wstring filename;
	std::wstring filedate;
	std::wstring filesize;
	BOOL bdirectory;
	BOOL bfavorite;
	if (m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_SHAREFILES) {
		idx = GetFileListMyVaultDataIndex();
		ASSERT(-1 != idx);
		if (-1 == idx)
			return;
		mvFileInfo = m_MyVaultFileInfo[idx];

		filename = mvFileInfo.rFileInfo.strFileName;
		filedate = mvFileInfo.rFileInfo.strFileDate;
		filesize = mvFileInfo.rFileInfo.strFileSize;
		bdirectory = mvFileInfo.rFileInfo.bIsDirectory;
		bfavorite = mvFileInfo.rFileInfo.bIsFavorite;
	}
	else {
		idx = GetFileListRepoDataIndex();
		ASSERT(-1 != idx);
		if (-1 == idx)
			return;
		repoFileInfo = m_fileData[idx];
		filename = repoFileInfo.strFileName;
		filedate = repoFileInfo.strFileDate;
		filesize = repoFileInfo.strFileSize;
		bdirectory = repoFileInfo.bIsDirectory;
		bfavorite = repoFileInfo.bIsFavorite;
	}

	CDeleteDlg dlg(bdirectory, CWnd::FromHandle(GetSafeHwnd()));

	dlg.m_strFileName = filename.c_str();
	dlg.m_strFileDate = filedate.c_str();
	dlg.m_strFileSize = filesize.c_str();

	if (dlg.DoModal() == IDCANCEL) {
		return;
	}

	switch (m_selBtnIdx) {
	case NX_BTN_MYVAULT:
	case NX_BTN_SHAREFILES:
		if (!theApp.DeleteMyValutFile(mvFileInfo))
			return;
		break;
	case NX_BTN_FAVORITE:
		if (!theApp.DeleteFavoriteFile(repoFileInfo))
			return;
		break;
	case NX_BTN_OFFLINE:
		for (size_t s = 0; s < m_OfflineFiles.size(); s++) {
			if (m_OfflineFiles[s].first == repoFileInfo.iRepoIndex) {
				std::vector<RepositoryFileInfo>::iterator iter;
				for (iter = m_OfflineFiles[s].second.begin(); iter != m_OfflineFiles[s].second.end(); iter++) {
					if (iter->strFileName.compare(repoFileInfo.strFileName) == 0) {
						m_OfflineFiles[s].second.erase(iter);
						break;
					}
				}
			}
		}
		if (theApp.SetFileToOffline(m_OfflineFiles)) {
			UpdateRepoFileList(m_OfflineFiles);
		}
		break;
	case NX_BTN_MYDRIVE:
	case NX_BTN_OTHERREPOS:
		idx = m_RepoList.GetCurSel() + 1;
		if (bdirectory) {
			if (!theApp.DeleteRepositoryFolder(idx, repoFileInfo.strPath, repoFileInfo.strFileName))
				return;
		}
		else {
			if (!theApp.DeleteRepositoryFile(idx, repoFileInfo.strPath, repoFileInfo.strFileName))
				return;
		}
		break;
	}
	if (bfavorite)
		m_bRefreshMyFavorite = TRUE;
	m_bRefreshFileList = TRUE;
	OnBnClickedButtonRefresh();
}

void CRepoPage::OnFilecontextmenuDownload()
{
    // TODO: Add your command handler code here
	RepositoryFileInfo fileinfo;
	int idx = GetFileListRepoDataIndex();
	ASSERT(-1 != idx);
	if (-1 == idx)
		return;
	fileinfo = m_fileData[idx];

    CFileDialog fileDialog(FALSE,
        NULL,
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        L"All types(*.*)|*.*||",
        this
    );

    OPENFILENAME& ofn = fileDialog.GetOFN();
    wchar_t fileName[2*MAX_PATH];
	wchar_t fileext[MAX_PATH];
	memset(fileName, 0, sizeof(fileName));
	memset(fileext, 0, sizeof(fileext));
    lstrcpy(fileName, fileinfo.strFileName.c_str());
	if (fileinfo.bIsNXL)
		lstrcpy(fileext, L"nxl");
	else if(fileinfo.strFileExtension.length() > 1)
		lstrcpy(fileext, fileinfo.strFileExtension.c_str() + 1);
    ofn.lpstrFile = fileName;
	ofn.lpstrDefExt = fileext;
    if (fileDialog.DoModal() == IDCANCEL)
    {
        return;
    }
    CString fileToSave = fileDialog.GetFileName();
	if (fileinfo.bIsNXL && fileToSave.MakeLower().Find(L".nxl", fileToSave.GetLength()-4) == -1) {
		fileToSave.Append(L".nxl");//append nxl extension if it is not there.
	}
	m_strDownloadFullPath = fileDialog.GetPathName();

	switch (m_selBtnIdx) {
	case NX_BTN_MYVAULT:
	case NX_BTN_SHAREFILES:
		theApp.DownloadMyValutFile(fileinfo, m_strDownloadFullPath);
		break;
	case NX_BTN_FAVORITE:
		theApp.DownloadRepositoryFile(fileinfo.iRepoIndex, fileinfo.strPath, m_strDownloadFullPath, fileinfo.bIsNXL);
		break;
	case NX_BTN_MYDRIVE:
	case NX_BTN_OTHERREPOS:
		theApp.DownloadRepositoryFile(fileinfo.iRepoIndex, fileinfo.strPath, m_strDownloadFullPath, fileinfo.bIsNXL);
		break;
	default:
		ASSERT(TRUE);
	}
}

void CRepoPage::OnFilecontextmenuView()
{
	int idx = GetFileListRepoDataIndex();
	ASSERT(-1 != idx);
	if (-1 == idx)
		return;
	RepositoryFileInfo *pItemData = &m_fileData[idx];
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	switch (m_selBtnIdx) {
	case NX_BTN_MYVAULT:
	case NX_BTN_SHAREFILES:
		theApp.ViewMyVaultFile(pItemData->strPath, pItemData->strFileName);
		break;
	case NX_BTN_MYDRIVE:
	case NX_BTN_OTHERREPOS:
	case NX_BTN_FAVORITE:
		theApp.ViewRepositoryFile(pItemData->iRepoIndex, pItemData->strPath, pItemData->strFileName);
		break;
	case NX_BTN_OFFLINE:
		theApp.ViewLocalFile(*pItemData);
		break;
	}
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CRepoPage::UpdateFileListData(void)
{
	if (m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_DELETEFILES || m_selBtnIdx == NX_BTN_SHAREFILES) {
		for (size_t s = 0; s < m_MyVaultFileInfo.size(); s++) {
			m_MyVaultFileInfo[s].rFileInfo.bIsOffline = IsOfflineFile(m_MyVaultFileInfo[s].rFileInfo);
			m_MyVaultFileInfo[s].rFileInfo.bIsFavorite = IsFavoriteFile(m_MyVaultFileInfo[s].rFileInfo);
		}
	}
	switch (m_selBtnIdx)
	{
	case NX_BTN_FAVORITE:
		break;
	case NX_BTN_MYVAULT:
		UpdateMyVaultFileList(m_MyVaultFileInfo);
		break;
	case NX_BTN_DELETEFILES:
		UpdateMyVaultFileList(m_MyVaultFileInfo, MVF_DELETEFILES);
		break;
	case NX_BTN_SHAREFILES:
		UpdateMyVaultFileList(m_MyVaultFileInfo, MVF_SHAREDFILES);
		break;
	case NX_BTN_MYDRIVE:
		break;
	case NX_BTN_OTHERREPOS:
		break;
	}
}
void CRepoPage::OnBnClickedButtonRefresh()
{
	switch (m_selBtnIdx)
    {
    case NX_BTN_FAVORITE:
		m_bRefreshFileList = TRUE;
		OnBnClickedRadioFavorite();
        break;
    case NX_BTN_OFFLINE:
		OnBnClickedRadioOffline();
        break;
    case NX_BTN_MYVAULT:
		m_bRefreshMyVault = TRUE;
		OnBnClickedRadioMyvault();
		theApp.RefreshHomePageMyDriveUsage();
		break;
	case NX_BTN_DELETEFILES:
		m_bRefreshMyVault = TRUE;
		OnClickedRadioDeletefiles();
		break;
	case NX_BTN_SHAREFILES:
		m_bRefreshMyVault = TRUE;
		OnClickedRadioSharedfiles();
		break;
    case NX_BTN_MYDRIVE:
		m_bRefreshFileList = TRUE;
		OnBnClickedRadioMydrive();
		theApp.RefreshHomePageMyDriveUsage();
		break;
	case NX_BTN_OTHERREPOS:
		m_bRefreshFileList = TRUE;
		OnBnClickedRadioAllFiles();
        break;
    }
}

void CRepoPage::DropOneFile(CString folderToSave, CString fileToSave)
{

	int nSelIdx = m_RepoList.GetCurSel() + 1;
	int dataidx = GetReposDataIndex(nSelIdx);
	if (dataidx == -1) {
		theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_AT_UPLOADING_FILE)/*L"An error occurred while uploading the file."*/);
		return;
	}
	auto curPath = m_RepoDataArr[dataidx].dispPath;
	curPath += curPath[curPath.size() - 1] == L'/' ? L"" : L"/"; //appending / at the end.
	std::wstring fullpath = folderToSave.GetBuffer() + std::wstring(L"\\") + fileToSave.GetBuffer();
	std::wstring targetname = m_RepoDataArr[dataidx].nxrmPath;
	if (targetname.find_last_of(L'/') != (targetname.size() -1)) {
		targetname += L"/";
	}
	targetname += fileToSave.GetBuffer();
	BOOL bProtect = FALSE;
	BOOL bisnxl = FALSE;
	if (!PathFileExists(fullpath.c_str())) {
		MessageBox(theApp.LoadString(IDS_FILE_NOT_FOUND)/*L"file not found"*/, theApp.LoadString(IDS_ERROR)/*L"Error"*/, MB_OK);
		return;
	}

	theApp.GetFileExtension(fileToSave.GetBuffer(), &bisnxl);
	if (!bisnxl) {
		CUploadFileDlg dlg(CWnd::FromHandle(GetSafeHwnd()));
		dlg.m_strFileName = fileToSave;
		dlg.m_strPath = curPath.c_str();
		INT_PTR intptr = dlg.DoModal();
		if (intptr == IDOK) {
			bProtect = TRUE;
		}
		else if (intptr == IDCANCEL) {
			if (!dlg.m_bUpdateOnly)//user closed the dialog.
				return;
		}
	}


	MSG wmmes;
	wmmes.hwnd = m_btnRefresh.GetSafeHwnd();
	wmmes.message = BM_CLICK;
	wmmes.wParam = 0;
	wmmes.lParam = 0;
	if (theApp.UploadFileToRepository(GetSafeHwnd(), dataidx, fullpath, targetname, bProtect, ProgressCallBack, &wmmes)) {
		m_bRefreshMyVault = TRUE;//need refresh myVault after protect file.
	}
}

void CRepoPage::OnBnClickedButtonUpload()
{
    // TODO: Add your control notification handler code here
    CFileDialog fileDialog(TRUE,
        NULL,
        NULL,
        OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		L"All types(*.*)|*.*||",
        this
        );

    OPENFILENAME& ofn = fileDialog.GetOFN();
    if (fileDialog.DoModal() == IDCANCEL)
    {
        return;
    }
    CString fileToSave = fileDialog.GetFileName();
	CString fullSavePath = fileDialog.GetPathName();
	CString strtemp = L"\\" + fileToSave;
	CString folderToSave = fullSavePath;
	folderToSave.Replace(strtemp, _T(""));
	DropOneFile(folderToSave, fileToSave);
}


void CRepoPage::OnBnClickedButtonResetSearch()
{
	int nIdx;
	switch (m_selBtnIdx)
	{
		break;
	case NX_BTN_FAVORITE:
		break;
	case NX_BTN_OFFLINE:
		break;
	case NX_BTN_MYVAULT:
	case NX_BTN_DELETEFILES:
	case NX_BTN_SHAREFILES:
		MyVaultListFiles(L"");
		break;
	case NX_BTN_MYDRIVE:
	case NX_BTN_OTHERREPOS:
		nIdx = m_RepoList.GetCurSel() + 1;
		RepoSearchFile(nIdx, L"");
		break;
	default:
		break;
	}
    ResetSearch();
}

void CRepoPage::ResetSearch()
{
    m_searchEditBox.SetWindowTextW(L"");
    m_resetSearchBtn.ShowWindow(SW_HIDE);
	m_strSearch = L"";
	UpdateData(FALSE);
}

void CRepoPage::RefreshSearchList()
{
	UpdateData(TRUE);
	int nIdx = m_RepoList.GetCurSel() + 1;
	int dataidx = GetReposDataIndex(nIdx);
	if (dataidx == -1) {
		theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_AT_REFRESHING)/*L"An error occurred while refreshing the file list."*/);
		return;
	}
	switch (m_selBtnIdx)
    {
    case NX_BTN_MYVAULT:
	case NX_BTN_SHAREFILES:
	case NX_BTN_DELETEFILES:
        MyVaultListFiles(m_strSearch.GetBuffer());
	    break;
    case NX_BTN_FAVORITE:
        ListFavoriteFilesForAllRepos(TRUE);
        break;
    case NX_BTN_OFFLINE:
        ListOfflineFilesForAllRepos();
        break;
	case NX_BTN_MYDRIVE:
	case NX_BTN_OTHERREPOS:
		RepoSearchFile(nIdx, m_strSearch.GetBuffer(), m_RepoDataArr[dataidx].nxrmPath, TRUE);
		break;
    }
}

struct CompareStruct
{
	CNxMFCListCtrl *pList; // pointer to list
	int iColumn;//column to compare
	BOOL bAscend;
	vector <RepositoryFileInfo> * pFiledata;
	vector <MyVaultFileInfo> *pMVFileData;
};

static int CALLBACK
MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if (lParam1 == lParam2)
		return 0;
	// lParamSort contains a pointer to the list view control.
	CompareStruct * pcs = (CompareStruct *)lParamSort;

	int nItem1, nItem2;
	LVFINDINFO lvfi;
	lvfi.flags = LVFI_PARAM;
	CNxMFCListCtrl* pListCtrl = pcs->pList;

	// find indexes of our items
	lvfi.lParam = lParam1;
	nItem1 = pListCtrl->FindItem(&lvfi);
	lvfi.lParam = lParam2;
	nItem2 = pListCtrl->FindItem(&lvfi);


	int nidx1 = (int)pListCtrl->GetItemData(nItem1);
	int nidx2 = (int)pListCtrl->GetItemData(nItem2);

	if (nidx1 == -1)
		return -1;
	if (nidx2 == -1)
		return 1;
	int iret = 0;
	if (NULL != pcs->pFiledata) {
		if (pcs->pFiledata->at(nidx1).bIsDirectory && !pcs->pFiledata->at(nidx2).bIsDirectory) {
			iret = -1;
		}
		else if (!pcs->pFiledata->at(nidx1).bIsDirectory && pcs->pFiledata->at(nidx2).bIsDirectory) {
			iret = 1;
		}
		else if (pcs->pFiledata->at(nidx1).bIsDirectory && pcs->pFiledata->at(nidx2).bIsDirectory) {
			switch (pcs->iColumn)
			{
			case 1://Compare Date
				if (pcs->pFiledata->at(nidx1).lFileDate > pcs->pFiledata->at(nidx2).lFileDate)
					iret = 1;
				else if (pcs->pFiledata->at(nidx1).lFileDate < pcs->pFiledata->at(nidx2).lFileDate)
					iret = -1;
				else
					iret = 0;
				break;
			case 0://Compare FileName;
			default:
				iret = _wcsicmp(pcs->pFiledata->at(nidx1).strFileName.c_str(), pcs->pFiledata->at(nidx2).strFileName.c_str());
				break;
			}
		}
		else {
			switch (pcs->iColumn)
			{
			case 0://Compare FileName;
				iret = _wcsicmp(pcs->pFiledata->at(nidx1).strFileName.c_str(), pcs->pFiledata->at(nidx2).strFileName.c_str());
				break;
			case 1://Compare Date
				if (pcs->pFiledata->at(nidx1).lFileDate > pcs->pFiledata->at(nidx2).lFileDate)
					iret = 1;
				else if (pcs->pFiledata->at(nidx1).lFileDate < pcs->pFiledata->at(nidx2).lFileDate)
					iret = -1;
				else
					iret = 0;
				break;
			case 2://Compare Type
				iret = _wcsicmp(pcs->pFiledata->at(nidx1).strFileExtension.c_str(),pcs->pFiledata->at(nidx2).strFileExtension.c_str());
				break;
			case 3://Compare Size
				if (pcs->pFiledata->at(nidx1).iFileSize > pcs->pFiledata->at(nidx2).iFileSize)
					iret = 1;
				else if (pcs->pFiledata->at(nidx1).iFileSize < pcs->pFiledata->at(nidx2).iFileSize)
					iret = -1;
				else
					iret = 0;
				break;
			case 4://Compare Repository
				if (pcs->pFiledata->at(nidx1).iRepoIndex > pcs->pFiledata->at(nidx2).iRepoIndex)
					iret = 1;
				else if (pcs->pFiledata->at(nidx1).iRepoIndex < pcs->pFiledata->at(nidx2).iRepoIndex)
					iret = -1;
				else
					iret = 0;
				break;
			default:
				iret = 0;//do nothing
			}
		}
	}
	else if (NULL != pcs->pMVFileData) {
		nidx1 = nidx1 >> 16;
		nidx2 = nidx2 >> 16;
		switch (pcs->iColumn)
		{
		case 0://Compare FileName;
			iret = _wcsicmp(pcs->pMVFileData->at(nidx1).rFileInfo.strFileName.c_str(), pcs->pMVFileData->at(nidx2).rFileInfo.strFileName.c_str());
			break;
		case 1://Compare Shared With
			iret = theApp.SerilaizeEmail(pcs->pMVFileData->at(nidx1).strSharedArr).compare(theApp.SerilaizeEmail(pcs->pMVFileData->at(nidx2).strSharedArr));
			break;
		case 2://Compare Data
			if (pcs->pMVFileData->at(nidx1).rFileInfo.lFileDate > pcs->pMVFileData->at(nidx2).rFileInfo.lFileDate)
				iret = 1;
			else if (pcs->pMVFileData->at(nidx1).rFileInfo.lFileDate < pcs->pMVFileData->at(nidx2).rFileInfo.lFileDate)
				iret = -1;
			else
				iret = 0;
			break;
		default:
			iret = 0;//do nothing
		}
	}


	if (!pcs->bAscend)
		iret *= (-1);
	return iret;
}

void CRepoPage::SortColumn(int columnindex, BOOL bRefresh)
{
	if (columnindex > 4)
		return;
	if (m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_DELETEFILES || m_selBtnIdx == NX_BTN_SHAREFILES) {
		if (columnindex == 1)//don't have function to sort sharedwith field.
			return;
	}

	if (m_iCurSelectedListColumn != columnindex || bRefresh)
		m_bColumnAscend = TRUE;
	else
		m_bColumnAscend = !m_bColumnAscend;

	m_iCurSelectedListColumn = columnindex;
	m_fileList.SetSortColumn(columnindex, m_bColumnAscend);
	CompareStruct cs;
	cs.pList = &m_fileList;
	cs.iColumn = columnindex;
	cs.bAscend = m_bColumnAscend;
	if (m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_DELETEFILES || m_selBtnIdx == NX_BTN_SHAREFILES) {
		MyVaultListFiles(m_strSearch.GetBuffer());
		return;
	}
	else {
		cs.pFiledata = &m_fileData;
		cs.pMVFileData = NULL;
	}
	m_fileList.SortItems(MyCompareProc, (LPARAM)&cs);
}

void CRepoPage::OnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	SortColumn(pNMLV->iSubItem);
}

void CRepoPage::PreSubclassWindow()
{
    // TODO: Add your specialized code here and/or call the base class

    CFlatDlgBase::PreSubclassWindow();
    //
}


void CRepoPage::OnFilecontextmenuFavorite()
{
	int idx = GetFileListRepoDataIndex();
	ASSERT(-1 != idx);
	if (-1 == idx)
		return;
	RepositoryFileInfo repoFileInfo = m_fileData[idx];

	int nIdx = m_RepoList.GetCurSel() + 1;
	if (m_selBtnIdx == 4) {
		nIdx = 0;
	}
	if (theApp.SetFileToFavorite(nIdx, m_strCurRepoParentPath, repoFileInfo)) {
		m_fileData[idx].bIsFavorite = TRUE;
		theApp.GetFavoriteFileList(m_FavoriteFiles, TRUE);
	}
		
}

void CRepoPage::OnFilecontextmenuOffline()
{
	int idx = GetFileListRepoDataIndex();
	ASSERT(-1 != idx);
	if (-1 == idx)
		return;
	RepositoryFileInfo repoFileInfo = m_fileData[idx];
	repoFileInfo.bIsOffline = TRUE;

	int repoidx = m_RepoList.GetCurSel() + 1;

	std::wstring temppath;
	BOOL bsuccessed = FALSE;
	temppath = theApp.GenerateOfflineFilePath(repoFileInfo);
	if (theApp.DownloadRepositoryFile(repoidx, repoFileInfo.strPath, temppath, repoFileInfo.bIsNXL)) {
		BOOL bFound = FALSE;
		for (size_t s = 0; s < m_OfflineFiles.size(); s++) {
			if (m_OfflineFiles[s].first == repoidx) {
				m_OfflineFiles[s].second.push_back(repoFileInfo);
				bFound = TRUE;
				if (theApp.SetFileToOffline(m_OfflineFiles)) {
					bsuccessed = TRUE;
				}
				else {
					m_OfflineFiles[s].second.pop_back();
				}
				break;
			}
		}
		if (!bFound) {//insert a new node
			vector<RepositoryFileInfo> filelist;
			filelist.push_back(repoFileInfo);
			m_OfflineFiles.push_back(make_pair(repoidx, filelist));
			if (theApp.SetFileToOffline(m_OfflineFiles)) {
				bsuccessed = TRUE;
			}
			else {
				m_OfflineFiles.clear();
			}
		}

	}
	if (bsuccessed) {//Update local offline information.
		m_fileData[idx].bIsOffline = TRUE;
		if (m_selBtnIdx == NX_BTN_MYVAULT) {
			idx = GetFileListMyVaultDataIndex();
			ASSERT(-1 != idx);
			if (-1 == idx)
				return;
			m_MyVaultFileInfo[idx].rFileInfo.bIsOffline = TRUE;
		}
	}

}

BOOL CRepoPage::GotoHistoryPosition()
{
	auto id = m_BrowseHistory[m_uHistoryPos].id;
	if (id.compare(OFFLINE_IDSTRING) == 0) {
		ChangeRepo(NX_BTN_OFFLINE);
		ListOfflineFilesForAllRepos();
	}
	else if (id.compare(FAVORITE_IDSTRING) == 0) {
		ChangeRepo(NX_BTN_FAVORITE);
		ListFavoriteFilesForAllRepos();
	}
	else if (id.compare(MYVALUT_IDSTRING) == 0) {
		ChangeRepo(NX_BTN_MYVAULT);
		UpdateMyVaultFileList(m_MyVaultFileInfo);
	}
	else if (id.compare(DELETEDFILE_IDSTRING) == 0) {
		ChangeRepo(NX_BTN_DELETEFILES);
		UpdateMyVaultFileList(m_MyVaultFileInfo, MVF_DELETEFILES);
	}
	else if (0 == id.compare(SHAREDFILE_IDSTRING)) {
		ChangeRepo(NX_BTN_SHAREFILES);
		UpdateMyVaultFileList(m_MyVaultFileInfo, MVF_SHAREDFILES);
	}
	else {
		BOOL bfound = FALSE;
		for (int i = 0; i < (int)m_RepositoryArr.size(); i++) {
			if (m_RepositoryArr[i].Id.compare(id) == 0) {
				int dataidx = GetReposDataIndex(i);
				if (dataidx == -1) {
					theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_AT_SWITCHING_BROWSING)/*L"An error occurred while switching browsing history."*/);
					return FALSE;
				}
				auto & newPath = m_BrowseHistory[m_uHistoryPos].nxrmPath;
				auto & dispPath = m_BrowseHistory[m_uHistoryPos].dispPath;

				m_RepoDataArr[dataidx].nxrmPath = newPath;
				m_RepoDataArr[dataidx].dispPath = dispPath;
				if (i == 0) {
					ChangeRepo(NX_BTN_MYDRIVE);
					RepoSearchFile(0, L"");
				}
				else {
					m_RepoList.SetCurSel(i - 1);
					ChangeRepo(NX_BTN_OTHERREPOS);
					auto typeID = m_RepositoryArr[i].Type;
					auto repoName = m_RepositoryArr[i].Name;
					m_RepositoryName.SetWindowTextW(repoName.c_str());
					m_stcRepoPath.SetWindowTextW(m_RepoDataArr[dataidx].dispPath.c_str());
					RepoSearchFile(i, L"", newPath);
				}
				m_btnAddDir.ShowWindow(IsUploadAvailable() ? SW_SHOW : SW_HIDE);
				m_btnUpload.ShowWindow(IsUploadAvailable() ? SW_SHOW : SW_HIDE);
				bfound = TRUE;
				break;
			}
		}
		if (!bfound)
			return FALSE;
	}
	return TRUE;
}

void CRepoPage::OnClickedButtonGoback()
{
	if (m_uHistoryHead == m_uHistoryPos) {//to prevent infinite loop when all history are not available.
		UpdateBrowserButton();
		return;
	}

	if (m_uHistoryPos == 0) {
		m_uHistoryPos = MAX_BROWSEHISTORY_NUM -1;
	}
	else {
		m_uHistoryPos--;
	}

	if (!GotoHistoryPosition()) {
		OnClickedButtonGoback();
	}
	UpdateBrowserButton();
}

void CRepoPage::OnClickedButtonForward()
{
	if (m_uHistoryTail == m_uHistoryPos) {//to prevent infinite loop when all history are not available.
		UpdateBrowserButton();
		return;
	}

	m_uHistoryPos ++;

	if (m_uHistoryPos == MAX_BROWSEHISTORY_NUM) {
		m_uHistoryPos = 0;
	}

	if (!GotoHistoryPosition()) {
		OnClickedButtonForward();
	}

	UpdateBrowserButton();
}

void CRepoPage::OnBnClickedButtonNewdir()
{
	NewFolderDlg dlg(CWnd::FromHandle(GetSafeHwnd()));
	for (size_t st = 0; st < m_fileData.size(); st++) {
		if (m_fileData[st].bIsDirectory) {
			dlg.m_strExistFolders.push_back(m_fileData[st].strFileName);
		}
	}
	if (dlg.DoModal() != IDOK)
		return;

	int repoidx = m_RepoList.GetCurSel() + 1;
	int dataidx = GetReposDataIndex(repoidx);
	if (dataidx == -1) {
		theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_AT_CREATING_DIR)/*L"An error occurred while creating the directory."*/);
		return;
	}
	if (theApp.CreateRepositoryFolder(repoidx, m_RepoDataArr[dataidx].nxrmPath, dlg.m_strFolderName.GetBuffer())) {
		OnBnClickedButtonRefresh();
	}
}


void CRepoPage::OnRclickListRepofiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	POINT pt;
	int idx = pNMItemActivate->iItem;;
	if (idx == -1)
	{
		return;
	}


	BOOL bdirectory;
	BOOL bnxl;
	BOOL boffline;
	BOOL bfavorite;
	BOOL bdelete;
	BOOL brevoked;
	BOOL bprotected;
	BOOL bdisableUploadAddDir = FALSE;
	switch (m_selBtnIdx) {
	case NX_BTN_MYVAULT:
	case NX_BTN_SHAREFILES:
		{
			idx = GetFileListMyVaultDataIndex(idx);
			ASSERT(-1 != idx);
			if (-1 == idx)
				return;
			MyVaultFileInfo fileinfo;
			fileinfo = m_MyVaultFileInfo[idx];
			bdirectory = fileinfo.rFileInfo.bIsDirectory;
			bnxl = fileinfo.rFileInfo.bIsNXL;
			boffline = fileinfo.rFileInfo.bIsOffline;
			bfavorite = fileinfo.rFileInfo.bIsFavorite;
			bdelete = fileinfo.bIsDeleted;
			bprotected = !fileinfo.bIsShared;
			brevoked = fileinfo.bIsRevoked;
		}
		break;
	case NX_BTN_OTHERREPOS:
	case NX_BTN_FAVORITE:
	case NX_BTN_OFFLINE:
	case NX_BTN_MYDRIVE:
		{
			idx = GetFileListRepoDataIndex(idx);
			if (-1 == idx)//go parent folder.
				return;
			RepositoryFileInfo file = m_fileData[idx];
			bdirectory = file.bIsDirectory;
			bnxl = file.bIsNXL;
			boffline = file.bIsOffline;
			bfavorite = file.bIsFavorite;
			bdelete = FALSE;
			brevoked = FALSE;
			bprotected = FALSE;
		}
		break;
	case NX_BTN_DELETEFILES:
		bdelete = TRUE;
		break;
	}

	CMenu menu;
	menu.LoadMenuW(IDR_FILE_CONTEXT_MENU);
	auto pMenu = menu.GetSubMenu(0);
	CString tempStr;

	if (bdelete) {
		pMenu->RemoveMenu(ID_FILECONTEXTMENU_VIEW, MF_BYCOMMAND);
		pMenu->RemoveMenu(ID_FILECONTEXTMENU_FAVORITE, MF_BYCOMMAND);
		pMenu->RemoveMenu(ID_FILECONTEXTMENU_OFFLINE, MF_BYCOMMAND);
		pMenu->RemoveMenu(ID_FILECONTEXTMENU_DOWNLOAD, MF_BYCOMMAND);
		pMenu->RemoveMenu(1, MF_BYPOSITION);
		pMenu->RemoveMenu(ID_FILECONTEXTMENU_PROTECT, MF_BYCOMMAND);
		pMenu->RemoveMenu(ID_FILECONTEXTMENU_SHARE, MF_BYCOMMAND);
		MENUITEMINFO minfo;
		minfo.cbSize = sizeof(MENUITEMINFO);
		minfo.fMask = MIIM_STRING;
		tempStr = theApp.LoadString(IDS_DETAILS);
		minfo.dwTypeData = tempStr.GetBuffer()/*L"Details"*/;
		pMenu->SetMenuItemInfoW(ID_FILECONTEXTMENU_MANAGE, &minfo);
		pMenu->RemoveMenu(ID_FILECONTEXTMENU_DELETE, MF_BYCOMMAND);
		pMenu->RemoveMenu(3, MF_BYPOSITION);
	}
	else {
		//remove make Available offline & favorite for current release.
		pMenu->RemoveMenu(ID_FILECONTEXTMENU_OFFLINE, MF_BYCOMMAND);
		pMenu->RemoveMenu(ID_FILECONTEXTMENU_FAVORITE, MF_BYCOMMAND);

		if (!IsFileInfoAvailable())
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_FILEPROPERTIES, MF_BYCOMMAND);
		if (!IsProtectAvailable())
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_PROTECT, MF_BYCOMMAND);
		if (!IsShareAvailable())
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_SHARE, MF_BYCOMMAND);


		if (m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_SHAREFILES) {//special handle for MyVault
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_FAVORITE, MF_BYCOMMAND);
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_OFFLINE, MF_BYCOMMAND);
			if (bprotected )
				pMenu->RemoveMenu(ID_FILECONTEXTMENU_MANAGE, MF_BYCOMMAND);
			else if (brevoked) {
				MENUITEMINFO minfo;
				minfo.cbSize = sizeof(MENUITEMINFO);
				minfo.fMask = MIIM_STRING;
				tempStr = theApp.LoadString(IDS_DETAILS);
				minfo.dwTypeData = tempStr.GetBuffer()/*L"Details"*/;
				pMenu->SetMenuItemInfoW(ID_FILECONTEXTMENU_MANAGE, &minfo);
			}
		}
		else if (m_selBtnIdx == NX_BTN_OTHERREPOS) {
			if (bdirectory)//no menu for directory at other repository
				return;
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_VIEWACTIVITY, MF_BYCOMMAND);
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_MANAGE, MF_BYCOMMAND);
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_FAVORITE, MF_BYCOMMAND);
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_OFFLINE, MF_BYCOMMAND);
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_DOWNLOAD, MF_BYCOMMAND);
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_DELETE, MF_BYCOMMAND);
			if (!bnxl) {
				pMenu->RemoveMenu(ID_FILECONTEXTMENU_VIEW, MF_BYCOMMAND);
				pMenu->RemoveMenu(3, MF_BYPOSITION);
				pMenu->RemoveMenu(0, MF_BYPOSITION);
			}
			else {
				pMenu->RemoveMenu(4, MF_BYPOSITION);
			}

		}
		else//for MyDrive, deletedfiles and others
		{
			if(!bnxl || m_selBtnIdx != NX_BTN_MYDRIVE)//show activity log menu for nxl file in myDrive
				pMenu->RemoveMenu(ID_FILECONTEXTMENU_VIEWACTIVITY, MF_BYCOMMAND);
			pMenu->RemoveMenu(ID_FILECONTEXTMENU_MANAGE, MF_BYCOMMAND);

			if (bdirectory) {//do directory first which depends on menu position
				pMenu->RemoveMenu(ID_FILECONTEXTMENU_VIEW, MF_BYCOMMAND);
				pMenu->RemoveMenu(ID_FILECONTEXTMENU_FAVORITE, MF_BYCOMMAND);
				pMenu->RemoveMenu(ID_FILECONTEXTMENU_OFFLINE, MF_BYCOMMAND);
				pMenu->RemoveMenu(ID_FILECONTEXTMENU_DOWNLOAD, MF_BYCOMMAND);
				pMenu->RemoveMenu(0, MF_BYPOSITION);
				pMenu->RemoveMenu(0, MF_BYPOSITION);
			}
			if (boffline) {
				pMenu->RemoveMenu(ID_FILECONTEXTMENU_OFFLINE, MF_BYCOMMAND);
			}
			if (bfavorite) {
				pMenu->RemoveMenu(ID_FILECONTEXTMENU_FAVORITE, MF_BYCOMMAND);
			}

			MENUITEMINFO minfo;
			minfo.cbSize = sizeof(MENUITEMINFO);
			minfo.fMask = MIIM_STRING;
			switch (m_selBtnIdx) {
			case NX_BTN_FAVORITE:
				tempStr = theApp.LoadString(IDS_REMOVE_FROM_FAVORITE);
				minfo.dwTypeData = tempStr.GetBuffer()/*L"Remove from Favorite"*/;
				pMenu->SetMenuItemInfoW(ID_FILECONTEXTMENU_DELETE, &minfo);
				break;
			case NX_BTN_OFFLINE:
				tempStr = theApp.LoadString(IDS_DELETE_OFFLINE_FILE);
				minfo.dwTypeData = tempStr.GetBuffer()/*L"Delete Offline File"*/;
				pMenu->SetMenuItemInfoW(ID_FILECONTEXTMENU_DELETE, &minfo);
				break;
			case NX_BTN_MYDRIVE:
			case NX_BTN_OTHERREPOS:
			case NX_BTN_MYVAULT:
				break;
			}
		}
	}

	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this, NULL);
	*pResult = 0;
}

BOOL CRepoPage::IsUploadAvailable(void)
{
	if (m_bSwitchpage)
		return FALSE;

	if (m_selBtnIdx == NX_BTN_MYDRIVE)
		return TRUE;
	if (m_selBtnIdx == NX_BTN_OTHERREPOS)
		return FALSE;
	if (m_RepoList.GetCurSel() != -1) {
		int dataidx = GetReposDataIndex(m_RepoList.GetCurSel() + 1);
		if (m_RepoDataArr[dataidx].type == NX::RmRepository::REPO_TYPE::SHAREPOINTONLINE) {
			if (0 == m_RepoDataArr[dataidx].nxrmPath.compare(L"/")) {
				return FALSE;//sharepoint online root directory
			}
		}
		return TRUE;// all other repository
	}

	return FALSE;//for all others
}

BOOL CRepoPage::IsProtectAvailable(void)
{
	BOOL bdirectory;
	BOOL bnxl;

	if (m_bSwitchpage)
		return FALSE;
	int idx = m_fileList.GetSelectionMark();
	if (-1 == idx)
		return FALSE;
	idx = GetFileListRepoDataIndex(idx);
	if (idx == -1)
		return FALSE;
	RepositoryFileInfo file = m_fileData[idx];

	switch (m_selBtnIdx) {
	case NX_BTN_FAVORITE:
	case NX_BTN_OFFLINE:
	case NX_BTN_MYDRIVE:
		bdirectory = file.bIsDirectory;
		bnxl = file.bIsNXL;
		break;
	case NX_BTN_OTHERREPOS:
		return (!file.bIsNXL && !file.bIsDirectory);
	default:
		return FALSE;
	}

	if (bdirectory || bnxl || !IsFileDeleteAvailable())
		return FALSE;

	return TRUE;
}

BOOL CRepoPage::IsShareAvailable()
{
	BOOL bdirectory;

	if (m_bSwitchpage)
		return FALSE;
	int idx = m_fileList.GetSelectionMark();
	if (-1 == idx)
		return FALSE;
	switch (m_selBtnIdx) {
	case NX_BTN_MYVAULT:
	case NX_BTN_DELETEFILES:
	case NX_BTN_SHAREFILES:
	{
		idx = GetFileListMyVaultDataIndex(idx);
		MyVaultFileInfo fileinfo;
		fileinfo = m_MyVaultFileInfo[idx];
		return !fileinfo.bIsShared && !fileinfo.bIsDeleted;
	}
	break;
	case NX_BTN_FAVORITE:
	case NX_BTN_OFFLINE:
	case NX_BTN_OTHERREPOS:
	case NX_BTN_MYDRIVE:
	{
		idx = GetFileListRepoDataIndex(idx);
		if (idx == -1)
			return FALSE;
		RepositoryFileInfo file = m_fileData[idx];
		bdirectory = file.bIsDirectory;
	}
	break;
	}

	if (bdirectory)
		return FALSE;

	return TRUE;
}

BOOL CRepoPage::IsFileInfoAvailable()
{
	if (m_bSwitchpage)
		return FALSE;

	int idx = m_fileList.GetSelectionMark();
	if (-1 == idx)
		return FALSE;
	switch (m_selBtnIdx) {
	case NX_BTN_MYVAULT:
	case NX_BTN_DELETEFILES:
	case NX_BTN_SHAREFILES:
		return TRUE;
	case NX_BTN_FAVORITE:
	case NX_BTN_OTHERREPOS:
	case NX_BTN_MYDRIVE:
		{
			BOOL bnxl;
			idx = GetFileListRepoDataIndex(idx);
			if (-1 == idx)
				return FALSE;
			RepositoryFileInfo file = m_fileData[idx];
			bnxl = file.bIsNXL;
			return bnxl;
		}
	case NX_BTN_OFFLINE:
		break;
	}

	return FALSE;
}

BOOL CRepoPage::IsFileDeleteAvailable()
{
	BOOL bdelete;

	if (m_bSwitchpage)
		return FALSE;

	int idx = m_fileList.GetSelectionMark();
	if (-1 == idx)
		return FALSE;
	switch (m_selBtnIdx) {
	case NX_BTN_MYVAULT:
	case NX_BTN_SHAREFILES:
	{
		idx = GetFileListMyVaultDataIndex(idx);
		MyVaultFileInfo fileinfo;
		fileinfo = m_MyVaultFileInfo[idx];

		bdelete = fileinfo.bIsDeleted;
	}
	break;
	case NX_BTN_FAVORITE:
	case NX_BTN_OFFLINE:
	case NX_BTN_MYDRIVE:
		if (-1 == GetFileListRepoDataIndex())
			bdelete = TRUE;//can't delete go parent folder
		else
			bdelete = FALSE;
		break;
	case NX_BTN_OTHERREPOS:
	case NX_BTN_DELETEFILES:
		return FALSE;
	break;
	}

	return !bdelete;
}

BOOL CRepoPage::IsFileRevokeAvailable()
{
	BOOL brevoke;

	if (m_bSwitchpage)
		return FALSE;

	int idx = m_fileList.GetSelectionMark();
	if (-1 == idx)
		return FALSE;
	switch (m_selBtnIdx) {
	case NX_BTN_MYVAULT:
	case NX_BTN_SHAREFILES:
	{
		idx = GetFileListMyVaultDataIndex(idx);
		MyVaultFileInfo fileinfo;
		fileinfo = m_MyVaultFileInfo[idx];
		brevoke = fileinfo.bIsRevoked;
	}
	break;
	case NX_BTN_FAVORITE:
	case NX_BTN_OFFLINE:
	case NX_BTN_OTHERREPOS:
	case NX_BTN_MYDRIVE:
	{
		brevoke = FALSE;
	}
	break;
	}

	return !brevoke;
}

void CRepoPage::OnCbnSelchangeComboMyvalutfilter()
{
	UpdateData(TRUE);
	m_btnLoadMore.ShowWindow(SW_HIDE);
	MyVaultListFiles(m_strSearch.GetBuffer());
}


void CRepoPage::OnItemchangedListRepofiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->uNewState == 0)
		m_fileList.SetSelectionMark(-1);

	*pResult = 0;
}


void CRepoPage::OnManage()
{
	ASSERT(m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_SHAREFILES || m_selBtnIdx == NX_BTN_DELETEFILES);
	if (m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_SHAREFILES || m_selBtnIdx == NX_BTN_DELETEFILES)
	{//MyVault is open.
		int idx = GetFileListMyVaultDataIndex();
		ASSERT(-1 != idx);
		if (-1 == idx)
			return;

		int imgIdx = theApp.GetFileExtensionImageIndex(m_MyVaultFileInfo[idx].rFileInfo.strFileExtension, m_MyVaultFileInfo[idx].rFileInfo.bIsNXL);
		if (theApp.ShowMyVaultFileProperty(GetSafeHwnd(), m_MyVaultFileInfo[idx], theApp.GetFileExtensionImageList()->ExtractIconW(imgIdx))) {
			OnBnClickedButtonRefresh();
		}
	}
}

void CRepoPage::MoveWindowPos(CWnd * pwnd, int right, int bottom, BOOL bKeepLeft)
{
	RECT rc;
	pwnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	if (right != 0) {
		if (!bKeepLeft)rc.left = right - (rc.right -rc.left);
		rc.right = right;
	}
	if(bottom != 0)
		rc.bottom = bottom;
	pwnd->MoveWindow(&rc, TRUE);
}

void CRepoPage::ResizeWindow(int cx, int cy)
{
    if (cx == 0 || cy == 0)
    {
        return;
    }
    static int lastCx = 0; //Keep last time width

	MoveWindowPos(&m_stcTitleBar, cx, 0, TRUE);
	RECT rc;
	m_stcMySpaceText.GetWindowRect(&rc);
	int width = rc.right - rc.left;
	m_btnUpDown.GetWindowRect(&rc);
	MoveWindowPos(&m_cmbProjects, (cx + width) / 2 + (rc.right - rc.left) + 5, 0);
	MoveWindowPos(&m_stcVerticalLine, 0, cy-1, TRUE);
	MoveWindowPos(&m_stcHorizLine, cx-1, 0, TRUE);
	MoveWindowPos(&m_RepositoryName, cx - 95, 0, TRUE);
	MoveWindowPos(&m_btnLoadMore, cx - 15, 0);
	MoveWindowPos(&m_fileList, cx-15, cy-15, TRUE);
	if (m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_SHAREFILES || m_selBtnIdx == NX_BTN_DELETEFILES)
		ResizeMyVaultFileListcontrol();
	else
		ResizeRepoFileListControl();

    if (cx <= lastCx)
    {
        MoveWindowPos(&m_navBand, cx - 245, 0, TRUE);
        MoveWindowPos(&m_btnRefresh, cx - 255, 0);
        MoveWindowPos(&m_stcVertiSeprator, cx - 245, 0);
        MoveWindowPos(&m_searchBtn, cx - 205, 0);
        MoveWindowPos(&m_searchEditBox, cx - 30, 0);
        MoveWindowPos(&m_resetSearchBtn, cx - 15, 0);
        MoveWindowPos(&m_btnAddDir, cx - 55, 0);
        MoveWindowPos(&m_btnUpload, cx - 15, 0);
        MoveWindowPos(&m_stcMySpaceText, (cx + width) / 2, 0);
        MoveWindowPos(&m_btnUpDown, (cx + width) / 2 + (rc.right - rc.left) + 5, 0);
    }
    else
    {
        MoveWindowPos(&m_btnUpDown, (cx + width) / 2 + (rc.right - rc.left) + 5, 0);
        MoveWindowPos(&m_stcMySpaceText, (cx + width) / 2, 0);
        MoveWindowPos(&m_btnUpload, cx - 15, 0);
        MoveWindowPos(&m_btnAddDir, cx - 55, 0);
        MoveWindowPos(&m_resetSearchBtn, cx - 15, 0);
        MoveWindowPos(&m_searchEditBox, cx - 30, 0);
        MoveWindowPos(&m_searchBtn, cx - 205, 0);
        MoveWindowPos(&m_stcVertiSeprator, cx - 245, 0);
        MoveWindowPos(&m_btnRefresh, cx - 255, 0);
        MoveWindowPos(&m_navBand, cx - 245, 0, TRUE);
    }

    lastCx = cx;

	MoveWindowPos(&m_stcRepoPath, cx-310, 0, TRUE);

	m_switchDlg.ResizeWindow(cx, cy);
    if (m_switchDlg.IsWindowVisible())
    {
        CRect rcWnd;
        m_switchDlg.GetWindowRect(&rcWnd);
        ScreenToClient(&rcWnd);
        cy = rcWnd.bottom;
    }
    MoveWindow(0, 0, cx, cy);
}


void CRepoPage::OnClickedRadioDeletefiles()
{
	ChangeRepo(NX_BTN_DELETEFILES);
	m_iCurSelectedListColumn = -1;
	m_bColumnAscend = TRUE;
	RefreshSearchList();

	RepositoryData rdata;
	rdata.id = DELETEDFILE_IDSTRING;
	rdata.dispPath = L"";
	rdata.nxrmPath = L"";
	AddHistoryInfo(rdata);
}


void CRepoPage::OnClickedRadioSharedfiles()
{
	ChangeRepo(NX_BTN_SHAREFILES);
	m_iCurSelectedListColumn = -1;
	m_bColumnAscend = TRUE;
	RefreshSearchList();

	RepositoryData rdata;
	rdata.id = SHAREDFILE_IDSTRING;
	rdata.dispPath = L"";
	rdata.nxrmPath = L"";
	AddHistoryInfo(rdata);
}


void CRepoPage::OnFilecontextmenuViewactivity()
{
	std::wstring filename;
	std::wstring uid;

	switch (m_selBtnIdx)
	{
	case NX_BTN_MYDRIVE:
	{
		int idx = GetFileListRepoDataIndex();
		ASSERT(-1 != idx);
		if (-1 == idx)
			return;
		filename = m_fileData[idx].strFileName;
		if (!theApp.GetRepoNXLFileUid(0, m_fileData[idx].strPath, uid))
			return;
	}
		break;
	case NX_BTN_MYVAULT:
	case NX_BTN_DELETEFILES:
	case NX_BTN_SHAREFILES:
	{
		int idx = GetFileListMyVaultDataIndex();
		ASSERT(-1 != idx);
		if (-1 == idx)
			return;
		filename = m_MyVaultFileInfo[idx].rFileInfo.strFileName;
		uid = m_MyVaultFileInfo[idx].strDuid;
	}
		break;
	case NX_BTN_FAVORITE:
	case NX_BTN_OFFLINE:
	case NX_BTN_OTHERREPOS:
	default:
		return;
	}

	CActivityLogDlg dlg(this);
	if (!theApp.GetFileActivityLog(filename, uid, dlg.m_FileActivities))
		return;
	dlg.m_strFileName = filename.c_str();
	dlg.DoModal();
}

void CRepoPage::OnHideAllCtrl(BOOL bhide /* = TRUE */)
{
	ShowSearchUI(bhide? FALSE: (m_selBtnIdx != NX_BTN_FAVORITE && m_selBtnIdx != NX_BTN_OFFLINE)? TRUE: FALSE);
	m_cmbMyvaultFilter.ShowWindow(bhide ? SW_HIDE : (m_selBtnIdx == NX_BTN_MYVAULT)? SW_SHOW : SW_HIDE);
	m_RepositoryName.ShowWindow(bhide ? SW_HIDE : (m_selBtnIdx != NX_BTN_MYVAULT)? SW_SHOW:SW_HIDE);
	m_btnUpload.ShowWindow(bhide ? SW_HIDE : IsUploadAvailable() ? SW_SHOW : SW_HIDE);
	m_btnAddDir.ShowWindow(bhide ? SW_HIDE : IsUploadAvailable() ? SW_SHOW : SW_HIDE);
	m_RepoList.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	m_stcRepoPath.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	m_fileList.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	m_searchBtn.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	m_RepoList.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	for (size_t i = 0; i < m_btnArr.size(); i++) {
		m_btnArr[i]->ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	}
	m_navBand.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	if (bhide) {
		m_btnGoBack.ShowWindow(SW_HIDE);
		m_btnForward.ShowWindow(SW_HIDE);
	}
	else {
		UpdateBrowserButton();
	}
	m_btnRefresh.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	m_stcVerticalLine.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	m_stcVertiSeprator.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	m_stcHorizLine.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	m_stcBtnHorizLine.ShowWindow(bhide ? SW_HIDE : SW_SHOW);
	m_btnLoadMore.ShowWindow(SW_HIDE);//alway hide

	m_btnUpDown.ShowWindow(theApp.GetProjectCount() == 0 ? SW_HIDE : SW_SHOW);
}

void CRepoPage::ResetMySpaceButton(void)
{
	if (m_bSwitchpage)
		OnBnClickedButtonUpdown();
}

void CRepoPage::OnBnClickedButtonUpdown()
{
	if (theApp.GetProjectCount() == 0)
		return;

	if (m_bSwitchpage)
	{
		m_btnUpDown.SetImages(IDB_PNG_WHITE_GODOWN, 0, 0, NX_NEXTLABS_STANDARD_GREEN);
		m_bSwitchpage = false;
		m_switchDlg.ShowWindow(SW_HIDE);
		OnHideAllCtrl(FALSE);
		if (m_strSearch.GetLength() == 0) { //refresh list only when user are not searching.
			if (m_selBtnIdx == NX_BTN_MYDRIVE && m_bRefreshFileList) {
				RepoSearchFile(0, L"");
			}
			else if (m_selBtnIdx == NX_BTN_MYVAULT || m_selBtnIdx == NX_BTN_DELETEFILES || m_selBtnIdx == NX_BTN_SHAREFILES) {
				if (m_bRefreshMyVault) {
					OnBnClickedButtonRefresh();
				}
			}
		}

	}
	else
	{
		m_btnUpDown.SetImages(IDB_PNG_WHITE_GOUP, 0, 0, NX_NEXTLABS_STANDARD_GREEN);
		m_bSwitchpage = true;
		OnHideAllCtrl(TRUE);
		m_switchDlg.ShowWindow(SW_SHOW);
		m_switchDlg.RefreshProjList();
		CRect rectBanner;
		m_stcTitleBar.GetWindowRect(&rectBanner);
		CRect rcScroll = m_switchDlg.m_rcBound;
		rcScroll.bottom += rectBanner.Height();
		SetWindowPos(0, 0, 0, rcScroll.Width(), rcScroll.Height(), SWP_NOMOVE | SWP_NOZORDER);
        m_switchDlg.SetLeftTopPosition(CPoint(0, rectBanner.Height()));
		m_switchDlg.SetWindowPos(&CWnd::wndTop, 0, rectBanner.Height(), 0, 0, SWP_NOSIZE);
	}
    CRect rc;
    GetParent()->GetClientRect(&rc);
    ResizeRepoPage(theApp.m_mainhWnd,rc.Width(), rc.Height());
}


void CRepoPage::OnStnClickedHomeText()
{
	::PostMessageW(theApp.m_mainhWnd, WM_COMMAND, ID_VIEW_HOME, (LPARAM)0);
}

#define PROJECT_MENU_LIST_ID_BASE 51000

void CRepoPage::OnCbnSelchangeComboProjects()
{
	int nindex = PROJECT_MENU_LIST_ID_BASE;
	if (-1 != m_cmbProjects.GetCurSel()) {
		m_cmbProjects.ShowWindow(SW_HIDE);
		OnBnClickedButtonUpdown();//recover the page.
		::PostMessageW(theApp.m_mainhWnd, WM_COMMAND,m_cmbProjects.GetCurSel() + PROJECT_MENU_LIST_ID_BASE, (LPARAM)0);
	}
}



void CRepoPage::OnSelendcancelComboProjects()
{
	m_cmbProjects.ShowWindow(SW_HIDE);
}

LRESULT CRepoPage::OnRefreshFileList(WPARAM wParam, LPARAM lParam)
{
	if (m_bSwitchpage) {
		m_bRefreshFileList = TRUE;
		theApp.RefreshHomePageMyDriveUsage();
		return LRESULT();
	}
	switch (m_selBtnIdx)
	{
	case NX_BTN_MYDRIVE:
		if (m_strSearch.GetLength() == 0) { //refresh list only when user are not searching.
			RepoSearchFile(0, L"");
		}
		theApp.RefreshHomePageMyDriveUsage();
		break;
	case NX_BTN_MYVAULT:
	case NX_BTN_DELETEFILES:
	case NX_BTN_SHAREFILES:
		break;
	case NX_BTN_FAVORITE:
	case NX_BTN_OFFLINE:
	case NX_BTN_OTHERREPOS:
		break;
	default:
		break;
	}
	return LRESULT();
}

LRESULT CRepoPage::OnRefreshMyVaultFileList(WPARAM wParam, LPARAM lParam)
{
	if (theApp.m_curShowedPage != MD_REPORITORYHOME || m_bSwitchpage) {//repository home is not active page
		m_bRefreshMyVault = TRUE;
	}
	else {
		switch (m_selBtnIdx)
		{
			break;
		case NX_BTN_MYVAULT:
		case NX_BTN_DELETEFILES:
		case NX_BTN_SHAREFILES:
			if (m_strSearch.GetLength() != 0) {
				m_bRefreshMyVault = TRUE;
			}
			else {
				OnBnClickedButtonRefresh();
			}
			break;
		case NX_BTN_MYDRIVE:
		case NX_BTN_FAVORITE:
		case NX_BTN_OFFLINE:
		case NX_BTN_OTHERREPOS:
			break;
		default:
			break;
		}
	}
	return LRESULT();
}
/*void CRepoPage::OnStnClickedStaticCaptionName()
{
	if (theApp.GetProjectCount() == 0)
		return;

	while (m_cmbProjects.GetCount()) {
		m_cmbProjects.DeleteString(0);
	}
	size_t arrcount = theApp.m_prjDataFromMe.size();
	size_t st;
	for (st = 0; st < arrcount; st++) {
		m_cmbProjects.AddString(theApp.m_prjDataFromMe[st].m_projName);
	}
	arrcount = theApp.m_prjDataFromOthers.size();
	for (st = 0; st < arrcount; st++) {
		m_cmbProjects.AddString(theApp.m_prjDataFromOthers[st].m_projName);
	}
	m_cmbProjects.ShowDropDown(TRUE);
}*/


BOOL CRepoPage::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return TRUE;
}


void CRepoPage::OnClickedButtonLoadmore()
{
	MSG msg;
	msg.hwnd = m_hWnd;
	msg.message = WM_COMMAND;
	msg.wParam = ID_FILECONTEXTMENU_REFRESH;
	msg.lParam = 0;

	CWaitCursor wcur;
	theApp.ShowTrayMsg(theApp.LoadString(IDS_LOADING_MORE).GetBuffer()/*L"Loading More..."*/);
	theApp.GetMyVaultMoreData(&m_finder, &m_MyVaultFileInfo, ProgressCallBack, &msg);
}
