// ProjSummary.cpp : implementation file
//

#include "stdafx.h"
#include "ProjSummary.h"
#include "afxdialogex.h"
#include "resource.h"
#include "nxrmRMC.h"
#include "nxrmc.h"

#include <iomanip>

ProjFile demo_files[] =
{
    {L"nextlabsFileName1",L"Created at 4/23/2017 10:00pm"},
    { L"nextlabsFileName2",L"Created at 4/23/2017 10:00pm" },
    { L"nextlabsFileName3",L"Created at 4/23/2017 10:00pm" }, 
    { L"nextlabsFileName4",L"Created at 4/23/2017 10:00pm" }, 
};
ProjMember demo_members[] =
{
    { L"nextlabs member1",L"Created at 4/23/2017 10:00pm" },
    { L"texus member2",L"Created at 4/23/2017 10:00pm" },
    { L"California member3",L"Created at 4/23/2017 10:00pm" },
    { L"navada memver4",L"Created at 4/23/2017 10:00pm" },
};

//extern void ListProjectFiles(wstring  currentPath, CProjData &projData);

void SearchProjectFiles(wstring currentPath, CProjData & projData);

extern bool SeparatePathAndName(const CString fullPath, CString &path, CString &name, const wchar_t sepChar = L'\\');

CString GetFileTimeString(NX::time::datetime dt)
{
	SYSTEMTIME st;
	dt.to_systemtime(&st, true);
	return theApp.FormatFileTimeString(st).c_str();

}

//
// CProjSummary dialog

IMPLEMENT_DYNAMIC(CProjSummary, CFlatDlgBase)

CProjSummary::CProjSummary(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_PROJ_SUMMARY, pParent)
{

}

CProjSummary::~CProjSummary()
{
}

void CProjSummary::SetProject()
{
    SetProject(m_projData);
}


void CProjSummary::SetProject(const CProjData & projData)
{
    m_projData = projData;

	if (projData.m_flag == NXRMC_PROJ_NOT_VALID)
	{
		return;
	}
    m_members.clear();
    auto desc = m_projData.m_desc;
    m_desc_val.SetWindowTextW(desc);
    ClearFileList();
    ClearMemberList();
    theApp.MessageYield();

    auto Cur = SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(true);
    auto  m_ownerId = m_projData.m_ownerID;
    theApp.FindProjectMembers(m_projData, L"", m_ownerId, m_members);
    theApp.m_dlgProjMain.m_btnPeople.SetCircleNumber((int)m_members.size());
    theApp.ListRecentProjectFiles(m_projData, MAX_ITEMS);
    ShowFileList();
    ShowMemberList();
    SetCursor(Cur);
    ShowCursor(true);
}

void CProjSummary::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_DESC, m_desc_val);
    DDX_Control(pDX, IDC_STATIC_DESCRIPTION, m_descTitle);
    DDX_Control(pDX, IDC_BUTTON_ADDFILE, m_addFile);
    DDX_Control(pDX, IDC_BTN_VIEW_ALL, m_viewAllFiles);
    DDX_Control(pDX, IDC_BUTTON_ADDFILE2, m_invite);
    DDX_Control(pDX, IDC_BTN_VIEW_ALL2, m_viewAllMember);
    DDX_Control(pDX, IDC_STATIC_PEOPLE, m_peopleLabel);
    DDX_Control(pDX, IDC_STATIC_RECENTFILES, m_fileLabel);
    DDX_Control(pDX, IDC_STATIC_NOFILE, m_noFileLabel);
    DDX_Control(pDX, IDC_ST_HLINE1, m_baseSepLine);
    DDX_Control(pDX, IDC__VERT_SEPARATOR, m_stcVertLine);
    DDX_Control(pDX, IDC_STATIC_GROUP, m_grpBox);
    DDX_Control(pDX, IDC_ST_HLINE10, m_memberBaseLine);
}


