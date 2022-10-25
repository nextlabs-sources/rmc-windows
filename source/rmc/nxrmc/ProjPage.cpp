// ProjPage.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "ProjPage.h"
#include "AddProjDlg.h"
#include "oneproject.h"
#include "ProjByOthersDlg.h"

#include "afxdialogex.h"


// CProjPage dialog
void InitScrollBars(HWND hWnd, CRect &rc);
BOOL OnVScroll(WPARAM wParam, LPARAM lParam);
BOOL OnHScroll(WPARAM wParam, LPARAM lParam);
extern int m_nScrollPos;
extern int m_nHScrollPos;


IMPLEMENT_DYNAMIC(CProjPage, CFlatDlgBase)

CProjPage::CProjPage(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_PROJECT, pParent)
{

}

CProjPage::~CProjPage()
{
}

void CProjPage::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_PAGE_TITLE, m_pageTitle);
    DDX_Control(pDX, IDC_BUTTON_CREATE, m_btnCreate);
    DDX_Control(pDX, IDC_STATIC_ME_LABEL, m_meLabel);
    DDX_Control(pDX, IDC_STATIC_OTHERS_LABEL, m_othersLabel);
}


BEGIN_MESSAGE_MAP(CProjPage, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_COMMAND(IDOK, &CProjPage::OnIdok)
    ON_WM_DESTROY()
    ON_MESSAGE(WM_NX_REFRESH_PROJ_LIST, &CProjPage::OnNxRefreshProjList)
    ON_BN_CLICKED(IDC_BUTTON_CREATE, &CProjPage::OnBnClickedButtonCreate)
    ON_MESSAGE(WM_NX_TIMED_REFRESH_PROJLIST, &CProjPage::OnNxTimedRefreshProjlist)
END_MESSAGE_MAP()


// CProjPage message handlers


HBRUSH CProjPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

	//if (nCtlColor == CTLCOLOR_LISTBOX)
	//{
	//	hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
	//}

    return hbr;
}


void CProjPage::OnPaint()
{
    CFlatDlgBase::OnPaint();
}

BOOL CProjPage::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    AddFontSize(80, m_pageTitle, false);
    AddFontSize(50, m_meLabel, false);
    AddFontSize(50, m_othersLabel, false);
    m_btnCreate.SetColors(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_BLUE);
    m_btnCreate.SetTextAlignment(BS_RIGHT);
    RefreshProjList();
    SetAllOrignRects();
    return 0;
}

//void CProjPage::RefreshProjectList()
//{
//	m_pProjList->RefreshProjList();
//}




void CProjPage::OnIdok()
{
    // TODO: Add your command handler code here
}


void CProjPage::OnDestroy()
{
    CFlatDlgBase::OnDestroy();
	for (auto x : m_projArr)
	{
		x->DestroyWindow();
		if (x->IsKindOf(RUNTIME_CLASS(COneProjDlg))) {
			delete (COneProjDlg*)x;
		}
	}
	m_projArr.clear();

	for (auto x : m_projOtherArr)
	{
		x->DestroyWindow();
		if (x->IsKindOf(RUNTIME_CLASS(CProjByOthersDlg))) {
			delete (CProjByOthersDlg*)x;
		}
	}
	m_projOtherArr.clear();
}


afx_msg LRESULT CProjPage::OnNxRefreshProjList(WPARAM wParam, LPARAM lParam)
{
    RefreshProjList(TRUE);
	return 0;
}


void CProjPage::OnBnClickedButtonCreate()
{
    theApp.AddNewProject(GetSafeHwnd());
}


void CProjPage::RefreshProjList(BOOL bUpdateData)
{
	theApp.GetProjectByMeArray(m_prjDataFromMe);
	theApp.GetProjectByOthersArray(m_prjDataFromOthers);
	theApp.GetInvitationArray(m_prjDataInvites);

	auto nMeCnt = (size_t)theApp.GetTotalProjectbyMeCount();
	if (nMeCnt > m_projArr.size())
	{
		//Create ProjejWindow here
		auto nDiff = nMeCnt - m_projArr.size();
		for (size_t i = 0; i < (size_t)nDiff; i++)
		{
			COneProjDlg *pOneProjDlg = new COneProjDlg();
			pOneProjDlg->Create(IDD_DIALOG_ONEPROJ, this);
			m_projArr.push_back(pOneProjDlg);
		}

	}


	auto nOtherCnt = (size_t)theApp.GetTotalProjectByOthersCount();
	auto nInviteCnt = m_prjDataInvites.size();
	if (nOtherCnt + nInviteCnt > m_projOtherArr.size())
	{
		//Create ProjejWindow here
		auto nDiff = nOtherCnt + nInviteCnt - m_projOtherArr.size();
		for (size_t i = 0; i < (size_t)nDiff; i++)
		{
			//        CProjByOthersDlg *pOtherProjDlg = new CProjByOthersDlg(m_prjDataInvites[i], this);
			//        pOtherProjDlg->Create(IDD_PROJ_BY_OTHERS, this);

			CProjByOthersDlg *pOtherProjDlg = new CProjByOthersDlg();
			pOtherProjDlg->Create(IDD_PROJ_BY_OTHERS, this);
			m_projOtherArr.push_back(pOtherProjDlg);
		}

	}


    DisplayProjectList();
    if (IsWindowVisible())
    {
        CRect rc;
        GetWindowRect(&rc);
        CWnd::FromHandle(theApp.m_mainhWnd)->ScreenToClient(&rc);
        InitScrollBars(theApp.m_mainhWnd, rc);
    }
}


