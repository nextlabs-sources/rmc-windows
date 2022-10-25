// ProjectFileList.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmc.h"
#include "nxrmRMC.h"
#include "ProjectFileList.h"
#include "afxdialogex.h"
#include "resource.h"
#include "NewFolderDlg.h"
#include "DeleteDlg.h"


wstring GetSpaceString(__int64 bytes);

// CProjectFileList dialog
//void ListProjectFiles(wstring  currentPath, CProjData &projData);

//extern int GetFileImageIndex(CString fName, CString &typeName, bool &bEncrypt);
extern CString GetFileTimeString(NX::time::datetime dt);
extern bool SeparatePathAndName(const CString fullPath, CString &path, CString &name, const wchar_t sepChar = L'\\');
//CString CombinePath(const CString& current, const CString& subpath);


int GetFileImageIndex(CString fName, CString &typeName, bool &bEncrypt)
{

	int pos = fName.ReverseFind(L'.');
	auto len = fName.GetLength();
    CString fExtension;
    if (pos == -1)
    {
        bEncrypt = false;
        typeName = "";
        fExtension = L"";
    }
    else
    {


        fExtension = fName.Right(len - pos);
        CString fNamePart = fName.Left(pos);
        bool bEncrypted = false;
        if (!fExtension.CompareNoCase(L".nxl"))
        {
            bEncrypted = true;
            pos = fNamePart.ReverseFind(L'.');
            len = fNamePart.GetLength();
            fExtension = fNamePart.Right(len - pos);
        }
        if (pos == -1)
        {
            bEncrypt = bEncrypted;
            typeName = "";
            fExtension = L"";
        }
        else
        {

            bEncrypt = bEncrypted;
            typeName = fExtension.Right(fExtension.GetLength() - 1);
            typeName.MakeUpper();
        }
    }
	return theApp.GetFileExtensionImageIndex(fExtension.GetBuffer(), bEncrypt);
}

CString CombinePath(const CString& current, const CString& subpath)
{
    CString newPath(current);
    std::vector<std::wstring> vFolders;
    if (subpath == L"..")
    {
        if (newPath == L"/")
        {
            //return newPath;
        }
        else
        {
            bool bDispPath = true;
            CString currPath;
            int pos = newPath.ReverseFind(L'/');
            if (pos == newPath.GetLength() - 1)
            {
                bDispPath = true;
                currPath = newPath.Left(pos);
                pos = currPath.ReverseFind(L'/');
            }
            if (pos == 0)
            {	//Include slash
                pos++;
                newPath = newPath.Left(pos);
            }
            else
            {
                newPath = newPath.Left(pos + (bDispPath ? 0 : 1));
            }
            //return  newPath;
        }

    }
    else
    {
        if (newPath == L"/")
        {
            newPath = newPath + subpath;
        }
        else
        {
            newPath = newPath + L"/" + subpath;
        }
    }
    return newPath;
}


IMPLEMENT_DYNAMIC(CProjectFileList, CFlatDlgBase)

CProjectFileList::CProjectFileList(CWnd* pParent /*=NULL*/)

	: CFlatDlgBase(IDD_PROJ_FILE_LIST, pParent)
{

}

//CProjectFileList::CProjectFileList(CWnd * pParent)
//{
//}

CProjectFileList::~CProjectFileList()
{
}

void CProjectFileList::SetProject(const CProjData & projData)
{
   
    m_projData = projData;
    m_projName.SetWindowText(m_projData.m_projName);
    m_currDir = L"/";
    theApp.ListProjectFiles(m_currDir.GetBuffer(), m_projData);
    m_fileList.SetSortColumn(-1, false);
    //ShowSortedFileList();
    ConvertDataToFileData();
    DisplayAllFiles();
    SortColumn(m_iCurSelectedListColumn, true);
}


void CProjectFileList::RefreshPage()
{

    m_projName.SetWindowText(m_projData.m_projName);
    theApp.ListProjectFiles(m_currDir.GetBuffer(), m_projData);
    ConvertDataToFileData();
    DisplayAllFiles();
    SortColumn(m_iCurSelectedListColumn, true);
}


void CProjectFileList::ConvertDataToFileData()
{
    m_fileData.clear();

    for (auto file : m_projData.m_Dirs)
    {
        ProjFileData fData;
        fData.repoFile = file;
        CString fName = file.GetDisplayPath().c_str();
        CString path, name;
        fData.filePath = file.GetPath();
        SeparatePathAndName(fName, path, name, L'/');
        SeparatePathAndName(path, path, name, L'/');
        fData.fileName = name;
        fData.itemType = NX_RMC_FOLDER;
        fData.dt = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
        m_fileData.push_back(fData);
    }

    for (auto file : m_projData.m_files)
    {
        ProjFileData fData;
        fData.repoFile = file;
        CString fName = file.GetDisplayPath().c_str();
        CString path, name;
        SeparatePathAndName(fName, path, name, L'/');
        fData.dt = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
        fData.ownerEmail = file.GetOwner().GetEmail().c_str();
        fData.fileName = name;
        fData.filePath = file.GetPath();
        fData.fileSize = file.GetSize();
        fData.duid = file.GetDuid();
        bool bProtected = false;
        fData.imageID = GetFileImageIndex(fName, fData.fileType, bProtected);
        fData.itemType = NX_RMC_FILE;
        m_fileData.push_back(fData);
    }


}