BEGIN_MESSAGE_MAP(CProjSummary, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
    ON_STN_CLICKED(IDC_STATIC_FILE_DESC8, &CProjSummary::OnStnClickedStaticFileDesc8)
    ON_STN_CLICKED(IDC_STATIC_DESC_GROUP, &CProjSummary::OnStnClickedStaticDescGroup)
    ON_BN_CLICKED(IDC_BUTTON_INVITEMEMBER, &CProjSummary::OnBnClickedButtonInvitemember)
    ON_BN_CLICKED(IDC_BTN_VIEW_ALL_MEMBER, &CProjSummary::OnBnClickedBtnViewAllMember)
    ON_BN_CLICKED(IDC_BUTTON_ADDFILE, &CProjSummary::OnBnClickedButtonAddfile)
    ON_BN_CLICKED(IDC_BTN_VIEW_ALL, &CProjSummary::OnBnClickedBtnViewAll)
    ON_WM_PAINT()
    ON_STN_CLICKED(IDC_ST_HLINE9, &CProjSummary::OnStnClickedStHline9)
    ON_MESSAGE(WM_NX_REFRESH_PROJFILELIST, &CProjSummary::OnNxRefreshProjfilelist)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CProjSummary message handlers
const int mbInBytes = 1024 * 1024;
const int kbInBytes = 1024;

wstring GetSpaceString(__int64 bytes)
{
    CString mbfStr = theApp.LoadString(IDS_FMT_F_MB)/*L"%.2f MB"*/;
    CString kbfStr = theApp.LoadString(IDS_FMT_F_KB)/*L"%.2f KB"*/;
    CString byteStr = theApp.LoadString(IDS_FMT_D_BYTES)/*L"%d Bytes"*/;
    float  fSize = (float)bytes / mbInBytes;
    if (fSize >= 1)
    {
        //Output mb in format
        CString dispStr;
        dispStr.Format(mbfStr, fSize);
        return dispStr.GetBuffer();

    }
    else if ((fSize = (float)bytes / kbInBytes) >= 1)
    {
        //Output kb in format
        CString dispStr;
        dispStr.Format(kbfStr, fSize);
        return dispStr.GetBuffer();
    }
    else
    {
        //output in bytes format
        CString dispStr;
        dispStr.Format(byteStr, bytes);
        return dispStr.GetBuffer();
    }
    return L"";
}



void CProjSummary::SortedMemberDescendByTime()
{
    std::sort(m_projData.m_Simmembers.begin(), m_projData.m_Simmembers.end(),
        [](const NX::RmProjectMemberSimple &a, const NX::RmProjectMemberSimple &b)
    {
        return a.GetCreationTime() > b.GetCreationTime();
    });
}


void CProjSummary::ClearFileList()
{
    int i = 0;


    m_noFileLabel.ShowWindow(SW_HIDE);
    m_baseSepLine.ShowWindow(SW_SHOW);
    while (i < MAX_ITEMS)
    { //Hide All of the windows
        GetDlgItem(m_fileIDArr[i].btnID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_fileIDArr[i].descID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_fileIDArr[i].sepID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_fileIDArr[i].mbID)->ShowWindow(SW_HIDE);
        i++;
    }
}


void CProjSummary::ShowFileList()
{
    int i=0;

    bool bShowNoFileLabel = !m_projData.m_files.size();

    m_noFileLabel.ShowWindow(bShowNoFileLabel ? SW_SHOW : SW_HIDE);
    m_baseSepLine.ShowWindow(bShowNoFileLabel ? SW_HIDE : SW_SHOW);
    m_fileData.clear();
    for (int n = 0; n < (int)m_projData.m_files.size(); n++)
    {
        //Show all Windows here
        auto file = m_projData.m_files[n];
        if (!file.IsFolder())
        {
            CString fName = file.GetDisplayPath().c_str();
            CString path, name;
            SeparatePathAndName(fName, path, name, L'/');
            SetDlgItemText(m_fileIDArr[i].btnID, name);
            NX::time::datetime dt = NX::time::datetime::from_unix_time(file.GetCreationTime());
            auto timeText = GetFileTimeString(dt);
            auto resultTime = theApp.LoadString(IDS_ADDED_PREFIX)/*L"Added "*/ + timeText;
            SetDlgItemText(m_fileIDArr[i].descID, resultTime);
            auto bytes = file.GetSize();
            auto spaceStr = GetSpaceString(bytes);
            SetDlgItemText(m_fileIDArr[i].mbID, spaceStr.c_str());
            GetDlgItem(m_fileIDArr[i].btnID)->ShowWindow(SW_SHOW);
            GetDlgItem(m_fileIDArr[i].descID)->ShowWindow(SW_SHOW);
            GetDlgItem(m_fileIDArr[i].sepID)->ShowWindow(SW_SHOW);
            GetDlgItem(m_fileIDArr[i].mbID)->ShowWindow(SW_SHOW);
            i++;
            m_fileData.push_back(file);
        }
        if (i >= MAX_ITEMS)
        {
           break;
        }
    }
    while (i < MAX_ITEMS)
    { //Hide rest of the windows
        GetDlgItem(m_fileIDArr[i].btnID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_fileIDArr[i].descID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_fileIDArr[i].sepID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_fileIDArr[i].mbID)->ShowWindow(SW_HIDE);
        i++;
    }
}

void CProjSummary::ClearMemberList()
{
    int i = 0;
    while (i < MAX_ITEMS)
    { //Hide rest of the windows
        GetDlgItem(m_memberIDArr[i].btnID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_memberIDArr[i].descID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_memberIDArr[i].sepID)->ShowWindow(SW_HIDE);
        i++;
    }
}


void CProjSummary::ShowMemberList()
{

    int i = 0;
    SortedMemberDescendByTime();


    for (int n = 0; n < (int)m_members.size(); n++)
    {
        //Show all Windows here
        auto member = m_members[n];
        CString memberName = member.memberName.c_str();
        SetDlgItemText(m_memberIDArr[i].btnID, memberName);
        wstring resultTime = theApp.LoadString(IDS_JOINED_PREFIX).GetBuffer()/*L"Joined "*/ + member.desc;
        SetDlgItemText(m_memberIDArr[i].descID, resultTime.c_str());
        GetDlgItem(m_memberIDArr[i].btnID)->ShowWindow(SW_SHOW);
        GetDlgItem(m_memberIDArr[i].descID)->ShowWindow(SW_SHOW);
        GetDlgItem(m_memberIDArr[i].sepID)->ShowWindow(SW_SHOW);
        i++;


        if (i >= MAX_ITEMS)
        {
            break;
        }
    }
    while (i < MAX_ITEMS)
    { //Hide rest of the windows
        GetDlgItem(m_memberIDArr[i].btnID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_memberIDArr[i].descID)->ShowWindow(SW_HIDE);
        GetDlgItem(m_memberIDArr[i].sepID)->ShowWindow(SW_HIDE);
        i++;
    }

}


void CProjSummary::InitializeButtons()
{
    int i;
    for(i=0; i<MAX_ITEMS; i++)
    {
        m_fileBtns[i].SubclassDlgItem(m_fileIDArr[i].btnID,this);
        m_fileBtns[i].SetColors(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_BLACK);
        m_fileBtns[i].SetTextAlignment(BS_LEFT);
        m_fileBtns[i].SetMouseCursorHand();
    }
    for (i = 0; i<MAX_ITEMS; i++)
    {
        m_memberBtns[i].SubclassDlgItem(m_memberIDArr[i].btnID, this);
        m_memberBtns[i].SetColors(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_BLACK);
        m_memberBtns[i].SetTextAlignment(BS_LEFT);
        //m_memberBtns[i].SetMouseCursorHand();
    }
}

BOOL CProjSummary::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    //m_memberPictures.SetDrawType(NX_DRAWTYPE_USERS_IMAGE);
    GetWindowRect(&m_rcBound);
    ScreenToClient(&m_rcBound);
    //CRect rcTest = m_rcBound;
    //theApp.m_dlgProjMain.ScreenToClient(&rcTest); //this operation done at set scroll operation
    
    AddFontSize(40, m_descTitle, true);
    AddFontSize(40, m_peopleLabel, false);
    AddFontSize(40, m_fileLabel, false);
    m_addFile.SetColors(NX_NEXTLABS_STANDARD_GREEN, NX_NEXTLABS_STANDARD_WHITE);
    m_viewAllFiles.SetColors(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_BLUE);
    m_viewAllFiles.SetTextAlignment(BS_LEFT);
    m_invite.SetColors(NX_NEXTLABS_STANDARD_GREEN, NX_NEXTLABS_STANDARD_WHITE);
    m_viewAllMember.SetColors(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_BLUE);
    m_viewAllMember.SetTextAlignment(BS_LEFT);
    SetBackColor(NX_NEXTLABS_STANDARD_WHITE);
    
    InitializeButtons();
    for (int i = 0; i < MAX_ITEMS; i++)
    {

        //m_fileCtrls[i].file.SubclassDlgItem(m_fileIDArr[i].btnID, this);
        m_fileCtrls[i].date.SubclassDlgItem(m_fileIDArr[i].descID, this);
        m_fileCtrls[i].size.SubclassDlgItem(m_fileIDArr[i].sepID, this);
        m_fileCtrls[i].sep.SubclassDlgItem(m_fileIDArr[i].mbID, this);

        m_memberCtrls[i].date.SubclassDlgItem(m_memberIDArr[i].descID, this);
        m_memberCtrls[i].sep.SubclassDlgItem(m_memberIDArr[i].sepID, this);

    }

    SetAllOrignRects();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CProjSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetBkMode(TRANSPARENT);
        hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
    }

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


