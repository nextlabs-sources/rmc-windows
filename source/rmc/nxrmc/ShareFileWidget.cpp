// ShareFileWidget.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "ShareFileWidget.h"
#include "afxdialogex.h"


// CShareFileWidget dialog

IMPLEMENT_DYNAMIC(CShareFileWidget, CFlatDlgBase)

CShareFileWidget::CShareFileWidget(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_SHAREFILEWIDGET_DLG, pParent)
	,m_bProtectFile (TRUE)
	, m_strRepoPath(_T(""))
	, m_strDragDrop(_T(""))
{

}

CShareFileWidget::~CShareFileWidget()
{
}

void CShareFileWidget::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REPO, m_RepoList);
	DDX_Control(pDX, IDC_LIST_REPOFILES, m_fileList);
	DDX_Text(pDX, IDC_STATIC_REPO_PATH, m_strRepoPath);
	DDX_Text(pDX, IDC_STATIC_DRAGDROP, m_strDragDrop);
	DDX_Control(pDX, IDC_STATIC_DRAGDROP, m_stcDragDrop);
	DDX_Control(pDX, IDOK, m_btnProceed);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_LOCALDRIVE, m_btnLocalFiles);
}


BEGIN_MESSAGE_MAP(CShareFileWidget, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOCALDRIVE, &CShareFileWidget::OnBnClickedButtonLocaldrive)
	ON_LBN_SELCHANGE(IDC_LIST_REPO, &CShareFileWidget::OnLbnSelchangeListRepo)
	ON_BN_CLICKED(IDOK, &CShareFileWidget::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_REPOFILES, &CShareFileWidget::OnItemdblclickListRepofiles)
	ON_NOTIFY(NM_CLICK, IDC_LIST_REPOFILES, &CShareFileWidget::OnItemclickListRepofiles)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CShareFileWidget message handlers


void CShareFileWidget::OnBnClickedButtonLocaldrive()
{
	CFileDialog fileDialog(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"All types(*.*)|*.*||",
		this
	);

	OPENFILENAME& ofn = fileDialog.GetOFN();
	if (fileDialog.DoModal() == IDCANCEL)
	{
		return;
	}

	CString fullSavePath = fileDialog.GetPathName();

	DropOneFile(fullSavePath);
}


