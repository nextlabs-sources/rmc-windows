// NXHomePage.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmc.h"
#include "nxrmRMC.h"
#include "NXHomePage.h"
#include "afxdialogex.h"
#include "RepoLabel.h"
#include "OneProject.h"
#include "ProjByOthersDlg.h"
#include "useredit.h"
#include "ConnectToRepo.h"
#include "OwnerDrawStatic.h"
#include <map>
#include <future>
#include "ShareFileWidget.h"

CString GetInitial(CString fullName);
void InitScrollBars(HWND hWnd, CRect &rc);
void ProtectingLocalFile(const HWND hWnd, const wstring filePath);
void SharingLocalFile(const HWND hWnd, const wstring filePath);

std::map <int, int> repoTypeToIcon =
{
    { NX::RmRepository::DROPBOX, IDI_DROPBOX_BLACK},
    { NX::RmRepository::GOOGLEDRIVE, IDI_GOOGLEDRIVE_BLACK},
    { NX::RmRepository::ONEDRIVE, IDI_ONEDRIVE_BLACK},
    { NX::RmRepository::SHAREPOINTONLINE, IDI_SHAREPOINT_BLACK},
    { NX::RmRepository::SHAREPOINTONPREM, IDI_SHAREPOINT_BLACK },
    { NX::RmRepository::SHAREPOINTCROSSLAUNCH, IDI_SHAREPOINT_BLACK },
    { NX::RmRepository::SHAREPOINTONLINE_CROSSLAUNCH, IDI_SHAREPOINT_BLACK },
};

// CNXHomePage dialog

IMPLEMENT_DYNAMIC(CNXHomePage, CFlatDlgBase)

CNXHomePage::CNXHomePage(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_HOME1, pParent)
{

}

CNXHomePage::~CNXHomePage()
{
}

void CNXHomePage::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_TITLE, m_welcome);
    DDX_Control(pDX, IDC_BUTTON2, m_btnPerson);
    DDX_Control(pDX, IDC_STATIC_MYDRIVE_BULLET, m_myDriveLabel);
    DDX_Control(pDX, IDC_STATIC_MYVAULT_LABEL, m_myVaultLabel);
    DDX_Control(pDX, IDC_BUTTON_MYDRIVE, m_btnMydrive);
    DDX_Control(pDX, IDC_BUTTON_MYVAULT, m_btnMyVault);
    DDX_Control(pDX, IDC_STATIC_MYDRIVEMB, m_stMydriveSize);
    DDX_Control(pDX, IDC_STATIC_MYVAULTMB, m_myVaultSize);
    DDX_Control(pDX, IDC_STATIC_STORAGECHART, m_storageChart);
    DDX_Control(pDX, IDC_STATIC_USEDTYTES, m_usedBytes);
    DDX_Control(pDX, IDC_STATIC_FREEBYTES, m_freeBytes);
    DDX_Control(pDX, IDC_BUTTON_MD_VIEWFILE, m_viewMyDrive);
    DDX_Control(pDX, IDC_BUTTON_MV_VIEWFILE, m_viewMyVault);
    DDX_Control(pDX, IDC_STATIC_REPOLIST, m_connectRepoLabel);
    DDX_Control(pDX, IDC_STATIC_PROJ_ME, m_projCreatedByMeLabel);
    DDX_Control(pDX, IDC_STATIC_PROJ_OTHERS, m_projInvited);
    DDX_Control(pDX, IDC_BUTTON_PROTECT, m_btnProtect);
    DDX_Control(pDX, IDC_BUTTON_SHARE, m_btnShare);
    DDX_Control(pDX, IDC_CONNECT_REPOSITORY, m_btnConnectRepo);
    DDX_Control(pDX, IDC_GOTO_MYSPACE, m_gotoMySpace);
    DDX_Control(pDX, IDC_BUTTON_ARROW_NEXT, m_gotoArrow);
    DDX_Control(pDX, IDC_STATIC_STORAGEGROUP, m_storageGroup);
    DDX_Control(pDX, IDC_BUTTON_VIEWALLPROJ, m_BtnViewAllProj);
    DDX_Control(pDX, IDC_STATIC_HGROUP, m_headerGroup);
    DDX_Control(pDX, IDC_BUTTON_ACTIVATE, m_btnActivateProj);
    DDX_Control(pDX, IDC_ACTIVATE_TEXT, m_strActivate);
    DDX_Control(pDX, IDC_ACTIVATE_GROUP, m_grpActivate);
    DDX_Control(pDX, IDC_STATIC_NO_REPO, m_noRepoLabel);
}