void CProjectFileList::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_RESET_SEARCH, m_resetSearchBtn);
    DDX_Control(pDX, IDC_EDIT_SEARCH, m_searchEditBox);
    DDX_Control(pDX, IDC_BUTTON_SEARCH, m_searchBtn);
    DDX_Control(pDX, IDC_LIST_REPOFILES, m_fileList);
    DDX_Control(pDX, IDC_BUTTON_UPLOAD, m_uploadBtn);
    DDX_Control(pDX, IDC_STATIC_TITLE, m_titleName);
    DDX_Control(pDX, IDC_STATIC__DIRECTORY, m_filePath);
    DDX_Control(pDX, IDC_BUTTON_ADD_DIR, m_btnAddDir);
    DDX_Control(pDX, IDC_STATIC_PROJ_NAME, m_projName);
    DDX_Control(pDX, IDC_BUTTON_REFRESH, m_btnRefresh);
    DDX_Control(pDX, IDC_STATIC_HORIZ, m_stcHorizLine);
    DDX_Control(pDX, IDC_VERT_SEPARATOR, m_stcVertLine);
    DDX_Control(pDX, IDC_STATIC_SEARCH_SEP, m_searchSep);
    DDX_Control(pDX, IDC_STATIC_NAVIGATION_BAND, m_navBand);
}


BEGIN_MESSAGE_MAP(CProjectFileList, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
    ON_NOTIFY(NM_RCLICK, IDC_LIST_REPOFILES, &CProjectFileList::OnRclickListRepofiles)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_REPOFILES, &CProjectFileList::OnDblclkListRepofiles)
    ON_BN_CLICKED(IDC_BUTTON_RESET_SEARCH, &CProjectFileList::OnBnClickedButtonResetSearch)
    ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CProjectFileList::OnBnClickedButtonSearch)
    ON_STN_CLICKED(IDC_STATIC_SEARCHGROUP, &CProjectFileList::OnStnClickedStaticSearchgroup)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BUTTON_GOBACK2, &CProjectFileList::OnBnClickedButtonGoback2)
    ON_BN_CLICKED(IDC_BUTTON_FORWARD, &CProjectFileList::OnBnClickedButtonForward)
    ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CProjectFileList::OnBnClickedButtonUpload)
    ON_BN_CLICKED(IDC_BUTTON_ADD_DIR, &CProjectFileList::OnBnClickedButtonAddDir)
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CProjectFileList::OnBnClickedButtonRefresh)
    ON_COMMAND(ID_FILECONTEXTMENU_FILEPROPERTIES, &CProjectFileList::OnFilecontextmenuFileproperties)
    ON_COMMAND(ID_FILECONTEXTMENU_DELETE, &CProjectFileList::OnFilecontextmenuDelete)
    ON_COMMAND(ID_FILECONTEXTMENU_DOWNLOAD, &CProjectFileList::OnFilecontextmenuDownload)
    ON_COMMAND(ID_FILECONTEXTMENU_VIEW, &CProjectFileList::OnFilecontextmenuView)
    ON_COMMAND(ID_PROJECTCONTEXTMENU_VIEWACTIVITY, &CProjectFileList::OnProjectcontextmenuViewactivity)
    ON_MESSAGE(WM_NX_REFRESH_PROJFILELIST, &CProjectFileList::OnNxRefreshProjfilelist)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_REPOFILES, &CProjectFileList::OnLvnItemchangedListRepofiles)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