void CProjSummary::OnDestroy()
{
    CDialogEx::OnDestroy();
    // TODO: Add your message handler code here
}


void CProjSummary::OnStnClickedStaticFileDesc8()
{
    // TODO: Add your control notification handler code here
}


void CProjSummary::OnStnClickedStaticDescGroup()
{
    // TODO: Add your control notification handler code here
}


void CProjSummary::OnBnClickedButtonInvitemember()
{
    theApp.InvitePeopleToProject(m_projData);
    if (theApp.m_dlgProjMain.m_membersDlg.GetSafeHwnd())
    {
        theApp.m_dlgProjMain.m_membersDlg.OnBnClickedButtonRefresh();
    }
}


void CProjSummary::OnBnClickedBtnViewAllMember()
{
    // TODO: Add your control notification handler code here
    
    auto mainProj = (CProjectMain*)GetParent();
    mainProj->ShowProjPage(NX_PROJ_PEOPLE_PAGE);
}

static void ProgressCallBack(MSG wmdata)
{
    ::SendMessage(wmdata.hwnd, wmdata.message, wmdata.wParam, wmdata.lParam);
}

void CProjSummary::OnBnClickedButtonAddfile()
{
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
    {
        return;
    }
	const std::wstring src(ofn.lpstrFile);
	if (NX::iend_with<wchar_t>(src, L".nxl")) {
		MessageBox(theApp.LoadString(IDS_CONNOT_UPLOAD_NXL)/*L"Cannot upload already protected .nxl file"*/, theApp.LoadString(IDS_APP_TITLE)/*L"SkyDRM"*/, MB_ICONWARNING | MB_OK);
		return;
	}

    MSG wmmes;
    wmmes.hwnd = GetSafeHwnd();
    wmmes.message = WM_NX_REFRESH_PROJFILELIST;
    wmmes.wParam = 0;
    wmmes.lParam = 0;

    std::wstring fullPath(ofn.lpstrFile);
    auto pos = fullPath.rfind(L"\\");
    const std::wstring& srcFileName = (pos == std::wstring::npos) ? fullPath : fullPath.substr(pos + 1);

    theApp.UploadFileToProject(GetSafeHwnd(), m_projData.m_nID, L"/", src, srcFileName, ProgressCallBack, &wmmes);
    
}