BEGIN_MESSAGE_MAP(CNXHomePage, CDialogEx)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_MV_VIEWFILE, &CNXHomePage::OnBnClickedButtonMvViewMyVaultfile)
	ON_BN_CLICKED(IDC_BUTTON2, &CNXHomePage::OnBnClickedPersonButton)
	ON_BN_CLICKED(IDC_BUTTON_PROTECT, &CNXHomePage::OnBnClickedButtonProtect)
	ON_BN_CLICKED(IDC_BUTTON_SHARE, &CNXHomePage::OnBnClickedButtonShare)
	ON_BN_CLICKED(IDC_CONNECT_REPOSITORY, &CNXHomePage::OnBnClickedConnectRepository)
	ON_BN_CLICKED(IDC_GOTO_MYSPACE, &CNXHomePage::OnBnClickedGotoMyspace)
	ON_BN_CLICKED(IDC_BUTTON_ARROW_NEXT, &CNXHomePage::OnBnClickedButtonArrowNext)
	ON_BN_CLICKED(IDC_BUTTON_MD_VIEWFILE, &CNXHomePage::OnBnClickedButtonMdViewMyDrivefile)
	ON_BN_CLICKED(IDC_BUTTON_VIEWALLPROJ, &CNXHomePage::OnBnClickedButtonViewallproj)
	ON_MESSAGE(WM_NX_REFRESH_PROJ_LIST, OnRefreshProjMessage)
	ON_MESSAGE(WM_NX_REFRESH_REPO_LIST, OnRefreshRepoList)
    ON_BN_CLICKED(IDC_BUTTON_ACTIVATE, &CNXHomePage::OnBnClickedButtonActivate)
    ON_MESSAGE(WM_NX_REFRESH_HOME_PAGE, &CNXHomePage::OnNxRefreshHomePage)
	ON_WM_DESTROY()
    ON_MESSAGE(WM_NX_TIMED_REFRESH_PROJLIST, &CNXHomePage::OnNxTimedRefreshProjlist)
END_MESSAGE_MAP()


// CNXHomePage message handlers


BOOL CNXHomePage::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    for (int i = 0; i <(int)drvTypeArray.size(); i++)
    {
        switch (drvTypeArray[i].storType)
        {
        case REPO_GOOGLE:
            drvTypeArray[i].captionText = theApp.LoadString(IDS_GOOGLE_DRIVE);
            break;
        case REPO_ONEDRIVE:
            drvTypeArray[i].captionText = theApp.LoadString(IDS_ONEDRIVE);
            break;
        case REPO_DROPBOX:
            drvTypeArray[i].captionText = theApp.LoadString(IDS_DROPBOX);
            break;
        }
    }

    m_repoRowCnt = 2;
    SetBackColor(NX_NEXTLABS_STANDARD_WHITE);
    AddFontSize(80, m_welcome, false);
    AddFontSize(30, m_connectRepoLabel, false);
    AddFontSize(30, m_projCreatedByMeLabel, false);
    AddFontSize(30, m_projInvited, false);
   
    //stMydriveSize
    m_viewMyDrive.SetTextAlignment(BS_LEFT);
    m_viewMyDrive.SetColors(RGB(255, 255, 255),NX_NEXTLABS_STANDARD_BLUE);
    m_viewMyVault.SetTextAlignment(BS_LEFT);
    m_viewMyVault.SetColors(RGB(255, 255, 255),NX_NEXTLABS_STANDARD_BLUE);
    m_btnMydrive.SetImages(IDB_PNG_MYDRIVE_GRAY, 0, 0, NX_NEXTLABS_STANDARD_WHITE);
    m_btnMyVault.SetImages(IDB_PNG_MYVAULTGRAY, 0, 0, NX_NEXTLABS_STANDARD_WHITE);

    //m_btnPerson.SetMouseCursorHand();

    m_gotoMySpace.SetColors(RGB(255, 255, 255),NX_NEXTLABS_STANDARD_BLUE);
    m_gotoArrow.SetImages(IDB_PNG_NEXT, 0, 0, NX_NEXTLABS_STANDARD_WHITE);

    m_BtnViewAllProj.SetTextAlignment(BS_RIGHT);
    m_BtnViewAllProj.SetColors(RGB(255, 255, 255), NX_NEXTLABS_STANDARD_BLUE);
    //m_btnMyVault.SetMouseCursorHand();

    m_btnProtect.SetImages(IDB_PNG_PROTECTFILE, 0, 0, NX_NEXTLABS_STANDARD_WHITE);;
    m_btnShare.SetImages(IDB_PNG_SHAREFILE, 0, 0, NX_NEXTLABS_STANDARD_WHITE);
    m_btnConnectRepo.SetImages(IDB_PNG_CONNECTREPO, 0, 0, NX_NEXTLABS_STANDARD_WHITE);


    m_myDriveLabel.SetBulletColor(NX_NEXTLABS_STANDARD_GREEN);
    m_myVaultLabel.SetBulletColor(RGB(79, 79, 79));

    UpdateStorageInfo();
    UpdateUserInfo();
	InitialRepoWindows();
	InitialProjWindows();
    DisplayProjectList();

    UpdateRepositoryList(); //This one will move all controls down if it is growing.

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCELPTION: OCX Property Pages should return FALSE
}