extern std::wstring OutputError(NX::Result &res, wstring errMsg);
// CProjectFileList message handlers
BOOL CProjectFileList::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    GetWindowRect(&m_rcBound);
    ScreenToClient(&m_rcBound);
    AddFontSize(60, m_titleName, true);
    AddFontSize(25, m_filePath, true);
    m_resetSearchBtn.SetImages(IDB_PNG_CLOSE, 0, 0);
    m_searchBtn.SetImages(IDB_PNG_SEARCH, 0, 0);
    m_btnAddDir.SetImages(IDB_PNG_ADD_FOLDER, IDB_PNG_ADD_FOLDER, 0, NX_NEXTLABS_STANDARD_WHITE);
    m_uploadBtn.SetImages(IDB_PNG_ADDFILE, IDB_PNG_ADDFILE_DISABLED, 0, NX_NEXTLABS_STANDARD_WHITE);

    m_gray.CreateSolidBrush(NX_NEXTLABS_STANDARD_GRAY);
    m_searchBtn.SetImages(IDB_PNG_SEARCH, 0, 0);
    m_resetSearchBtn.SetImages(IDB_PNG_CLOSE, 0, 0);

    m_btnRefresh.SetImages(IDB_PNG_REFRESH, 0, 0, NX_NEXTLABS_STANDARD_GRAY);

    m_searchEditBox.SendMessage(EM_SETCUEBANNER, 0, (LPARAM)(LPCWSTR)theApp.LoadStringW(IDS_SEARCH_FILE_FOLDER).GetBuffer()/*L"Search File/Folder"*/);

    //m_searchGroup.SetDrawType(NX_DRAWTYPE_ROUNDRECT);
    //m_goBack.SetImages(IDB_PNG_PREV, 0, 0, NX_NEXTLABS_STANDARD_GRAY);

    CRect rcList;
    m_fileList.GetWindowRect(&rcList);
    int width = rcList.Width();
    m_fileList.InsertColumn(0, theApp.LoadString(IDS_FILE_NAME)/*L"File name"*/, LVCFMT_LEFT, width / 2);
    m_fileList.InsertColumn(1, theApp.LoadString(IDS_DATE_MODIFIED)/*L"Date modified"*/, LVCFMT_LEFT, width / 6);
    m_fileList.InsertColumn(2, theApp.LoadString(IDS_TYPE)/*L"Type"*/, LVCFMT_LEFT, width / 12);
    m_fileList.InsertColumn(3, theApp.LoadString(IDS_SIZE)/*L"Size"*/, LVCFMT_LEFT, width / 12);
    m_fileList.InsertColumn(4, theApp.LoadString(IDS_SHARED_BY)/*L"Shared by"*/, LVCFMT_LEFT, width / 6);

    m_fileList.SetExtendedStyle(LVS_EX_SUBITEMIMAGES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

    m_fileList.SetImageList(theApp.GetFileExtensionImageList(), LVSIL_SMALL);
    m_sortedType = OrderByCreationTimeDescend;

    m_iCurSelectedListColumn = 1;       //Default is sort by date
    m_bColumnAscend = false;            //Descnding

    SetAllOrignRects();
    SetBackgroundColor(NX_NEXTLABS_STANDARD_WHITE);

    //m_currDir = L"/";
    //m_btnAddDir.ShowWindow(SW_HIDE);
    //ListProjectFiles(m_currDir.GetBuffer(), m_projData);
    //m_sortedType = OrderByCreationTimeDescend;
    //m_fileList.SetSortColumn(1, false);
    //ShowSortedFileList();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CProjectFileList::DisplayAllFiles()
{
    m_fileList.DeleteAllItems();
    bool IsRoot = (m_currDir == L'/');

    if (IsRoot)
    {
        m_filePath.SetWindowText(theApp.LoadString(IDS_ROOT)/*L"root"*/);
    }
    else
    {
        if (m_currDispDir.GetAt(m_currDispDir.GetLength() - 1) == L'/')
        {
            m_currDispDir.SetAt(m_currDispDir.GetLength() - 1, L'\0');
        }
        m_filePath.SetWindowText(m_currDispDir);
		if (!m_searchMode)
		{
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
    for (int n = 0; n < (int)m_fileData.size(); n++) {

        auto x = m_fileData[n];
        LVITEM listItem;
        int i = m_fileList.GetItemCount();
        m_fileList.InsertItem(i, L"");

        memset((void*)&listItem, 0, sizeof(LVITEM));
        listItem.mask = LVIF_IMAGE | LVIF_TEXT;
        listItem.pszText = x.fileName.GetBuffer();
        listItem.iItem = i;
        listItem.iSubItem = 0;



        if (x.itemType == NX_RMC_FOLDER) {
            listItem.iImage = theApp.GetFolderImageIndex();
            m_fileList.SetItem(&listItem);

            NX::time::datetime dt = x.dt;
            auto timeText = GetFileTimeString(dt);
            m_fileList.SetItemText(i, 1, timeText);
            CString fileType;
            fileType = theApp.LoadString(IDS_FOLDER)/*L"Folder"*/;
            m_fileList.SetItemText(i, 2, fileType);
        }
        else 
        {
            CString fileType;

            listItem.iImage = x.imageID;
            m_fileList.SetItem(&listItem);

            NX::time::datetime dt = x.dt;
            auto timeText = GetFileTimeString(dt);
            m_fileList.SetItemText(i, 1, timeText);
            fileType = x.fileType + theApp.LoadString(IDS_FILE_SUFFIX)/*L" file"*/;
            m_fileList.SetItemText(i, 2, fileType);

            wstringstream txtOut;

            auto sizeStr = GetSpaceString(x.fileSize);
            m_fileList.SetItemText(i, 3, sizeStr.c_str());
            txtOut.str(L"");
            txtOut << x.ownerEmail.GetBuffer();
            m_fileList.SetItemText(i, 4, txtOut.str().c_str());

        }
        m_fileList.SetItemData(i, n);
    }

}




HBRUSH CProjectFileList::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        auto ctlID = pWnd->GetDlgCtrlID();
        if (ctlID == IDC_STATIC_TITLE || ctlID == IDC_STATIC__DIRECTORY )
        {
            hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
            pDC->SetBkMode(TRANSPARENT);
        }
        else if (ctlID == IDC_STATIC_NAVIGATION_BAND)
        {
            hbr = (HBRUSH)m_gray;
        }
        else if (ctlID == IDC_STATIC_PROJ_NAME)
        {
            pDC->SetBkColor(NX_NEXTLABS_STANDARD_GRAY);
            hbr = (HBRUSH)m_gray;
        }
    }

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


void CProjectFileList::OnDestroy()
{
    CDialogEx::OnDestroy();
    m_gray.DeleteObject();
    // TODO: Add your message handler code here
}


struct CompareStruct
{
    CNxMFCListCtrl *pList; // pointer to list
    int iColumn;//column to compare
    BOOL bAscend;
    vector <ProjFileData> * pFiledata;
   
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
        if (pcs->pFiledata->at(nidx1).itemType == NX_RMC_FOLDER && !(pcs->pFiledata->at(nidx2).itemType == NX_RMC_FOLDER)) {
            iret = -1;
        }
        else if (!(pcs->pFiledata->at(nidx1).itemType == NX_RMC_FOLDER) && pcs->pFiledata->at(nidx2).itemType == NX_RMC_FOLDER) {
            iret = 1;
        }
        else if (pcs->pFiledata->at(nidx1).itemType == NX_RMC_FOLDER && pcs->pFiledata->at(nidx2).itemType == NX_RMC_FOLDER) {
            if (pcs->iColumn == 1)
            {
                if (pcs->pFiledata->at(nidx1).dt > pcs->pFiledata->at(nidx2).dt)
                    iret = 1;
                else if (pcs->pFiledata->at(nidx1).dt < pcs->pFiledata->at(nidx2).dt)
                    iret = -1;
                else
                    iret = 0;
            }
            else
            {
                iret = _wcsicmp(pcs->pFiledata->at(nidx1).fileName, pcs->pFiledata->at(nidx2).fileName);
            }
        }
        else 
        {
            switch (pcs->iColumn)
            {
            case 0://Compare FileName;
                iret = _wcsicmp(pcs->pFiledata->at(nidx1).fileName, pcs->pFiledata->at(nidx2).fileName);
                break;
            case 1://Compare Date
                if (pcs->pFiledata->at(nidx1).dt > pcs->pFiledata->at(nidx2).dt)
                    iret = 1;
                else if (pcs->pFiledata->at(nidx1).dt < pcs->pFiledata->at(nidx2).dt)
                    iret = -1;
                else
                    iret = 0;
                break;
            case 2://Compare Type
                iret = _wcsicmp(pcs->pFiledata->at(nidx1).fileType, pcs->pFiledata->at(nidx2).fileType);
                break;
            case 3://Compare Size
                if (pcs->pFiledata->at(nidx1).fileSize > pcs->pFiledata->at(nidx2).fileSize)
                    iret = 1;
                else if (pcs->pFiledata->at(nidx1).fileSize < pcs->pFiledata->at(nidx2).fileSize)
                    iret = -1;
                else
                    iret = 0;
                break;
            case 4://Compare Repository
                iret = pcs->pFiledata->at(nidx1).ownerEmail.Compare(pcs->pFiledata->at(nidx2).ownerEmail);
                break;
            default:
                iret = 0;//do nothing
            }
        }
    }
    if (!pcs->bAscend)
        iret *= (-1);
    return iret;
}

void CProjectFileList::SortColumn(int columnindex, BOOL bRefresh)
{
    if (columnindex > 4)
        return;

    if (!bRefresh)
    {
        if (m_iCurSelectedListColumn != columnindex)
            m_bColumnAscend = TRUE;
        else
            m_bColumnAscend = !m_bColumnAscend;
    }

    m_iCurSelectedListColumn = columnindex;
    m_fileList.SetSortColumn(columnindex, m_bColumnAscend);
    CompareStruct cs;
    cs.pList = &m_fileList;
    cs.iColumn = columnindex;
    cs.bAscend = m_bColumnAscend;
   
    cs.pFiledata = &m_fileData;

    m_fileList.SortItems(MyCompareProc, (LPARAM)&cs);
}

void CProjectFileList::OnColumnclickList(NMHDR * pNMHDR, LRESULT * pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    SortColumn(pNMLV->iSubItem);
}

void CProjectFileList::OnRclickListRepofiles(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    POINT pt;

    CMenu menu;
    menu.LoadMenuW(IDR_PROJ_CONTEXT_MENU);
    auto pMenu = menu.GetSubMenu(0);
    GetCursorPos(&pt);
    auto idx = pNMItemActivate->iItem;
    if (idx == -1)
    {
        return;
    }
    auto dataIdx = GetFileListRepoDataIndex(idx);
    if (-1 == dataIdx)//go parent folder.
    {
        return;
    }
    auto *pItemData = &m_fileData[dataIdx];
    //NxFileData *pItemData = (NxFileData *)m_fileList.GetItemData(idx);

    if (m_projData.m_flag == NXRMC_PROJ_BY_OTHERS)
    {
        pMenu->RemoveMenu(ID_FILECONTEXTMENU_DELETE, MF_BYCOMMAND);
    }
    if (pItemData->itemType == NX_RMC_FILE)
    {
        if (!theApp.IsProjectFileDownloadable(m_projData.m_nID, m_fileData[dataIdx]))
        {
            pMenu->RemoveMenu(ID_FILECONTEXTMENU_DOWNLOAD, MF_BYCOMMAND);
        }
        auto ownerId = m_projData.m_ownerID;
        if (theApp.RestAPIGetCurrentUserID() != ownerId)
        {//We make sure only the owner can view the activities
            pMenu->RemoveMenu(ID_PROJECTCONTEXTMENU_VIEWACTIVITY, MF_BYCOMMAND);
        }
        pMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this, NULL);
    }
    else
    {
        pMenu->RemoveMenu(ID_FILECONTEXTMENU_FILEPROPERTIES, MF_BYCOMMAND);
        pMenu->RemoveMenu(ID_FILECONTEXTMENU_DOWNLOAD, MF_BYCOMMAND);
        pMenu->RemoveMenu(ID_FILECONTEXTMENU_VIEW, MF_BYCOMMAND);
        pMenu->RemoveMenu(ID_PROJECTCONTEXTMENU_VIEWACTIVITY, MF_BYCOMMAND);
        pMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this, NULL);
    }

    *pResult = 0;
}


