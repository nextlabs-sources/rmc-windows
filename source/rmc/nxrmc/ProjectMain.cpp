// ProjectMain.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmc.h"
#include "nxrmRMC.h"
#include "ProjectMain.h"
#include "afxdialogex.h"
#include "resource.h"
#include "FlatDlgBase.h"

void InitScrollBars(HWND hWnd, CRect &rc);
BOOL OnVScroll(WPARAM wParam, LPARAM lParam);
BOOL OnHScroll(WPARAM wParam, LPARAM lParam);

// CProjectMain dialog

IMPLEMENT_DYNAMIC(CProjectMain, CDialogEx)


CProjectMain::CProjectMain(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_RMCMAIN_PROJECT, pParent)
{
    m_needUpdateMembers = true;
    m_needUpdateFiles = true;
}

void CProjectMain::SetProject(const CProjData & projData)
{
	m_projData = projData;
    m_needUpdateMembers = true;
    m_needUpdateFiles = true;
    SetProject();
}

void CProjectMain::SetProject(void)
{


    BOOL bSummaryExist = IsWindow(m_summaryDlg.m_hWnd);

    if (!bSummaryExist)
    {
        //CRect rc;
        //GetParent()->GetWindowRect(&rc);
        auto pHome = (CBtnClass*)GetDlgItem(IDC_RADIO_SUMMARY);
        CRect rcBtn;
        pHome->GetWindowRect(&rcBtn);
        CRect rcCaption;
        m_projCaption.GetWindowRect(rcCaption);

        //m_fileDlg->GetWindowRect(&rc);
        ScreenToClient(&rcBtn);
        rcBtn.right += rcBtn.Width() / 10;
        int left = rcBtn.left + rcBtn.Width();
        int top = rcCaption.Height();
        m_ptSubPage.x = left;
        m_ptSubPage.y = top;
        m_membersDlg.SetLeftTopPosition(CPoint(left, top));
        m_fileDlg.SetLeftTopPosition(CPoint(left, top));
        m_summaryDlg.SetLeftTopPosition(CPoint(left, top));
        m_summaryDlg.Create(IDD_PROJ_SUMMARY, this);
    }
    

    m_summaryDlg.SetProject(m_projData);

    if (!bSummaryExist)
    {

        int left = m_ptSubPage.x;
        int top = m_ptSubPage.y;

        m_summaryDlg.SetWindowPos(0, left, top, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
    }
    if (IsWindow(m_fileDlg.m_hWnd))
    {
        m_fileDlg.ResetSearch();
        m_fileDlg.SetProject(m_projData);
    }

    if (IsWindow(m_membersDlg.m_hWnd))
    {
        m_membersDlg.ResetSearch();
        m_membersDlg.SetProject(&m_projData);
    }
	m_projName.SetWindowTextW(m_projData.m_projName);
	m_projNameLeft.SetWindowTextW(m_projData.m_projName);
}

CProjectMain::~CProjectMain()
{
}

void CProjectMain::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ALLPROJ_TEXT, m_allProjText);
    DDX_Control(pDX, IDC_STATIC_CAPTION_NAME, m_projName);
    DDX_Control(pDX, IDC_BUTTON_UPDOWN, m_btnShowAllProj);
    DDX_Control(pDX, IDC_RADIO_SUMMARY, m_btnSummary);
    DDX_Control(pDX, IDC_RADIO_FILES, m_btnFiles);
    DDX_Control(pDX, IDC_RADIO_PEOPLE, m_btnPeople);
    DDX_Control(pDX, IDC_STATIC_PROJNAME, m_projNameLeft);
    DDX_Control(pDX, IDC_STATIC_HIGHTLITE, m_projCaption);
}


BEGIN_MESSAGE_MAP(CProjectMain, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_STN_CLICKED(IDC_STATIC_HIGHTLITE, &CProjectMain::OnStnClickedStaticHightlite)
    ON_BN_CLICKED(IDC_BUTTON_UPDOWN, &CProjectMain::OnBnClickedButtonAllproject)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_RADIO_PEOPLE, &CProjectMain::OnClickedRadioPeople)
    ON_BN_CLICKED(IDC_RADIO_FILES, &CProjectMain::OnClickedRadioFiles)
    ON_BN_CLICKED(IDC_RADIO_SUMMARY, &CProjectMain::OnClickedRadioSummary)
    ON_MESSAGE(WM_NX_TIMED_REFRESH_ONE_PROJ, &CProjectMain::OnNxTimedRefreshOneProj)
    ON_WM_ERASEBKGND()
    ON_STN_CLICKED(IDC_ALLPROJ_TEXT, &CProjectMain::OnStnClickedAllprojText)
    ON_MESSAGE(WM_NX_UPDATE_WINDOW_SIZE, &CProjectMain::OnNxUpdateWindowSize)