void CNXHomePage::RefreshPage()
{

    UpdateStorageInfo();
    UpdateUserInfo();
    DisplayProjectList();
    UpdateRepositoryList(); //This one will move all controls down if it is growing.
}

void CNXHomePage::UpdateStorageInfo()
{
    CString mbStr = theApp.LoadStringW(IDS_FMT_D_MB);
    CString mbfStr = theApp.LoadStringW(IDS_FMT_F_MB); 

    int megaByte = 1024 * 1024;
    auto totalCap = (float)theApp.m_total / megaByte;
    auto totalUsed = (float)theApp.m_used / megaByte;
    auto totalMyVault = (float)theApp.m_myvaultused / megaByte;
    CString dispStr;
    auto freeMB = totalCap - totalUsed;
    if (freeMB < 0)
    {
        freeMB = 0;
    }
    dispStr.Format(mbfStr, freeMB);
    dispStr += theApp.LoadStringW(IDS_SUFFIX_FREE);
    m_freeBytes.SetWindowText(dispStr);
    dispStr.Format(mbfStr, totalUsed);
    dispStr += theApp.LoadStringW(IDS_SUFFIX_USED);
    m_usedBytes.SetWindowText(dispStr);
    dispStr.Format(mbfStr, totalMyVault);
    m_myVaultSize.SetWindowText(dispStr);
    auto myDriveSize = totalUsed - totalMyVault;
    dispStr.Format(mbfStr, myDriveSize);
    m_stMydriveSize.SetWindowText(dispStr);

    m_storageChart.SetChartColorsAndInfo(RGB(79, 79, 79), NX_NEXTLABS_STANDARD_GREEN, NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_GRAY_BORDER,
        totalCap, totalMyVault, totalUsed - totalMyVault);

} 

void CNXHomePage::UpdateUserInfo()
{
    CString welcomeStr = theApp.LoadStringW(IDS_FMT_S_WELCOME);
    CString fullStr;
    fullStr.Format(welcomeStr, theApp.m_usrName.c_str());
    m_welcome.SetWindowText(fullStr);
    auto initial = GetInitial(theApp.m_usrName.c_str());
    m_btnPerson.SetWindowText(initial);

    RmColorSchemeEntry clrScheme;
    if (initial.GetLength())
    {
        clrScheme = GetColorScheme(initial.GetAt(0));
    }
    else
    {
        clrScheme = GetColorScheme(L' '); //Get default color here
    }

    m_btnPerson.SetColorsCircle(clrScheme.bg, clrScheme.fg);

}

#define NX_REPO_WINDOW_TO_SPACE_RATIO   5 / 4
#define MAXMIUM_REPOLIST_NUM		6
void CNXHomePage::InitialRepoWindows()
{
	CRect rc;
	m_repoLabels.clear();

	m_connectRepoLabel.GetWindowRect(&rc);
	ScreenToClient(&rc);
	vector<RepositoryInfo> repoArr;

	CRect rcProj;
	RepositoryInfo rinfo;
	rinfo.Type = NX::RmRepository::MYDRIVE;
	rinfo.AccountName = theApp.LoadStringW(IDS_RESERVED).GetBuffer();
	rinfo.Name = theApp.LoadStringW(IDS_RESERVED).GetBuffer();
	for (int i = 0; i < MAXMIUM_REPOLIST_NUM; i++)
	{
		CRepoLabel *pRepoLabel = new CRepoLabel(rinfo);
		pRepoLabel->Create(IDD_DIALOG_REPOSOTORY, this);

		pRepoLabel->GetWindowRect(&rcProj);

		int nLeft = 0;
		int nTop = 0;

		pRepoLabel->SetWindowPos(this, rc.left + (i % 3)* rcProj.Width() * 21 / 20,
			rc.bottom + (i / 3)* rcProj.Height() * NX_REPO_WINDOW_TO_SPACE_RATIO, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		pRepoLabel->ShowWindow(SW_HIDE);
		m_repoLabels.push_back(pRepoLabel);
	}

	m_repoHeight = rcProj.Height() *NX_REPO_WINDOW_TO_SPACE_RATIO;

}

void CNXHomePage::UpdateRepositoryList()
{
    CRect rc;

    m_connectRepoLabel.GetWindowRect(&rc);
    ScreenToClient(&rc);
    vector<RepositoryInfo> repoArr;
    theApp.GetRepositoryArray(repoArr);

	if (repoArr.size() > 1)
    {
        m_noRepoLabel.ShowWindow(SW_HIDE);
    }
    else
    {
        m_noRepoLabel.ShowWindow(SW_SHOW);
    }
    int maxRepo = 0;   //List 6 the most at this time.

	if (((int)repoArr.size() - 1) > MAXMIUM_REPOLIST_NUM)
		maxRepo = MAXMIUM_REPOLIST_NUM;
	else if (((int)repoArr.size() - 1) > 0)
		maxRepo = (int)repoArr.size() - 1;

    //Here the first one is my drive
    CRect rcProj(0,0,0,0);
    for (int i = 0; i < MAXMIUM_REPOLIST_NUM; i++)
    {
		CRepoLabel *pRepoLabel = (CRepoLabel *)m_repoLabels[i];
		if (i < maxRepo) {
			pRepoLabel->UpdateRepoInfo(repoArr[i + 1]);
			pRepoLabel->GetWindowRect(&rcProj);

			int nLeft = 0;
			int nTop = 0;

			pRepoLabel->SetWindowPos(this, rc.left + (i % 3)* rcProj.Width() * 21 / 20,
				rc.bottom + (i / 3)* rcProj.Height() * NX_REPO_WINDOW_TO_SPACE_RATIO, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			pRepoLabel->ShowWindow(SW_SHOW);
		}
		else {
			pRepoLabel->ShowWindow(SW_HIDE);
		}
    }

    int oldRowCount = m_repoRowCnt;

    m_repoRowCnt = (2 + maxRepo) / 3;

    int deltaY = m_repoRowCnt - oldRowCount;

    if (deltaY != 0)
    {
        MoveAllControls(deltaY * m_repoHeight);
    }
}

void CNXHomePage::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CFlatDlgBase::OnPaint() for painting messages
}


BOOL CNXHomePage::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CFlatDlgBase::OnEraseBkgnd(pDC);
}