void CProjectFileList::OnDblclkListRepofiles(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here


    //m_currRepoName.GetWindowText(m_currDir);
    //m_RepoList.GetCurSel();
    if (pNMItemActivate->iItem == -1)
    {
        return;
    }
    int dataIdx = GetFileListRepoDataIndex();
    if (dataIdx == -1) {//go parent folder
        OnBnClickedButtonBack();
        ResetSearch();
        return;
    }
    auto m_selectedFile = m_fileList.GetItemText(pNMItemActivate->iItem, 0);
    OpenFileOrFolder(pNMItemActivate->iItem);
    *pResult = 0;
}



void CProjectFileList::OpenFileOrFolder(int iItem)
{
    int idx = (int)m_fileList.GetItemData(iItem);
    auto *pItemData = &m_fileData[idx];

    if (pItemData->itemType == NX_RMC_FILE)
    {
        OnFilecontextmenuView();
    }
    else
    {
        auto newPath = pItemData->repoFile.GetPath();
        m_currDispDir = pItemData->repoFile.GetDisplayPath().c_str();
        m_currDir = newPath.c_str();
        theApp.ListProjectFiles(m_currDir.GetBuffer(), m_projData);
        ConvertDataToFileData();
        DisplayAllFiles();
        SortColumn(m_iCurSelectedListColumn, true);
        ResetSearch();
    }
}