void CProjPage::DisplayProjectList()
{
    if (m_nHScrollPos != 0 || m_nScrollPos != 0)
    {//TODO:Tempary fix for scrollbar not correct after refresh.
        ::OnVScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.
        ::OnHScroll(MAKEWORD(SB_THUMBPOSITION, 0), 0l); //Scroll to top of the page.
    }
    int nCount = (int)m_prjDataFromMe.size();

    CRect rc;
    GetClientRect(&rc);
    if (!rc.Width())
    {
        return;
    }
    CRect rcMeLabel;
    m_meLabel.GetWindowRect(&rcMeLabel);
    ScreenToClient(&rcMeLabel);


    int nLeft = 0;
    int nTop = rcMeLabel.bottom + rcMeLabel.Height();

    if (nCount > 0)
    {
        auto drawCount = nCount;
		int i;
        for (i = 0; i < drawCount; i++)
        {
			COneProjDlg *pOneProjDlg = (COneProjDlg *)m_projArr[i];
			pOneProjDlg->UpdateProjectInfo(m_prjDataFromMe[i]);
            pOneProjDlg->GetWindowRect(&m_rcProj);
            m_nMaxPrjInRow = rc.Width() / (m_rcProj.Width()* NX_ONE_PROJECT_SPACE_RATIO);
            if (m_nMaxPrjInRow == 0) return;
            nLeft = m_rcProj.Width() / NX_PROJECT_SPACE_DIVISOR;
            int nxLeft = nLeft + (i % m_nMaxPrjInRow) * m_rcProj.Width() * NX_ONE_PROJECT_SPACE_RATIO;
            int nxTop = nTop + (i / m_nMaxPrjInRow) * (m_rcProj.Height() * NX_ONE_PROJECT_SPACE_RATIO);

            pOneProjDlg->SetWindowPos(this, nxLeft, nxTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
		while (i < (int)m_projArr.size())
		{
			m_projArr[i]->ShowWindow(SW_HIDE);
			i++;
		}


        m_meLabel.ShowWindow(SW_SHOW);
        m_btnCreate.ShowWindow(SW_SHOW);
    }
    else
    {
		for (auto x : m_projArr)
		{
			x->ShowWindow(SW_HIDE);
		}
        m_meLabel.ShowWindow(SW_HIDE);
        m_btnCreate.ShowWindow(SW_HIDE);
    }

    int nInviteProjCnt = (int)m_prjDataInvites.size();
    int nOtherProjCnt = (int)m_prjDataFromOthers.size();
    if (nOtherProjCnt || nInviteProjCnt)
    {
        CRect rcLabel;
        m_othersLabel.GetWindowRect(&rcLabel);
        ScreenToClient(&rcLabel);
        CRect lastRc;
        if (nCount > 0)
        {
            m_projArr[nCount - 1]->GetWindowRect(&lastRc);
            ScreenToClient(&lastRc);
            rcLabel.OffsetRect(0, lastRc.bottom - rcLabel.bottom + 2 * rcLabel.Height());
            m_othersLabel.MoveWindow(rcLabel);
            nTop = rcLabel.bottom + rcLabel.Height();
        }
        else
        {
            CRect rc;
            m_meLabel.GetWindowRect(&rc);
            ScreenToClient(&rc);
            m_othersLabel.MoveWindow(rc);
            //m_othersLabel.ShowWindow(SW_SHOW);

        }

		int i;
        auto drawCount = nInviteProjCnt;
        for (i = 0; i < drawCount; i++)
        {
            CProjData data = {};
			CProjByOthersDlg *pOtherProjDlg = (CProjByOthersDlg *)m_projOtherArr[i];

			pOtherProjDlg->UpdateProjectInfo(m_prjDataInvites[i]);
			pOtherProjDlg->ShowWindow(SW_HIDE);
            pOtherProjDlg->GetWindowRect(&m_rcProj);
            m_nMaxPrjInRow = rc.Width() / (m_rcProj.Width()* NX_ONE_PROJECT_SPACE_RATIO);
            if (!nLeft)
            {
                nLeft = m_rcProj.Width() / NX_PROJECT_SPACE_DIVISOR;
            }
            int nxLeft = nLeft + (i % m_nMaxPrjInRow) * m_rcProj.Width() * NX_ONE_PROJECT_SPACE_RATIO;
            int nxTop = nTop + (i / m_nMaxPrjInRow) * (m_rcProj.Height() * NX_ONE_PROJECT_SPACE_RATIO);

            pOtherProjDlg->SetWindowPos(this, nxLeft, nxTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }


        for (i = nInviteProjCnt; i < nInviteProjCnt + nOtherProjCnt; i++)
        {
            CProjData data = {};
			CProjByOthersDlg *pOtherProjDlg = (CProjByOthersDlg *)m_projOtherArr[i];
			pOtherProjDlg->ShowWindow(SW_HIDE);
			pOtherProjDlg->UpdateProjectInfo(m_prjDataFromOthers[i - nInviteProjCnt]);
            pOtherProjDlg->GetWindowRect(&m_rcProj);
            m_nMaxPrjInRow = rc.Width() / (m_rcProj.Width()* NX_ONE_PROJECT_SPACE_RATIO);
            if (!nLeft)
            {
                nLeft = m_rcProj.Width() / NX_PROJECT_SPACE_DIVISOR;
            }
            int nxLeft = nLeft + (i % m_nMaxPrjInRow) * m_rcProj.Width() * NX_ONE_PROJECT_SPACE_RATIO;
            int nxTop = nTop + (i / m_nMaxPrjInRow) * (m_rcProj.Height() * NX_ONE_PROJECT_SPACE_RATIO);

            pOtherProjDlg->SetWindowPos(this, nxLeft, nxTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
		while (i < (int)m_projOtherArr.size())
		{
		        m_projOtherArr[i]->ShowWindow(SW_HIDE);
				i++;
		}

        m_othersLabel.ShowWindow(SW_SHOW);
    }
    else
    {
		for (auto x : m_projOtherArr)
		{
			x->ShowWindow(SW_HIDE);
		}

        m_othersLabel.ShowWindow(SW_HIDE);
    }

    if (nOtherProjCnt + nInviteProjCnt> 0)
    {
        CRect lastRc;
        m_projOtherArr[nOtherProjCnt + nInviteProjCnt - 1]->GetWindowRect(&lastRc);
        ScreenToClient(&lastRc);
        CRect rcDlg;
        GetWindowRect(&rcDlg);
        ScreenToClient(&rcDlg);
        rcDlg.bottom = lastRc.bottom + lastRc.Height() / NX_PROJECT_SPACE_DIVISOR;
        MoveWindow(rcDlg);

    }
    else if (nCount > 0)
    {
        CRect lastRc;
        m_projArr[m_projArr.size() - 1]->GetWindowRect(&lastRc);
        ScreenToClient(&lastRc);
        CRect rcDlg;
        GetWindowRect(&rcDlg);
        ScreenToClient(&rcDlg);
        rcDlg.bottom = lastRc.bottom + lastRc.Height() / NX_PROJECT_SPACE_DIVISOR;
        MoveWindow(rcDlg);
    }
}


void CProjPage::UpdateProjWnd(CProjData & projData)
{
    if (projData.m_flag == NXRMC_PROJ_BY_ME)
    {
        for (auto x : m_projArr)
        {
            ASSERT(x != NULL);
            if (((COneProjDlg*)x)->GetProjectID() == projData.m_nID)
            {
				((COneProjDlg*)x)->UpdateProjectInfo(projData);
            }
        }
    } 
    else
    {
        for (auto x : m_projOtherArr)
        {
            ASSERT(x != NULL);
            if (((CProjByOthersDlg*)x)->GetProjectID() == projData.m_nID)
            {
				((CProjByOthersDlg*)x)->UpdateProjectInfo(projData);
            }
        }

    }
}

int CProjPage::ResizeWindow(int cx, int cy)
{
    if (cx == 0 || cy == 0)
    {
        return cy;
    }

    CRect rc;
    GetWindowRect(&rc);
    MoveWindow(0, 0, cx, rc.Height());
    int nCount = (int)m_prjDataFromMe.size();
    int nInviteProjCnt = (int)m_prjDataInvites.size();
    int nOtherProjCnt = (int)m_prjDataFromOthers.size();
    if (nCount + nInviteProjCnt + nOtherProjCnt != 0)
    {
        m_btnCreate.SetWindowPos(this, cx - (m_ctrlData[0].rc.right - m_ctrlData[1].rc.left) , m_ctrlData[1].rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        if (cx / (m_rcProj.Width() * NX_ONE_PROJECT_SPACE_RATIO) != m_nMaxPrjInRow)
        {
			DisplayProjectList();
        }

    }
    return cy;
}



void  CProjPage::SetAllOrignRects()
{
    CtrlSizeData ctrlSize;
    CRect rc;
    GetClientRect(&rc);

    ctrlSize.rc = rc;
    ctrlSize.pCtrl = this;
    m_ctrlData.push_back(ctrlSize);
    //SetChildRect(&)
    SetChildRect(&m_btnCreate, m_ctrlData);
}


afx_msg LRESULT CProjPage::OnNxTimedRefreshProjlist(WPARAM wParam, LPARAM lParam)
{
    RefreshProjList(FALSE);
    return 0;
}