BOOL CShareFileWidget::OnInitDialog()
{
	CFlatDlgBase::OnInitDialog();

	size_t total_repo = theApp.GetRepositoryArray(m_RepositoryArr);

	if (0 == (int)total_repo) {
		theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_AT_GETTING_REPO_LIST)/*L"An error occurred while getting the repository list."*/, theApp.LoadString(IDS_ERROR)/*L"Error"*/);
		return FALSE;
	}
	m_RepoList.ResetContent();
	m_RepoList.ReleaseItems();
	for (size_t i = 0; i < total_repo; i++)
	{
		auto a = m_RepositoryArr[i];

		int idx = m_RepoList.AddItem(a.Name.c_str(), GetRepoIconID(a.Type), GetRepoSelIconID(a.Type), a.AccountName.c_str());
		RepositoryData rdata;
		rdata.type = a.Type;
		rdata.id = a.Id;
		rdata.dispPath = L"/";
		rdata.nxrmPath = L"/";
		m_RepoDataArr.push_back(rdata);

		m_RepositoryArr[i].iDataIndex = (int)i;
	}

	DragAcceptFiles(TRUE);
	CString strtemp = (m_bProtectFile) ? theApp.LoadString(IDS_WIDGET_TITLE_PROTECT) : theApp.LoadString(IDS_WIDGET_TITLE_SHARE);
	SetWindowTextW(strtemp);
	strtemp = (m_bProtectFile) ? theApp.LoadString(IDS_KEYWORD_PROTECT) : theApp.LoadString(IDS_KEYWORD_SHARE);
	m_strDragDrop.Format(IDS_WIDGET_DRAGDROP, strtemp.GetBuffer() );
	
	CRect rcList;
	m_fileList.GetWindowRect(&rcList);
	m_fileList.DeleteAllItems();
	m_fileList.SetImageList(theApp.GetFileExtensionImageList(), LVSIL_SMALL);
	m_fileList.InsertColumn(0, theApp.LoadString(IDS_NAME)/*L"Name"*/, LVCFMT_LEFT, rcList.Width());
	m_fileList.SetExtendedStyle(LVS_EX_SUBITEMIMAGES | LVS_EX_FULLROWSELECT);

	m_fileList.ShowWindow(SW_SHOW);

	m_btnLocalFiles.SetImagetText(IDB_PNG_LOCALFILE, IDB_PNG_LOCALFILE, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_BLACK);
	m_btnLocalFiles.SetBtnCheck(BST_INDETERMINATE);


	m_stcDragDrop.ShowWindow(SW_HIDE);

	m_btnProceed.EnableWindow(FALSE);
	m_btnProceed.SetColorsWithBorder(NX_NEXTLABS_STANDARD_GREEN, NX_NEXTLABS_STANDARD_WHITE);
	m_btnCancel.SetColorsWithBorder(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK);
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CShareFileWidget::OnLbnSelchangeListRepo()
{
	m_stcDragDrop.ShowWindow(SW_HIDE);
	m_fileList.ShowWindow(SW_SHOW);
	DragAcceptFiles(FALSE);

	int nSelIdx = m_RepoList.GetCurSel();
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	if (nSelIdx >= 0 && nSelIdx <= (int)m_RepositoryArr.size())
	{
		//reset path to root
		m_RepoDataArr[nSelIdx].dispPath = L"/";
		m_RepoDataArr[nSelIdx].nxrmPath = L"/";

		wstring path;
		path = m_RepoDataArr[nSelIdx].nxrmPath;

		auto repoName = m_RepositoryArr[nSelIdx].Name;

		std::vector<RepositoryFileInfo> m_files;

		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		m_files.clear();
		if (!theApp.GetRepositoryFileList(nSelIdx, path, m_files, L"", true)) {//alway refresh so that user can get latest file list
			CString errorMsg;
			errorMsg.FormatMessage(IDS_FMT_REFRESH_MSG, m_RepositoryArr[nSelIdx].Name.c_str());
			//auto strerr = L"An error occurred while refreshing the file list from " + m_RepositoryArr[nIndex].Name;
			theApp.ShowTrayMsg(errorMsg);
		}

		m_strCurRepoParentPath = L"/";
		m_strRepoPath = (repoName + m_strCurRepoParentPath).c_str();
		UpdateRepoFileList(m_files);
	}
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}


void CShareFileWidget::OnBnClickedOk()
{
	int nsel = m_fileList.GetSelectionMark();
	if (-1 == nsel)
		return;
	int idx = (int)m_fileList.GetItemData(nsel);

	if (idx == -1) {//go parent folder
		RepoGoParentFolder();
		return;
	}

	idx &= 0xFFFF;
	m_selRepoFile = m_fileData[idx];
	
	CDialogEx::OnOK();
}

void CShareFileWidget::UpdateRepoFileList(std::vector<RepositoryFileInfo> filelist)
{
	m_fileData.clear();
	m_fileList.DeleteAllItems();
	
	if (m_strCurRepoParentPath.length() > 1) {//add upfolder when current path is more than "/"
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

	std::for_each(filelist.cbegin(), filelist.cend(), [&](const RepositoryFileInfo & file) {
		if (!m_bProtectFile || !file.bIsNXL) {
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
			}
			else {
				listItem.iImage = theApp.GetFileExtensionImageIndex(file.strFileExtension, file.bIsNXL);
				m_fileList.SetItem(&listItem);
			}
			RepositoryFileInfo fileinfo = file;
			m_fileData.push_back(fileinfo);
			m_fileList.SetItemData(i, m_fileData.size() - 1);
		}
	});

	UpdateData(FALSE);
}

void CShareFileWidget::RepoOpenFileOrFolder(RepositoryFileInfo &repoFileInfo, int nIdx)
{
	if (!repoFileInfo.bIsDirectory)
	{
		OnBnClickedOk();
	}
	else
	{
		bool isFolder = false;
		std::wstring displayPath;
		int dataidx = m_RepositoryArr[nIdx].iDataIndex;
		if (dataidx == -1) {
			theApp.ShowTrayMsg(theApp.LoadString(IDS_ERROR_AT_OPENING_FILE)/*L"An error occurred while opening the file."*/);
			return;
		}
		m_RepoDataArr[dataidx].nxrmPath = repoFileInfo.strPath;;
		m_RepoDataArr[dataidx].dispPath = repoFileInfo.strDisplayPath;

		m_fileList.DeleteAllItems();
		wstring path;
		path = m_RepoDataArr[dataidx].nxrmPath;


		std::vector<RepositoryFileInfo> m_files;

		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		m_files.clear();
		if (!theApp.GetRepositoryFileList(nIdx, path, m_files, L"", true)) {//alway refresh so that user can get latest file list
			CString errorMsg;
			errorMsg.FormatMessage(IDS_FMT_REFRESH_MSG, m_RepositoryArr[nIdx].Name.c_str());
			//auto strerr = L"An error occurred while refreshing the file list from " + m_RepositoryArr[nIndex].Name;
			theApp.ShowTrayMsg(errorMsg);
		}

		m_strRepoPath = (m_RepositoryArr[nIdx].Name + m_RepoDataArr[nIdx].dispPath).c_str();
		m_strCurRepoParentPath = path;
		UpdateRepoFileList(m_files);
	}
}