void CProjectFileList::OnBnClickedButtonBack()
{
    // TODO: Add your control notification handler code here
 
    {
        m_currDir = CombinePath(m_currDir, L"..");
        if (m_currDir != "/")
        {   //Assume pathID end with "/"
            m_currDir += "/";
        }
        m_currDispDir = CombinePath(m_currDispDir, L"..");
        theApp.ListProjectFiles(m_currDir.GetBuffer(), m_projData);
        ConvertDataToFileData();
        DisplayAllFiles();
        SortColumn(m_iCurSelectedListColumn, true);
    }
}


void CProjectFileList::OnBnClickedButtonResetSearch()
{

    theApp.ListProjectFiles(m_currDir.GetBuffer(), m_projData);
    ConvertDataToFileData();
	ResetSearch();
    DisplayAllFiles();
    SortColumn(m_iCurSelectedListColumn, true);
}


void CProjectFileList::OnBnClickedButtonSearch()
{
    // TODO: Add your control notification handler code here
    m_searchEditBox.SetFocus();
    CString searchText;
    m_searchEditBox.GetWindowText(searchText);
    if (searchText.GetLength())
    {
        OnOK();
    }

}

void CProjectFileList::SearchFilesUsingSearchText()
{
    CString wndText;
    m_searchEditBox.GetWindowText(wndText);
    {
        m_searchMode = true;
        m_searchString = wndText;

        SearchProjectFiles(m_currDir.GetBuffer(), m_projData, wndText.GetBuffer());
        ConvertDataToFileData();
        DisplayAllFiles();
        SortColumn(m_iCurSelectedListColumn, true);
        m_resetSearchBtn.ShowWindow(SW_SHOW);
        m_filePath.ShowWindow(SW_HIDE);
    }
}