void CProjSummary::OnBnClickedBtnViewAll()
{
    // TODO: Add your control notification handler code 
    auto mainProj = (CProjectMain*)GetParent();
    mainProj->ShowProjPage(NX_PROJ_FILES_PAGE);
}


BOOL CProjSummary::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    switch (LOWORD(wParam))
    {
    case IDC_BTN_FILE_NAME1:
    case IDC_BTN_FILE_NAME2:
    case IDC_BTN_FILE_NAME3:
    case IDC_BTN_FILE_NAME4:
    case IDC_BTN_FILE_NAME5:
    case IDC_BTN_FILE_NAME6:
    case IDC_BTN_FILE_NAME7:
    case IDC_BTN_FILE_NAME8:
    {
        auto idx = idToFileIdx.at(LOWORD(wParam));
        auto displayPath = m_fileData[idx].GetDisplayPath();
        auto pos = displayPath.find_first_of(L'/');
        auto dispName = displayPath.substr(pos + 1);
        theApp.OpenProjFile(m_projData, m_fileData[idx].GetPath().c_str(), dispName);
        break;
    }
    default:
        break;
    }

    return CFlatDlgBase::OnCommand(wParam, lParam);
}


void CProjSummary::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class

    //CFlatDlgBase::OnOK();
}


void CProjSummary::OnCancel()
{
    // TODO: Add your specialized code here and/or call the base class

    //CFlatDlgBase::OnCancel();
}