void CShareFileWidget::RepoGoParentFolder()
{
	int nRepoIdx = m_RepoList.GetCurSel();

	auto & newPath = theApp.GetParentFolder(m_RepoDataArr[nRepoIdx].nxrmPath);
	auto & dispPath = theApp.GetParentFolder(m_RepoDataArr[nRepoIdx].dispPath);

	m_RepoDataArr[nRepoIdx].nxrmPath = newPath;
	m_RepoDataArr[nRepoIdx].dispPath = dispPath;

	std::vector<RepositoryFileInfo> m_files;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	if (!theApp.GetRepositoryFileList(nRepoIdx, newPath, m_files, L"", true)) {//alway refresh so that user can get latest file list
		CString errorMsg;
		errorMsg.FormatMessage(IDS_FMT_REFRESH_MSG, m_RepositoryArr[nRepoIdx].Name.c_str());
		//auto strerr = L"An error occurred while refreshing the file list from " + m_RepositoryArr[nIndex].Name;
		theApp.ShowTrayMsg(errorMsg);
	}

	m_strRepoPath = (m_RepositoryArr[nRepoIdx].Name + dispPath).c_str();
	m_strCurRepoParentPath = newPath;
	UpdateRepoFileList(m_files);

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

}
void CShareFileWidget::OnItemdblclickListRepofiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	if (phdr->iItem == -1)
		return;

	int nsel = m_fileList.GetSelectionMark();
	if (-1 == nsel)
		return;
	int idx = (int)m_fileList.GetItemData(nsel);

	if (idx == -1) {//go parent folder
		RepoGoParentFolder();
		return;
	}

	idx &= 0xFFFF;
	RepositoryFileInfo ItemData = m_fileData[idx];
	//Combine
	int nRepoIdx = m_RepoList.GetCurSel();
	RepoOpenFileOrFolder(ItemData, nRepoIdx);

	*pResult = 0;
}


void CShareFileWidget::OnItemclickListRepofiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	if (phdr->iItem == -1)
		return;

	int nsel = m_fileList.GetSelectionMark();
	if (-1 == nsel)
		return;
	int idx = (int)m_fileList.GetItemData(nsel);

	if (idx == -1 ) {//go parent folder
		m_btnProceed.EnableWindow(FALSE);
	}
	else {
		idx &= 0xFFFF;
		RepositoryFileInfo ItemData = m_fileData[idx];
		if (ItemData.bIsDirectory)
			m_btnProceed.EnableWindow(FALSE);
		else
			m_btnProceed.EnableWindow(TRUE);
	}

	*pResult = 0;
}

void CShareFileWidget::DropOneFile(CString fullpath)
{
	if (m_bProtectFile) {
		BOOL bisnxl;
		theApp.GetFileExtension(fullpath.GetBuffer(), &bisnxl);
		if (bisnxl) {
			MessageBox(theApp.LoadStringW(IDS_FILE_ALREADY_PROTECTED), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
			return;
		}
	}

	m_strFullPath = fullpath.GetBuffer();

	CDialogEx::OnOK();
}


void CShareFileWidget::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	wchar_t sItem[4 * MAX_PATH];
	int i = 0;
	while (DragQueryFile(hDropInfo, i++, sItem, 4 * MAX_PATH))
	{
		auto attrib = GetFileAttributes(sItem);
		if (!(attrib & FILE_ATTRIBUTE_NORMAL) && !(attrib & FILE_ATTRIBUTE_ARCHIVE))
		{
			continue;
		}
		CString fullPath = sItem;

		DropOneFile(fullPath);
		return;//Only support one file at this time
	}

	CFlatDlgBase::OnDropFiles(hDropInfo);
}