HBRUSH CNXHomePage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        auto ctriID = pWnd->GetDlgCtrlID();
        if (ctriID == IDC_STATIC_STORAGELABEL || ctriID == IDC_STATIC_USEDTYTES || 
            ctriID == IDC_STATIC_FREEBYTES || ctriID == IDC_STATIC_STORAGEGROUP)
        {
            pDC->SetTextColor(RGB(0, 0, 0));
            pDC->SetBkMode(TRANSPARENT);
            hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
        }
    }

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}

CString GetInitial(CString fullName)
{
    CString initial = L"";
    int pos = 0;
    pos = fullName.Find(L' ');
    if (pos == -1)
    {
        CString first = fullName;
        first.TrimLeft();
        initial.AppendChar(first[0]);
        initial.MakeUpper();
        return initial;
    }

    auto firstName = fullName.Left(pos);
    firstName.TrimLeft();
    initial.AppendChar(firstName[0]);
    pos = fullName.ReverseFind(L' ');
    auto beforeLast = fullName.Left(pos);
    beforeLast.TrimLeft();
    auto lastName = fullName.Right(fullName.GetLength() - pos);
    lastName.TrimLeft();
    if (beforeLast.GetLength() && lastName.GetLength())
    {
        initial.AppendChar(lastName[0]);
    }
    initial.MakeUpper();
    return initial;
}

#define MAXMIUM_PROJECT_NUM			3
#define MAXMIUM_INVITATION_NUM		2