void CProjSummary::OnPaint()
{
    //CPaintDC dc(this); // device context for painting
    // TODO: Add your message handler code here
    CFlatDlgBase::OnPaint();// for painting messages
}


void CProjSummary::OnStnClickedStHline9()
{
    // TODO: Add your control notification handler code here
}


afx_msg LRESULT CProjSummary::OnNxRefreshProjfilelist(WPARAM wParam, LPARAM lParam)
{
    theApp.ListRecentProjectFiles(m_projData, MAX_ITEMS);
    ShowFileList();

    if (theApp.m_dlgProjMain.m_fileDlg.GetSafeHwnd())
    {
        theApp.m_dlgProjMain.m_fileDlg.OnBnClickedButtonRefresh();
    }
    //We have to update project information on 4 pages
    theApp.GetProjectInfo(m_projData.m_nID, m_projData);
    theApp.SetProjectInfoInData(m_projData);
    theApp.m_dlgProjPage.UpdateProjWnd(m_projData);
    theApp.m_homePage.UpdateProjWnd(m_projData);
    if (theApp.m_dlgProjMain.m_switchDlg.GetSafeHwnd())
    {
        theApp.m_dlgProjMain.m_switchDlg.UpdateProjWnd(m_projData);
    }
    theApp.m_dlgRepoPage.m_switchDlg.UpdateProjWnd(m_projData);
    return 0;
}