void CProjectFileList::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    auto pWnd = GetFocus();

    if (pWnd->GetSafeHwnd() == m_searchEditBox.GetSafeHwnd())
    {
        SearchFilesUsingSearchText();
    }
    else if (pWnd->GetSafeHwnd() == m_fileList.GetSafeHwnd())
    {
        auto idx = m_fileList.GetSelectionMark();
        if (idx != -1)
        {
            int dataIdx = GetFileListRepoDataIndex();
            if (-1 == dataIdx)
            {
                return;
            }

            OpenFileOrFolder(idx);
        }

    }

}

void CProjectFileList::ResetSearch()
{

    m_searchEditBox.SetWindowTextW(L"");
    m_resetSearchBtn.ShowWindow(SW_HIDE);
    m_filePath.ShowWindow(SW_SHOW);
    m_searchMode = false;
    m_searchString = L"";
}

void CProjectFileList::SearchProjectFiles(const wstring  currentPath, CProjData &projData, const wstring searchString)
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
    auto res = theApp.m_pSession->ProjectFindFiles(projData.m_nID, parentDir, searchString, &bcancel, files);

    if (!res) {
        TRACE(OutputError(res, L"Fail to list 1st page of files").c_str());
    }
    else {
        std::for_each(files.cbegin(), files.cend(), [&](const NX::RmProjectFile& file) {
            if (file.IsFolder())
            {
                //TRACE(file.GetDisplayPath().c_str());
                projData.m_Dirs.push_back(file);
            }
            else
            {
                //TRACE(file.GetDisplayPath().c_str());
                projData.m_files.push_back(file);
            }
        });
    }

}


void CProjectFileList::OnCancel()
{
    // TODO: Add your specialized code here and/or call the base class

    //CFlatDlgBase::OnCancel();
}


void CProjectFileList::OnStnClickedStaticSearchgroup()
{
    // TODO: Add your control notification handler code here
}


void CProjectFileList::OnPaint()
{
    //CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
    CFlatDlgBase::OnPaint();// for painting messages
}


void CProjectFileList::OnBnClickedButtonGoback2()
{
    GetParent()->PostMessage(WM_NX_PREV_PROJECT, 0, 0);
}


void CProjectFileList::OnBnClickedButtonForward()
{
    // TODO: Add your control notification handler code here
    GetParent()->PostMessage(WM_NX_NEXT_PROJECT, 0, 0);
}


void CProjectFileList::OnBnClickedButtonUpload()
{
    // TODO: Add your control notification handler code here
    // TODO: Add your control notification handler code here
    CFileDialog fileDialog(TRUE,
        NULL,
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"All types(*.*)|*.*||",
        this
    );

    OPENFILENAME& ofn = fileDialog.GetOFN();
    if (fileDialog.DoModal() == IDCANCEL)
		return;

	std::wstring fullPath(ofn.lpstrFile);
	auto pos = fullPath.rfind(L"\\");
	const std::wstring& srcFileName = (pos == std::wstring::npos) ? fullPath : fullPath.substr(pos + 1);
	const std::wstring& srcFileDir = (pos == std::wstring::npos) ? fullPath : fullPath.substr(0, pos);

    DropOneFile(srcFileDir.c_str(), srcFileName.c_str());

}


static void ProgressCallBack(MSG wmdata)
{
    ::SendMessage(wmdata.hwnd, wmdata.message, wmdata.wParam, wmdata.lParam);
}

void CProjectFileList::DropOneFile(CString folderToSave, CString fileToSave)
{
	std::wstring name(fileToSave.GetBuffer() ? fileToSave.GetBuffer() : L"");
	if(NX::iend_with<wchar_t>(name, L".nxl")) {
        MessageBox(theApp.LoadString(IDS_CONNOT_UPLOAD_NXL)/*L"Cannot upload already protected .nxl file."*/, theApp.LoadString(IDS_APP_TITLE)/*L"SkyDRM"*/, MB_ICONWARNING | MB_OK);
        return;
    }

	std::wstring src(folderToSave.GetBuffer());
	if (L'\\' != src[src.length() - 1]) src.append(L"\\");
	src.append(name);

    wstring curDir = m_currDir.GetBuffer();
	if (curDir.empty() || (!curDir.empty() && curDir[curDir.length() - 1] != L'/'))
		curDir.append(L"/");
    BOOL bProtect = FALSE;

    MSG wmmes;
    wmmes.hwnd = GetSafeHwnd();
    wmmes.message = WM_NX_REFRESH_PROJFILELIST;
    wmmes.wParam = 0;
    wmmes.lParam = 0;
    if (theApp.UploadFileToProject(GetSafeHwnd(), m_projData.m_nID, curDir, src, name, ProgressCallBack, &wmmes)) {
    }
}