void CNXHomePage::InitialProjWindows(void)
{
	m_myProjArr.clear();
	m_otherProjArr.clear();
	CProjData projdata;
	projdata.m_initials.clear();
	projdata.m_fileCount = 0;
	projdata.m_imembercount = 0;
	projdata.m_projName = theApp.LoadStringW(IDS_RESERVED);
	int i;
	projdata.m_flag = NXRMC_PROJ_BY_ME;
	for (i = 0; i < MAXMIUM_PROJECT_NUM; i++)
	{
		COneProjDlg *pOneProjDlg = new COneProjDlg(projdata);
		pOneProjDlg->Create(IDD_DIALOG_ONEPROJ, this);
		pOneProjDlg->ShowWindow(SW_HIDE);
		m_myProjArr.push_back(pOneProjDlg);
	}

	projdata.m_flag = NXRMC_FROM_BY_OTHERS_PENDING;
	projdata.m_invitorName = theApp.LoadStringW(IDS_RESERVED);
	projdata.m_ownerName = theApp.LoadStringW(IDS_RESERVED);
	for (i = 0; i < MAXMIUM_INVITATION_NUM; i++)
	{
		CProjByOthersDlg *pOtherProjDlg = new CProjByOthersDlg(projdata);
		pOtherProjDlg->Create(IDD_PROJ_BY_OTHERS, this);
		pOtherProjDlg->ShowWindow(SW_HIDE);
		m_otherProjArr.push_back(pOtherProjDlg);
	}

	projdata.m_flag = NXRMC_PROJ_BY_OTHERS;
	for (i = 0; i < MAXMIUM_PROJECT_NUM; i++)
	{
		CProjByOthersDlg *pOtherProjDlg = new CProjByOthersDlg(projdata);
		pOtherProjDlg->Create(IDD_PROJ_BY_OTHERS, this);
		pOtherProjDlg->ShowWindow(SW_HIDE);
		m_otherProjArr.push_back(pOtherProjDlg);
	}

}
void CNXHomePage::DisplayProjectList()
{
    CRect rc;
    //int nMaxPrjInRow;
    int nMyProjCount = (int)theApp.GetProjectByMeCount();
    int nOtherProjCount = (int)theApp.GetProjectByOthersCount();
    int nInvitesCount = (int)theApp.GetInvitationCount();

	if (nInvitesCount > MAXMIUM_INVITATION_NUM)//show maximum 2 invitiations
		nInvitesCount = MAXMIUM_INVITATION_NUM;

    CRect rcProj; //use to caculate position for new window;
	m_headerGroup.GetWindowRect(&rcProj);
	int nWindowWidth = rcProj.Width();

    m_projCreatedByMeLabel.ShowWindow(SW_SHOW);
    m_projCreatedByMeLabel.GetWindowRect(&rc);
    ScreenToClient(&rc);

    for (int i = 0; i < MAXMIUM_PROJECT_NUM; i++)
    {
		COneProjDlg *pOneProjDlg = (COneProjDlg *)m_myProjArr[i];
		if (nMyProjCount > i) {
			CProjData pdata;
			theApp.GetProjectByMeInfo(i, pdata);
			pOneProjDlg->UpdateProjectInfo(pdata);
			int imargin = (nWindowWidth - 3 * rcProj.Width()) / 2;
			pOneProjDlg->GetWindowRect(&rcProj);
			int nLeft = rc.left + i * (rcProj.Width() + imargin);
			int nTop = rc.bottom + rc.Height() / 2;

			pOneProjDlg->SetWindowPos(this, nLeft, nTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			pOneProjDlg->ShowWindow(SW_SHOW);
			pOneProjDlg->GetWindowRect(&rcProj);
		}
		else {
			pOneProjDlg->ShowWindow(SW_HIDE);
		}
    }
	if (nMyProjCount > 0) {
		m_BtnViewAllProj.ShowWindow(SW_SHOW);
		CRect rcViewAll;
		m_BtnViewAllProj.GetWindowRect(&rcViewAll);
		ScreenToClient(&rcViewAll);
		m_BtnViewAllProj.SetWindowPos(this, rcViewAll.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
    }
    else
    {//Here we have activate part
        m_projCreatedByMeLabel.ShowWindow(SW_HIDE);
        m_BtnViewAllProj.ShowWindow(SW_HIDE);
        m_btnActivateProj.ShowWindow(SW_SHOW);
        m_strActivate.ShowWindow(SW_SHOW);
        m_grpActivate.ShowWindow(SW_SHOW);
        m_grpActivate.GetWindowRect(&rcProj);

        AddFontSize(50, m_strActivate, true);
        m_btnActivateProj.SetColorsWithBorder(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_GREEN);

    }

    if (nInvitesCount > 0)
    {
        m_projInvited.ShowWindow(SW_SHOW);
        m_projInvited.GetWindowRect(&rc);
        ScreenToClient(&rc);

        int nLeft = rc.left;
        ScreenToClient(&rcProj);
        int nTop = rcProj.bottom + rc.Height() * 1 / 2;
        rc.OffsetRect(0, nTop - rc.top);
        m_projInvited.SetWindowPos(this, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

        if (nMyProjCount == 0)
        {
            CRect rcViewAll;
            m_BtnViewAllProj.ShowWindow(SW_SHOW);
            m_BtnViewAllProj.GetWindowRect(&rcViewAll);
            ScreenToClient(&rcViewAll);
            rcViewAll.OffsetRect(0, nTop - rcViewAll.top);
            m_BtnViewAllProj.SetWindowPos(this, rcViewAll.left, rcViewAll.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }

        int i;
        for (i = 0; i < MAXMIUM_INVITATION_NUM; i++)
        {
			CProjByOthersDlg *pOtherProjDlg = (CProjByOthersDlg *)m_otherProjArr[i];
			if (nInvitesCount > i) {
				CProjData pdata;
				theApp.GetInvitationInfo(i, pdata);

				pOtherProjDlg->UpdateProjectInfo(pdata);
				pOtherProjDlg->GetWindowRect(&rcProj);

				int imargin = (nWindowWidth - 3 * rcProj.Width()) / 2;
				int nLeft = rc.left + i * (rcProj.Width() + imargin);
				int nTop = rc.bottom + rc.Height() / 2;

				pOtherProjDlg->SetWindowPos(this, nLeft, nTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
				pOtherProjDlg->GetWindowRect(&rcProj);
				pOtherProjDlg->ShowWindow(SW_SHOW);
			}
			else {
				pOtherProjDlg->ShowWindow(SW_HIDE);
			}
		}

		m_projInvited.GetWindowRect(&rc);
		ScreenToClient(&rc);
		nLeft = rc.left;
		int base = i;
		
        for (i = 0; i < MAXMIUM_PROJECT_NUM; i++)
        {
			CProjByOthersDlg *pOtherProjDlg = (CProjByOthersDlg *)m_otherProjArr[i+MAXMIUM_INVITATION_NUM];
			if (nInvitesCount + i < MAXMIUM_PROJECT_NUM && i < nOtherProjCount) {
				CProjData pdata;
				theApp.GetProjectByOthersInfo(i, pdata);
				pOtherProjDlg->UpdateProjectInfo(pdata);
				pOtherProjDlg->GetWindowRect(&rcProj);

				int imargin = (nWindowWidth - 3 * rcProj.Width()) / 2;

				int nLeft = rc.left + (i + nInvitesCount) * (rcProj.Width() + imargin);
				int nTop = rc.bottom + rc.Height() / 2;

				pOtherProjDlg->SetWindowPos(this, nLeft, nTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
				pOtherProjDlg->GetWindowRect(&rcProj);
				pOtherProjDlg->ShowWindow(SW_SHOW);
			}
			else {
				pOtherProjDlg->ShowWindow(SW_HIDE);
			}

        }

        m_BtnViewAllProj.ShowWindow(SW_SHOW);
	}
    else if (nOtherProjCount > 0)
    {
		if (nInvitesCount == 0) {
			for (int i = 0; i < MAXMIUM_INVITATION_NUM; i++)
			{
				CProjByOthersDlg *pOtherProjDlg = (CProjByOthersDlg *)m_otherProjArr[i];
				pOtherProjDlg->ShowWindow(SW_HIDE);
			}
		}

        int nLeft = rc.left;
        ScreenToClient(&rcProj);
        int nTop = rcProj.bottom + rc.Height() * 1/ 2;
        rc.OffsetRect(0, nTop - rc.top);
        m_projInvited.SetWindowPos(this, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

        if (nMyProjCount == 0)
        {
            CRect rcViewAll;
            m_BtnViewAllProj.ShowWindow(SW_SHOW);
            m_BtnViewAllProj.GetWindowRect(&rcViewAll);
            ScreenToClient(&rcViewAll);
            rcViewAll.OffsetRect(0, nTop - rcViewAll.top);
            m_BtnViewAllProj.SetWindowPos(this, rcViewAll.left, rcViewAll.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }

        for (int i = 0; i < MAXMIUM_PROJECT_NUM; i++)
        {
            CProjByOthersDlg *pOtherProjDlg = (CProjByOthersDlg *)(m_otherProjArr[i + MAXMIUM_INVITATION_NUM]);
			if (i < nOtherProjCount) {
				CProjData pdata;
				theApp.GetProjectByOthersInfo(i, pdata);

				pOtherProjDlg->UpdateProjectInfo(pdata);
				pOtherProjDlg->GetWindowRect(&rcProj);

				int imargin = (nWindowWidth - 3 * rcProj.Width()) / 2;

				int nLeft = rc.left + i * (rcProj.Width() + imargin);
				int nTop = rc.bottom + rc.Height() / 2;

				pOtherProjDlg->SetWindowPos(this, nLeft, nTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
				pOtherProjDlg->GetWindowRect(&rcProj);
				pOtherProjDlg->ShowWindow(SW_SHOW);
			}
			else {
				pOtherProjDlg->ShowWindow(SW_HIDE);
			}
        }
    }
    else
    {
        m_projInvited.ShowWindow(SW_HIDE);
    }

	if (nMyProjCount != 0)
	{
		m_BtnViewAllProj.ShowWindow(SW_SHOW);
		m_btnActivateProj.ShowWindow(SW_HIDE);
		m_strActivate.ShowWindow(SW_HIDE);
		m_grpActivate.ShowWindow(SW_HIDE);
	}

	CRect homePgRect = { 0, 0, 0, 0 };
	if (::GetWindowRect(this->m_hWnd, &homePgRect)) {
		ScreenToClient(homePgRect);
		ScreenToClient(&rcProj);
		homePgRect.bottom = rcProj.bottom + 10;//add margin
		this->SetWindowPos(NULL, 0, 0, homePgRect.Width(), homePgRect.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}
}

void CNXHomePage::SetScrollbar()
{

    CRect homePgRect;
    GetWindowRect(&homePgRect);
    ScreenToClient(homePgRect);
    InitScrollBars(theApp.m_mainhWnd, homePgRect);
}

void CNXHomePage::MoveOneWindow(CWnd *pWnd, int deltaY)
{
    ASSERT(pWnd != NULL);
    CRect rc;
    pWnd->GetWindowRect(&rc);
    ScreenToClient(&rc);
    rc.OffsetRect(0,deltaY);
    pWnd->MoveWindow(&rc);
}

void CNXHomePage::AddWindowHeight(CWnd *pWnd, int deltaY)
{
    ASSERT(pWnd != NULL);
    CRect rc;
    pWnd->GetWindowRect(&rc);
    ScreenToClient(&rc);
    //rc.OffsetRect(0, deltaY);
    rc.bottom += deltaY;
    pWnd->MoveWindow(&rc);
}

//This function move all controls in deltaY pixels vertically
CRect CNXHomePage::MoveAllControls(int deltaY)
{
    AddWindowHeight(&m_headerGroup, deltaY);
    AddWindowHeight(this, deltaY);

    MoveOneWindow(&m_btnProtect, deltaY);
    MoveOneWindow(&m_btnShare, deltaY);
    MoveOneWindow(&m_btnConnectRepo, deltaY);

    MoveOneWindow(&m_gotoMySpace, deltaY);
    MoveOneWindow(&m_gotoArrow, deltaY);

    MoveOneWindow(&m_btnActivateProj, deltaY);
    MoveOneWindow(&m_strActivate, deltaY);
    MoveOneWindow(&m_grpActivate, deltaY);

    MoveOneWindow(&m_BtnViewAllProj, deltaY);
    MoveOneWindow(&m_projCreatedByMeLabel, deltaY);

    for (auto x : m_myProjArr)
    {
        MoveOneWindow(x, deltaY);
    }

	MoveOneWindow(&m_projInvited, deltaY);
    for (auto x : m_otherProjArr)
    {
        MoveOneWindow(x, deltaY);
    }

    return CRect(0, 0, 0, 0);
}

void CNXHomePage::OnBnClickedButtonMvViewMyVaultfile()
{
    // TODO: Add your control notification handler code here
    theApp.ShowPage(MD_REPORITORYHOME);
    theApp.m_dlgRepoPage.GotoRepository(NX_BTN_MYVAULT);
}


void CNXHomePage::OnBnClickedPersonButton()
{
    CUserEdit dlg(CWnd::FromHandle(theApp.m_mainhWnd));
    dlg.DoModal();
    if (dlg.m_bLogoff)
    {
		theApp.LogoutUser();
    }
    else if (dlg.m_bNeedRefresh)
    {
        theApp.RestAPIRefreshUserInfo();
        UpdateUserInfo();
    }

}

void CNXHomePage::EditUserProfile()
{
	theApp.ShowEditUserProfile(this->m_hWnd);
}

void CNXHomePage::OnBnClickedButtonProtect()
{
	CShareFileWidget dlg(CWnd::FromHandle(theApp.m_mainhWnd));

	dlg.m_bProtectFile = TRUE;
	if (dlg.DoModal() == IDOK) {
		if (dlg.m_strFullPath.length() != 0) {
			ProtectingLocalFile(theApp.m_mainhWnd, dlg.m_strFullPath);
		}
		else {
			if (theApp.OpenProtectDlg(theApp.m_mainhWnd, dlg.m_selRepoFile.iRepoIndex, dlg.m_selRepoFile)) {
				theApp.m_dlgRepoPage.SetMyVaultRefreshFlag(TRUE);
			}
		}
	}

}


void CNXHomePage::OnBnClickedButtonShare()
{
	CShareFileWidget dlg(CWnd::FromHandle(theApp.m_mainhWnd));
	
	dlg.m_bProtectFile = FALSE;
	if (dlg.DoModal() == IDOK) {
		if (dlg.m_strFullPath.length() != 0) {
			SharingLocalFile(theApp.m_mainhWnd, dlg.m_strFullPath);
		}
		else {
			if (theApp.ShareRepositoryFile(theApp.m_mainhWnd, dlg.m_selRepoFile)) {
				theApp.m_dlgRepoPage.SetMyVaultRefreshFlag(TRUE);
			}
		}
	}

}

void CNXHomePage::OnManageLocalFile(void)
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
	CString fileToSave = fileDialog.GetFileName();
	CString fullSavePath = fileDialog.GetPathName();
	CString strtemp = L"\\" + fileToSave;
	CString folderToSave = fullSavePath;
	folderToSave.Replace(strtemp, _T(""));

	if (!PathFileExists(fullSavePath)) {
		MessageBox(theApp.LoadStringW(IDS_FILE_NOT_FOUND), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		return;
	}
	BOOL bisnxl;
	theApp.GetFileExtension(fileToSave.GetBuffer(), &bisnxl);
	if (bisnxl) {
		MessageBox(theApp.LoadStringW(IDS_FILE_ALREADY_PROTECTED), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		return;
	}

	CDlgProtectShareFile dlg(CWnd::FromHandle(theApp.m_mainhWnd));
	dlg.m_FilePath = folderToSave;
	dlg.m_strFileName = fileToSave;

	if (IDOK == dlg.DoModal()) {
		if (dlg.m_bShareFile)
			SharingLocalFile(theApp.m_mainhWnd, fullSavePath.GetBuffer());
		else if (dlg.m_bProtectFile)
			ProtectingLocalFile(theApp.m_mainhWnd, fullSavePath.GetBuffer());
		::PostMessage(theApp.m_dlgRepoPage.m_hWnd, WM_NX_REFRESH_FILE_LIST, 0, 0);
		theApp.RefreshMyVaultInfo();
	}
}
void CNXHomePage::OnBnClickedConnectRepository()
{
    // TODO: Add your control notification handler code here
    if (theApp.OpenAddRepositoryUI(NULL))
    {
		//No need refresh UI. add other code if needed.
    }
}


void CNXHomePage::OnBnClickedGotoMyspace()
{
    // TODO: Add your control notification handler code here
    theApp.ShowPage(MD_REPORITORYHOME);
    theApp.m_dlgRepoPage.GotoRepository(NX_BTN_MYVAULT);
}


void CNXHomePage::OnBnClickedButtonArrowNext()
{
    // TODO: Add your control notification handler code here
    theApp.ShowPage(MD_REPORITORYHOME);
    theApp.m_dlgRepoPage.GotoRepository(NX_BTN_MYDRIVE);
}


void CNXHomePage::OnBnClickedButtonMdViewMyDrivefile()
{
    // TODO: Add your control notification handler code here
    theApp.ShowPage(MD_REPORITORYHOME);
    theApp.m_dlgRepoPage.GotoRepository(NX_BTN_MYDRIVE);
}


void CNXHomePage::OnBnClickedButtonViewallproj()
{
    // TODO: Add your control notification handler code here
    theApp.ShowPage(MD_PROJECTSHOME);
}

LRESULT CNXHomePage::OnRefreshProjMessage(WPARAM wParam, LPARAM lParam)
{
    theApp.RefreshProjectList();
    DisplayProjectList();
    return LRESULT();
}

LRESULT CNXHomePage::OnRefreshRepoList(WPARAM wParam, LPARAM lParam)
{
	UpdateRepositoryList();
	theApp.CenterHomePage();
	SetScrollbar();
	return LRESULT();
}

void CNXHomePage::OnBnClickedButtonActivate()
{
    // TODO: Add your control notification handler code here
    theApp.ShowPage(MD_PROJECTINTRO);
}


afx_msg LRESULT CNXHomePage::OnNxRefreshHomePage(WPARAM wParam, LPARAM lParam)
{
    RefreshPage();
    return 0;
}


void CNXHomePage::OnDestroy()
{
	CFlatDlgBase::OnDestroy();

	for (auto x : m_myProjArr)
	{
		x->DestroyWindow();
		if (x->IsKindOf(RUNTIME_CLASS(COneProjDlg))) {
			delete (COneProjDlg*)x;
		}
	}

	for (auto x : m_otherProjArr)
	{
		x->DestroyWindow();
		if (x->IsKindOf(RUNTIME_CLASS(CProjByOthersDlg))) {
			delete (CProjByOthersDlg*)x;
		}
	}
	m_myProjArr.clear();
	m_otherProjArr.clear();

	for (auto x : m_repoLabels)
	{
		((CRepoLabel *)x)->DestroyWindow();
		delete (CRepoLabel *)x;
	}
	m_repoLabels.clear();
}

void CNXHomePage::UpdateProjWnd(CProjData & projData)
{
    if (projData.m_flag == NXRMC_PROJ_BY_ME)
    {
        for (auto x : m_myProjArr)
        {
            ASSERT(x != NULL);
            if (((COneProjDlg*)x)->GetProjectID() == projData.m_nID)
            {
				((COneProjDlg*)x)->UpdateProjectInfo(projData);
                ((COneProjDlg*)x)->ProjectRefresh();
            }
        }
    }
    else
    {
        for (auto x : m_otherProjArr)
        {
            ASSERT(x != NULL);
            if (((CProjByOthersDlg*)x)->GetProjectID() == projData.m_nID)
            {
				((CProjByOthersDlg*)x)->UpdateProjectInfo(projData);
            }
        }

    }
}


afx_msg LRESULT CNXHomePage::OnNxTimedRefreshProjlist(WPARAM wParam, LPARAM lParam)
{
    DisplayProjectList();
    return 0;
}