END_MESSAGE_MAP()


// CProjectMain message handlers

void CProjectMain::ShowProjPage(ProjPage pageIdx)
{
    ::OnVScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.
    ::OnHScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.
    m_summaryDlg.ShowWindow(pageIdx == NX_PROJ_SUMMARY_PAGE ? SW_SHOW : SW_HIDE);
    if ( pageIdx == NX_PROJ_FILES_PAGE)
    {
        if (!m_fileDlg.GetSafeHwnd())
        {
            m_fileDlg.Create(IDD_PROJ_FILE_LIST, this);
            m_fileDlg.SetWindowPos(0, m_ptSubPage.x, m_ptSubPage.y, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
        }
        if (m_needUpdateFiles)
        {
            auto Cur = SetCursor(LoadCursor(NULL, IDC_WAIT));
            ShowCursor(true);
            m_fileDlg.ResetSearch();
            m_fileDlg.SetProject(m_projData);
            m_needUpdateFiles = false;
            SetCursor(Cur);
            ShowCursor(true);
        }

    }

    if (m_fileDlg.GetSafeHwnd())
    {
        m_fileDlg.ShowWindow(pageIdx == NX_PROJ_FILES_PAGE ? SW_SHOW : SW_HIDE);
    }

    if ( pageIdx == NX_PROJ_PEOPLE_PAGE)
    {
        
        if (!m_membersDlg.GetSafeHwnd())
        {
            m_membersDlg.Create(IDD_PROJ_MEMBERS, this);
            m_membersDlg.SetWindowPos(0, m_ptSubPage.x, m_ptSubPage.y, 0, 0, SWP_NOSIZE | SWP_NOOWNERZORDER);
        }
        if (m_needUpdateMembers)
        {
            auto Cur = SetCursor(LoadCursor(NULL, IDC_WAIT));
            ShowCursor(true);
            m_membersDlg.ResetSearch();
            m_membersDlg.SetProject(&m_projData);
            m_needUpdateMembers = false;
            SetCursor(Cur);
            ShowCursor(true);
        }

    }

    if (m_membersDlg.GetSafeHwnd())
    {
        m_membersDlg.ShowWindow(pageIdx == NX_PROJ_PEOPLE_PAGE ? SW_SHOW : SW_HIDE);
    }
    if (!m_switchDlg.GetSafeHwnd() && pageIdx == NX_PROJ_SWITCH_PAGE)
    {
        m_switchDlg.Create(IDD_PROJECT_SWITCH, this);
        m_switchDlg.SetWindowPos(&CWnd::wndTop, 0, m_ptSubPage.y, 0, 0, SWP_NOSIZE);
        m_switchDlg.SetLeftTopPosition(CPoint(0, m_ptSubPage.y));
    }
    if(m_switchDlg.GetSafeHwnd())
    {
        m_switchDlg.ShowWindow(pageIdx == NX_PROJ_SWITCH_PAGE ? SW_SHOW : SW_HIDE);
    }

    if(pageIdx != NX_PROJ_SWITCH_PAGE)
    {
        for(int i=0; i <(int)m_btnArr.size(); i++)
        {
            if (i == pageIdx)
            {
                m_btnArr[i]->SetBtnCheck(BST_CHECKED);
                m_btnArr[i]->InvalidateRect(NULL);
            }
            else
            {
                m_btnArr[i]->SetBtnCheck(BST_UNCHECKED);
                m_btnArr[i]->InvalidateRect(NULL);
            }
        }
        m_pageIdx = pageIdx;
        m_projNameLeft.ShowWindow(SW_SHOW);
        m_btnSummary.ShowWindow(SW_SHOW);
        m_btnFiles.ShowWindow(SW_SHOW);
        m_btnPeople.ShowWindow(SW_SHOW);
        m_btnShowAllProj.SetImages(IDB_PNG_WHITE_GODOWN, 0, 0, NX_NEXTLABS_STANDARD_GREEN);
        m_btnShowAllProj.InvalidateRect(NULL);
        m_bSwitchpage = false;

        CRect rectBanner;
        m_projCaption.GetWindowRect(&rectBanner);
        ScreenToClient(&rectBanner);
        

        CRect rcPage; 
        switch (pageIdx)
        {
        case NX_PROJ_SUMMARY_PAGE:
            //m_summaryDlg.GetWindowRect(&rcScroll);
            rcPage = m_summaryDlg.m_rcBound;
            break;
        case NX_PROJ_FILES_PAGE:
            rcPage = m_fileDlg.m_rcBound;
            break;
        case NX_PROJ_PEOPLE_PAGE:
            rcPage = m_membersDlg.m_rcBound;
            break;
        }

        CRect rcScroll(0, 0, m_ptSubPage.x + rcPage.Width(), m_ptSubPage.y + rcPage.Height());
        SetWindowPos(0, 0, 0, rcScroll.Width(), rcScroll.Height(), SWP_NOMOVE | SWP_NOZORDER);
        InitScrollBars(theApp.m_mainhWnd, rcScroll);
        CRect rc;
        GetParent()->GetClientRect(&rc);
        ResizeWindow(rc.Width(), rc.Height());


    }
    else
    {
        m_projNameLeft.ShowWindow(SW_HIDE);
        m_btnSummary.ShowWindow(SW_HIDE);
        m_btnFiles.ShowWindow(SW_HIDE);
        m_btnPeople.ShowWindow(SW_HIDE);
        m_btnShowAllProj.SetImages(IDB_PNG_WHITE_GOUP, 0, 0, NX_NEXTLABS_STANDARD_GREEN);
        m_btnShowAllProj.InvalidateRect(NULL);
        m_bSwitchpage = true;
        CRect rc;
        GetParent()->GetClientRect(&rc);
        ResizeWindow(rc.Width(), rc.Height());

        CRect rcPage;
        m_switchDlg.GetWindowRect(&rcPage);
        CRect rcScroll(0, 0, rcPage.Width(), m_ptSubPage.y + rcPage.Height());
        SetWindowPos(0, 0, 0, rcScroll.Width(), rcScroll.Height(), SWP_NOMOVE | SWP_NOZORDER);
        InitScrollBars(theApp.m_mainhWnd, rcScroll);
    }
}

BOOL CProjectMain::OnInitDialog()
{

    CFlatDlgBase::OnInitDialog();

    CRect rcBanner;
    CRect rcUpDown;

    m_projCaption.GetWindowRect(&rcBanner);
    ScreenToClient(&rcBanner);
    m_btnShowAllProj.GetWindowRect(&rcUpDown);
    ScreenToClient(&rcUpDown);
    m_initRight = rcBanner.right;
    m_initBtnRight = rcUpDown.right;

    ScreenToClient(&rcBanner);



    m_page = NXRMC_FROM_HOME_PAGE;
    AddFontSize(80, m_projName, false);
    AddFontSize(80, m_projNameLeft, false);
    AddFontSize(80, m_allProjText, false);
    m_br.CreateSolidBrush(NX_NEXTLABS_STANDARD_GREEN);

    m_btnShowAllProj.SetImages(IDB_PNG_WHITE_GODOWN, 0, 0, NX_NEXTLABS_STANDARD_GREEN);
    m_bSwitchpage = false;
    //m_btnShowAllProj.ShowWindow(SW_HIDE);


    m_btnSummary.SetImagetText(IDB_PNG_ALLFILESBLACK, IDB_PNG_ALLFILESGRAY, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_GREEN);
    m_btnFiles.SetImagetText(IDB_PNG_ALLFILESBLACK, IDB_PNG_ALLFILESGRAY, NX_USE_BMP_TEXT_HORIZONTAL, NX_NEXTLABS_STANDARD_GREEN);
    m_btnPeople.SetImagetText(IDB_PNG_PEOPLE_BLACK, IDB_PNG_PEOPLE_GRAY, NX_USE_BMP_TEXT_HORIZONTAL_CIRCLE_TEXT, NX_NEXTLABS_STANDARD_GREEN);

    m_btnArr.clear();
    m_dlgArray.clear();
    m_btnArr.push_back(&m_btnSummary);
    m_btnArr.push_back(&m_btnFiles);
    m_btnArr.push_back(&m_btnPeople);
    //m_pageIdx = NX_PROJ_SUMMARY_PAGE;
    //m_btnSummary.SetCheck(BST_CHECKED);
    //ShowProjPage(NX_PROJ_SUMMARY_PAGE);
    SetBackColor(NX_NEXTLABS_STANDARD_WHITE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CProjectMain::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);
    switch (nCtlColor)
    {
    case CTLCOLOR_STATIC:
        if (pWnd->GetDlgCtrlID() != IDC_STATIC_PROJNAME)
        {
            pDC->SetTextColor(NX_NEXTLABS_STANDARD_WHITE);
            pDC->SetBkColor(NX_NEXTLABS_STANDARD_GREEN);
            hbr = (HBRUSH)m_br.GetSafeHandle();
        } 
        else
        {
            pDC->SetTextColor(NX_NEXTLABS_STANDARD_BLACK);
            pDC->SetBkColor(NX_NEXTLABS_STANDARD_WHITE);

        }
        break;
    }

    // TODO:  Change any attributes of the DC here

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


void CProjectMain::OnStnClickedStaticHightlite()
{
    // TODO: Add your control notification handler code here
}


void CProjectMain::OnBnClickedButtonAllproject()
{
    //::ShowWindow(m_hWnd, SW_HIDE);
    //theApp.m_dlgProjPage.DestroyWindow();
    //theApp.ShowPage(MD_PROJECTSHOME);  
    if (m_bSwitchpage)
    {
        //m_btnShowAllProj.SetImages(IDB_PNG_WHITE_GODOWN, 0, 0, NX_NEXTLABS_STANDARD_GREEN);
        //m_bSwitchpage = false;
        ShowProjPage(m_pageIdx);
    }
    else
    {
        //m_btnShowAllProj.SetImages(IDB_PNG_WHITE_GOUP, 0, 0, NX_NEXTLABS_STANDARD_GREEN);
        //m_bSwitchpage = true;
        ShowProjPage(NX_PROJ_SWITCH_PAGE);
    }

}


void CProjectMain::OnBnClickedButtonAdd()
{
    // TODO: Add your control notification handler code here
}


void CProjectMain::OnDestroy()
{
    CFlatDlgBase::OnDestroy();
    m_br.DeleteObject();
    //delete this;
    // TODO: Add your message handler code here
}


void CProjectMain::OnClickedRadioPeople()
{
    // TODO: Add your control notification handler code here
    ShowProjPage(NX_PROJ_PEOPLE_PAGE);
}


void CProjectMain::OnClickedRadioFiles()
{
    // TODO: Add your control notification handler code here
    ShowProjPage(NX_PROJ_FILES_PAGE);
}


void CProjectMain::OnClickedRadioSummary()
{
    // TODO: Add your control notification handler code here
    ShowProjPage(NX_PROJ_SUMMARY_PAGE);
}


void CProjectMain::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class

    //CFlatDlgBase::OnOK();
}


void CProjectMain::OnCancel()
{
    // TODO: Add your specialized code here and/or call the base class

    //CFlatDlgBase::OnCancel();
}


void CProjectMain::OnClickedButtonClose()
{
    // TODO: Add your control notification handler code here
    ShowProjPage(m_pageIdx);
}

void CProjectMain::SetReturnPage(FromPage page)
{
    m_page = page;
    m_allProjText.SetWindowTextW(theApp.LoadString(IDS_HOME)/*L"Home"*/);
    //if (page == NXRMC_FROM_HOME_PAGE)
    //{
    //    m_allProjText.SetWindowTextW(L"Home");
    //}
    //else if (page == NXRMC_FROM_PROJECT_PAGE)
    //{
    //    m_allProjText.SetWindowTextW(L"Projects");
    //}


}



//Return the new height of the resized window
int CProjectMain::ResizeWindow(int cx, int cy)
{
    if (cx == 0 || cy == 0)
    {
        return cy;
    }

    //rc = m_summaryDlg.m_rcBound;
    //GetWindowRect(&rc);
    if (m_summaryDlg.IsWindowVisible())
    {
        //CRect rcClient = m_summaryDlg.m_rcBound;
        //cx = max(cx, m_ptSubPage.x + rcClient.Width());
        //cy = max(cy, m_ptSubPage.y + rcClient.Height());
        //m_summaryDlg.ResizeWindow(cx, cy);
        m_summaryDlg.ResizeWindow(cx, cy);
        CRect rcWnd;
        m_summaryDlg.GetWindowRect(&rcWnd);
        ScreenToClient(&rcWnd);
        cy = rcWnd.bottom;

    }
    else if (m_fileDlg.IsWindowVisible())
    {
        m_fileDlg.ResizeWindow(cx, cy);
        CRect rcWnd;
        m_fileDlg.GetWindowRect(&rcWnd);
        ScreenToClient(&rcWnd);
        cy = rcWnd.bottom;
    }
    else  if (m_membersDlg.IsWindowVisible())
    {

        m_membersDlg.ResizeWindow(cx, cy);
        CRect rcWnd;
        m_membersDlg.GetWindowRect(&rcWnd);
        ScreenToClient(&rcWnd);

        cy = rcWnd.bottom;

    } 
    else if (m_switchDlg.IsWindowVisible())
    {
        m_switchDlg.ResizeWindow(cx, cy);
        CRect rcWnd;
        m_switchDlg.GetWindowRect(&rcWnd);
        ScreenToClient(&rcWnd);
        cy = rcWnd.bottom;
        
    }

    MoveWindow(0, 0, cx, cy);
	MoveWindowPos(&m_projCaption, cx, 0, TRUE);

	RepositionCaption();

    return cy;
}

void CProjectMain::RepositionCaption()
{

	CRect rc;
	CRect rcTopRect;
	CRect rcHome;

	m_allProjText.GetWindowRect(&rcHome);
	ScreenToClient(&rcHome);

	GetParent()->GetWindowRect(&rcTopRect);
	int captionWidth = rcTopRect.Width() - rcHome.right; //Caption area start from rcHome.right
	m_btnShowAllProj.ShowWindow(SW_HIDE);
	m_projName.ShowWindow(SW_HIDE);
	int textWidth = GetTextWidth(&m_projName, 80); 
	m_projName.GetWindowRect(&rc);
	ScreenToClient(&rc);
	int textTop = rc.top;
	int textHeight = rc.Height();

	m_btnShowAllProj.GetWindowRect(&rc);
	ScreenToClient(&rc);

	int left = rcHome.right + (captionWidth - (textWidth + rc.Width() * 4 / 3)) / 2;

	m_projName.SetWindowPos(0, left, textTop, textWidth, textHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

	m_btnShowAllProj.SetWindowPos(0, left + textWidth + rc.Width() / 3, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

}



afx_msg LRESULT CProjectMain::OnNxTimedRefreshOneProj(WPARAM wParam, LPARAM lParam)
{
        m_fileDlg.OnBnClickedButtonRefresh();
        m_membersDlg.OnBnClickedButtonRefresh();
        m_summaryDlg.SetProject(m_projData);
    return 0;
}


void  CProjectMain::SetAllOrignRects()
{
    CtrlSizeData ctrlSize;
    CRect rc;
    GetClientRect(&rc);

    ctrlSize.rc = rc;
    ctrlSize.pCtrl = this;
    m_ctrlData.push_back(ctrlSize);

    
    SetChildRect(&m_projName, m_ctrlData);
    SetChildRect(&m_btnShowAllProj, m_ctrlData);
    
}


BOOL CProjectMain::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}


void CProjectMain::OnStnClickedAllprojText()
{
    theApp.ShowPage(MD_HOMEPAGE);
}


afx_msg LRESULT CProjectMain::OnNxUpdateWindowSize(WPARAM wParam, LPARAM lParam)
{
    CWnd *pWnd = (CWnd *)lParam;
    ASSERT(pWnd != 0);
    CRect rcChild;
    pWnd->GetWindowRect(&rcChild);
    CRect rcProjMain;
    GetWindowRect(&rcProjMain);
    CRect rcScroll(0, 0, rcProjMain.Width() - 1, rcChild.Height() - 1 + m_ptSubPage.y);
    SetWindowPos(GetParent(), 0, 0, rcScroll.Width(), rcScroll.Height(), SWP_NOMOVE | SWP_NOZORDER);
    ::OnVScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l);
    InitScrollBars(theApp.m_mainhWnd, rcScroll);
    return 0;
}