int CProjSummary::ResizeWindow(int cx, int cy)
{
    //CRect rc;
    //GetWindowRect(&rc);
    cx -= m_topPos.x;
    cy -= m_topPos.y;
    int h = max(cy, m_rcBound.Height());
    SetWindowPos(&theApp.m_dlgProjMain, 0, 0, cx, h, SWP_NOMOVE | SWP_NOZORDER);
    MoveWindowPos(&m_stcVertLine, 0, h - 1, TRUE);

    ScaleControlHorizontally(1, cx, m_ctrlData);
    ScaleLeftHorizontally(2, cx, m_ctrlData);
    ScaleControlHorizontally(3, cx, m_ctrlData);

    ScaleRightHorizontally(4, cx, m_ctrlData);

    ScaleLeftHorizontally(5, cx, m_ctrlData);

    ScaleRightHorizontally(6, cx, m_ctrlData);
    ScaleLeftHorizontally(7, cx, m_ctrlData);
    ScaleLeftHorizontally(8, cx, m_ctrlData);

    ScaleLeftHorizontally(9, cx, m_ctrlData);
    ScaleLeftHorizontally(10, cx, m_ctrlData);
    ScaleControlHorizontally(11, cx, m_ctrlData);
    ScaleControlHorizontally(12, cx, m_ctrlData);


    for (int i = 0; i < MAX_ITEMS; i++)
    { //Hide rest of the windows
        int j = (i << 2);
        ScaleControlHorizontally(j + (int)m_fileItemStart, cx, m_ctrlData);
        ScaleControlHorizontally(j + 1 + (int)m_fileItemStart, cx, m_ctrlData);
        ScaleControlHorizontally(j + 2 + (int)m_fileItemStart, cx, m_ctrlData);
        ScaleControlHorizontally(j + 3 + (int)m_fileItemStart, cx, m_ctrlData);

    }


    for (int i = 0; i < MAX_ITEMS; i++)
    { //Hide rest of the windows
        int j = 3 * i;
        ScaleControlHorizontally(j + (int)m_memberItemStart, cx, m_ctrlData);
        ScaleControlHorizontally(j + 1 + (int)m_memberItemStart, cx, m_ctrlData);
        ScaleControlHorizontally(j + 2 + (int)m_memberItemStart, cx, m_ctrlData);
    }



    return m_rcBound.Height();
}


void  CProjSummary::SetAllOrignRects()
{
    CtrlSizeData ctrlSize;
    CRect rc;
    GetClientRect(&rc);

    ctrlSize.rc = rc;
    ctrlSize.pCtrl = this;
    m_ctrlData.push_back(ctrlSize);
    SetChildRect(&m_grpBox, m_ctrlData);
    SetChildRect(&m_descTitle, m_ctrlData);
    SetChildRect(&m_desc_val, m_ctrlData);

    SetChildRect(&m_addFile, m_ctrlData);
    SetChildRect(&m_viewAllFiles, m_ctrlData);
    SetChildRect(&m_invite, m_ctrlData);
    SetChildRect(&m_viewAllMember, m_ctrlData);
    SetChildRect(&m_peopleLabel, m_ctrlData);
    SetChildRect(&m_fileLabel, m_ctrlData);
    SetChildRect(&m_noFileLabel, m_ctrlData);

    SetChildRect(&m_baseSepLine, m_ctrlData);
    SetChildRect(&m_memberBaseLine, m_ctrlData);

    m_fileItemStart = m_ctrlData.size();
    for (int i = 0; i < MAX_ITEMS; i++)
    { 
        SetChildRect(&m_fileBtns[i], m_ctrlData);
        SetChildRect((&m_fileCtrls[i].date), m_ctrlData);
        SetChildRect((&m_fileCtrls[i].size), m_ctrlData);
        SetChildRect((&m_fileCtrls[i].sep), m_ctrlData);
    }


    m_memberItemStart = m_ctrlData.size();
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        SetChildRect(&m_memberBtns[i], m_ctrlData);
        SetChildRect((&m_memberCtrls[i].date), m_ctrlData);
        SetChildRect((&m_memberCtrls[i].sep), m_ctrlData);
    }
}


BOOL CProjSummary::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    //return CFlatDlgBase::OnEraseBkgnd(pDC
    return TRUE;
}

BOOL CProjSummary::IsWindowVisible()
{
    return m_hWnd != 0 && CWnd::IsWindowVisible();
}