void CProjectFileList::OnBnClickedButtonAddDir()
{
    // TODO: Add your control notification handler code here
    NewFolderDlg dlg(CWnd::FromHandle(GetSafeHwnd()));
    for (size_t st = 0; st < m_fileData.size(); st++) {
        if (m_fileData[st].itemType == NX_RMC_FOLDER) {
            dlg.m_strExistFolders.push_back(m_fileData[st].fileName.GetBuffer());
        }
    }
    if (dlg.DoModal() != IDOK)
        return;

    if (theApp.CreateProjectFolder(m_projData.m_nID, m_currDir.GetBuffer(), dlg.m_strFolderName.GetBuffer())) {
        OnBnClickedButtonRefresh();
    }
}


void CProjectFileList::OnBnClickedButtonRefresh()
{
    // TODO: Add your control notification handler code here
    if (m_searchMode == true)
    {
        SearchFilesUsingSearchText();
    }
    else
    {
        RefreshPage();
    }
}


void CProjectFileList::OnFilecontextmenuFileproperties()
{
    // TODO: Add your command handler code here

    auto idx = m_fileList.GetSelectionMark();
    if (idx != -1)
    {
        auto dataIdx = m_fileList.GetItemData(idx);
        auto fileData = m_fileData[dataIdx];

        theApp.ShowProjectFileProperty(m_projData.m_nID, fileData);

    }
}


void CProjectFileList::OnFilecontextmenuDelete()
{
    // TODO: Add your command handler code here
    auto idx = m_fileList.GetSelectionMark();
    if (idx != -1)
    {
        auto dataIdx = m_fileList.GetItemData(idx);
        auto fileData = m_fileData[dataIdx];

        RepositoryFileInfo repoFileInfo;
        MyVaultFileInfo mvFileInfo;
        wstringstream txtOut;
        std::wstring filename;
        std::wstring filedate;
        std::wstring filesize;
        BOOL bdirectory;

        filename = fileData.fileName;
        filedate = GetFileTimeString(fileData.dt);
        filesize = GetSpaceString(fileData.fileSize);
        bdirectory = (fileData.itemType == NX_RMC_FOLDER);

        CDeleteDlg dlg(bdirectory, CWnd::FromHandle(GetSafeHwnd()));

        dlg.m_strFileName = filename.c_str();
        dlg.m_strFileDate = filedate.c_str();
        dlg.m_strFileSize = filesize.c_str();

        if (dlg.DoModal() == IDCANCEL) {
            return;
        }

        if (theApp.DeleteProjectFile(m_projData.m_nID, fileData.filePath))
        {
            OnNxRefreshProjfilelist(0, 0);
        }
    }
}


void CProjectFileList::OnFilecontextmenuDownload()
{
    // TODO: Add your command handler code here
    auto idx = m_fileList.GetSelectionMark();
    if (idx != -1)
    {
        auto dataIdx = m_fileList.GetItemData(idx);
        auto fileData = m_fileData[dataIdx];

		HWND hwndOwner = this->GetSafeHwnd();
		WCHAR wzDisplayName[MAX_PATH] = { 0 };
		CString wzChooseFolderName = theApp.LoadString(IDS_CHOOSE_DOWNLOAD_FOLDER);
		BROWSEINFOW bi = {
			hwndOwner,
			NULL,
			wzDisplayName,
			wzChooseFolderName.GetBuffer()/*L"Choose download target folder"*/,
			BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE,
			NULL,
			0,
			0
		};

		std::wstring targetFolder;
		PCIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
		if (NULL == pidl) {
			return;
		}
		if (!SHGetPathFromIDListW(pidl, NX::wstring_buffer(targetFolder, MAX_PATH))) {
			return;
		}

        theApp.DownloadProjectFile(m_projData.m_nID, fileData.filePath, targetFolder);
    }
}


void CProjectFileList::OnFilecontextmenuView()
{
    auto idx = m_fileList.GetSelectionMark();
    if (idx != -1)
    {
        auto dataIdx = m_fileList.GetItemData(idx);
        auto fileData = m_fileData[dataIdx];

        auto displayPath = fileData.repoFile.GetDisplayPath();
        auto pos = displayPath.find_first_of(L'/');
        auto dispName = displayPath.substr(pos + 1);
        theApp.OpenProjFile(m_projData, fileData.repoFile.GetPath().c_str(), dispName);
    }
}

BOOL CProjectFileList::IsFileInfoAvailable()
{
    if (!IsWindowVisible())
    {
        return FALSE;
    }
    BOOL bdirectory;
    
    int idx = m_fileList.GetSelectionMark();
    if (-1 == idx)
        return FALSE;
   
    auto dataIdx = m_fileList.GetItemData(idx);
    auto fileData = m_fileData[dataIdx];
    bdirectory = (fileData.itemType == NX_RMC_FOLDER);

    return (!bdirectory);
}

BOOL CProjectFileList::IsFileDeleteable()
{
    if (!IsWindowVisible())
    {
        return FALSE;
    }

    int idx = m_fileList.GetSelectionMark();
    if (-1 == idx)
        return FALSE;

    if (m_projData.m_flag == NXRMC_PROJ_BY_OTHERS)
    {
        return FALSE;
    }
    return TRUE;
}



void CProjectFileList::OnProjectcontextmenuViewactivity()
{
    int idx = m_fileList.GetSelectionMark();
    if (-1 == idx)
        return;

    idx = (int)m_fileList.GetItemData(idx);
    auto fileData = m_fileData[idx];
    CActivityLogDlg dlg(this);
    if (!theApp.GetFileActivityLog(fileData.fileName.GetBuffer(), fileData.duid, dlg.m_FileActivities))
        return;
    dlg.m_strFileName = fileData.fileName.GetBuffer();
    dlg.DoModal();
}


afx_msg LRESULT CProjectFileList::OnNxRefreshProjfilelist(WPARAM wParam, LPARAM lParam)
{
	if (m_projData.m_flag == NXRMC_PROJ_NOT_VALID)
	{
		return 0;
	}

	theApp.m_dlgProjMain.m_summaryDlg.OnNxRefreshProjfilelist(0, 0);
    return 0;
}


void CProjectFileList::OnLvnItemchangedListRepofiles(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if (pNMLV->uNewState == 0)
        m_fileList.SetSelectionMark(-1);

    *pResult = 0;
}


int CProjectFileList::ResizeWindow(int cx, int cy)
{
    if (cx == 0 || cy == 0)
    {
        return cy;
    }
    cx -= m_topPos.x;
    cy -= m_topPos.y;

    CRect rc;
    GetWindowRect(&rc);
    SetWindowPos(&theApp.m_dlgProjMain, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
    MoveWindowPos(&m_stcHorizLine, cx - 1, 0, TRUE);
    MoveWindowPos(&m_stcVertLine, 0, cy - 1, TRUE);

    BOOL bResetVisible = m_resetSearchBtn.IsWindowVisible();


    for (int i = 1; i <= 9; i++)
    {
        m_ctrlData[i].pCtrl->ShowWindow(SW_HIDE);
    }

    MoveRightHorizontally(1, cx, m_ctrlData);
    MoveRightHorizontally(2, cx, m_ctrlData);
    MoveRightHorizontally(3, cx, m_ctrlData);
    MoveRightHorizontally(4, cx, m_ctrlData);
    MoveRightHorizontally(5, cx, m_ctrlData);
    MoveRightHorizontally(6, cx, m_ctrlData);
    ShrinkRightHorizontally(7, cx, m_ctrlData);
    ShrinkRightHorizontally(8, cx, m_ctrlData);
    MoveRightHorizontally(9, cx, m_ctrlData);

    for (int i = 1; i <= 9; i++)
    {
        if (i == 4 && !bResetVisible)
        {   //We do not show reset button when it was hidden originally
            continue;
        }

        m_ctrlData[i].pCtrl->ShowWindow(SW_SHOW);
    }


    m_fileList.SetWindowPos(this, 0, 0, m_ctrlData[10].rc.Width() + (cx - m_ctrlData[0].rc.right), 
                        m_ctrlData[10].rc.Height() + (cy - m_ctrlData[0].rc.bottom), SWP_NOMOVE | SWP_NOZORDER);
    ResizeRepoFileListControl();

    return cy;
}

int CProjectFileList::GetFileListRepoDataIndex(int nsel)
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


void  CProjectFileList::SetAllOrignRects()
{
    CtrlSizeData ctrlSize;
    CRect rc;
    GetClientRect(&rc);

    ctrlSize.rc = rc;
    ctrlSize.pCtrl = this;
    m_ctrlData.push_back(ctrlSize);
    //SetChildRect(&)
    SetChildRect(&m_btnAddDir, m_ctrlData);
    SetChildRect(&m_searchBtn, m_ctrlData);
    SetChildRect(&m_searchEditBox, m_ctrlData);
    SetChildRect(&m_resetSearchBtn, m_ctrlData);
    SetChildRect(&m_searchSep, m_ctrlData);
    SetChildRect(&m_btnRefresh, m_ctrlData);
    SetChildRect(&m_navBand, m_ctrlData);
    SetChildRect(&m_projName, m_ctrlData);
    SetChildRect(&m_uploadBtn, m_ctrlData);
    SetChildRect(&m_fileList, m_ctrlData);
    
}


void CProjectFileList::ResizeRepoFileListControl()
{
    CRect rcList;
    m_fileList.GetWindowRect(&rcList);
    int width = rcList.Width();
    m_fileList.SetColumnWidth(0, width / 2);
    m_fileList.SetColumnWidth(1, width / 6);
    m_fileList.SetColumnWidth(2, width / 12);
    m_fileList.SetColumnWidth(3, width / 12);
    m_fileList.SetColumnWidth(4, width / 6);
}



BOOL CProjectFileList::OnEraseBkgnd(CDC* pDC)
{
    

    return TRUE;
}

BOOL CProjectFileList::IsWindowVisible()
{
    return m_hWnd != 0 && CWnd::IsWindowVisible();
}