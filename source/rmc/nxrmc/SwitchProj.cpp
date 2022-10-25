// SwitchProj.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "nxrmRMC.h"
#include "SwitchProj.h"
#include "AddProjDlg.h"
#include "oneproject.h"
#include "ProjByOthersDlg.h"
#include "afxdialogex.h"

void InitScrollBars(HWND hWnd, CRect &rc);
// CSwitchProj dialog

IMPLEMENT_DYNAMIC(CSwitchProj, CFlatDlgBase)

CSwitchProj::CSwitchProj(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_PROJECT_SWITCH, pParent)
{

}

CSwitchProj::~CSwitchProj()
{
}

void CSwitchProj::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_PAGE_TITLE, m_switchTitle);
    DDX_Control(pDX, IDC_STATIC_PROJ_OTHER_LABEL, m_otherProjLabel);
    DDX_Control(pDX, IDC_STATIC_BYME, m_projByMeLabel);
}


BEGIN_MESSAGE_MAP(CSwitchProj, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSwitchProj message handlers


BOOL CSwitchProj::OnInitDialog()
{
    CDialogEx::OnInitDialog();


    AddFontSize(80, m_switchTitle, false);
    AddFontSize(50, m_projByMeLabel, false);
    AddFontSize(50, m_otherProjLabel, false);


    RefreshProjList();
    SetAllOrignRects();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CSwitchProj::RefreshProjList(BOOL bUpdateData)
{
    if (bUpdateData)
    {
        theApp.RefreshProjectList();
    }

	theApp.GetProjectByMeArray(m_prjDataFromMe);
	theApp.GetProjectByOthersArray(m_prjDataFromOthers);
	//theApp.GetInvitationArray(m_prjDataInvites);
	if (!m_projArr.size())
	{
		CAddProjDlg *pAddProjDlg = new CAddProjDlg(NX_ADD_BUTTON_PROJPAGE, this);
		pAddProjDlg->Create(IDD_DIALOG_ADDPROJ, this);
		m_projArr.push_back(pAddProjDlg);
	}

	auto nMeCnt = (size_t)theApp.GetTotalProjectbyMeCount();
	if (nMeCnt > m_projArr.size()-1)
	{
		//Create ProjejWindow here
		auto nDiff = nMeCnt - (m_projArr.size()-1);
		for (size_t i = 0; i < (size_t)nDiff; i++)
		{
			COneProjDlg *pOneProjDlg = new COneProjDlg();
			pOneProjDlg->Create(IDD_DIALOG_ONEPROJ, this);
			m_projArr.push_back(pOneProjDlg);
		}

	}
	int n = (int)m_projArr.size();

	auto nOtherCnt = (size_t)theApp.GetTotalProjectByOthersCount();
	if (nOtherCnt > m_projOtherArr.size())
	{
		//Create ProjejWindow here
		auto nDiff = nOtherCnt - m_projOtherArr.size();
		for (size_t i = 0; i < (size_t)nDiff; i++)
		{

			CProjByOthersDlg *pOtherProjDlg = new CProjByOthersDlg();
			pOtherProjDlg->Create(IDD_PROJ_BY_OTHERS, this);
			m_projOtherArr.push_back(pOtherProjDlg);
		}

	}


    DisplayProjectList();
}




void CSwitchProj::DisplayProjectList()
{
    int nCount = (int)m_prjDataFromMe.size();
    int nLeft;
    int nTop;
    CRect rc;
    GetClientRect(&rc);
    CRect rcMeLabel;
	int lastMeIdx = -1;
	int lastOtherIdx = -1;

    CAddProjDlg *pAddProjDlg = (CAddProjDlg *)m_projArr[0];

    pAddProjDlg->GetWindowRect(&m_rcProj);

    m_projByMeLabel.GetWindowRect(&rcMeLabel);
    ScreenToClient(&rcMeLabel);


    nLeft = m_rcProj.Width() / 10;
    nTop = rcMeLabel.bottom + rcMeLabel.Height();
    if (nCount >0)
    {
        pAddProjDlg->SetWindowPos(this, nLeft, nTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

        m_projByMeLabel.ShowWindow(SW_SHOW);
    }
    else
    {
        m_projByMeLabel.ShowWindow(SW_HIDE);
		pAddProjDlg->ShowWindow(SW_HIDE);
    }

    if (nCount > 0)
    {
        auto drawCount = nCount;
		int j = 1;
		auto currProjID = theApp.m_dlgProjMain.GetCurrProjID();
		int i;
        for (i = 1; i < drawCount + 1; i++)
        {
			if (GetParent()->m_hWnd != theApp.m_dlgRepoPage.m_hWnd)
			{
				if (m_prjDataFromMe[i - 1].m_nID == currProjID)
				{
					continue;
				}
			}

            COneProjDlg *pOneProjDlg = (COneProjDlg *)m_projArr[j];
			pOneProjDlg->UpdateProjectInfo(m_prjDataFromMe[i-1]);


            pOneProjDlg->GetWindowRect(&m_rcProj);
            m_nMaxPrjInRow = rc.Width() / (m_rcProj.Width() * NX_ONE_PROJECT_SPACE_RATIO);

            int nxLeft = nLeft + (j % m_nMaxPrjInRow) * m_rcProj.Width() * NX_ONE_PROJECT_SPACE_RATIO;
            int nxTop = nTop + (j / m_nMaxPrjInRow) * (m_rcProj.Height() * NX_ONE_PROJECT_SPACE_RATIO);

            pOneProjDlg->SetWindowPos(this, nxLeft, nxTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			j++;
        }
		int n = (int)m_projArr.size();
		lastMeIdx = j-1;
		while (j < (int)m_projArr.size())
		{
			m_projArr[j]->ShowWindow(SW_HIDE);
			j++;
		}

        if (m_prjType == NX_HOME_PROJ_LIST)
        {
            //ShowScrollBar(SB_BOTH, FALSE);
        }
        else
        {
            //InitScrollBars(rcContent);
        }
    }
    else
    {
		for (auto x : m_projArr)
		{
			x->ShowWindow(SW_HIDE);
		}
		m_projByMeLabel.ShowWindow(SW_HIDE);
    }


    int nOtherProjCnt = (int)m_prjDataFromOthers.size();
    if (nOtherProjCnt)
    {
        CRect rcLabel;
        m_otherProjLabel.GetWindowRect(&rcLabel);
        ScreenToClient(&rcLabel);
        CRect lastRc;
        if (nCount > 0)
        {
            m_projArr[lastMeIdx]->GetWindowRect(&lastRc);
            ScreenToClient(&lastRc);

            rcLabel.OffsetRect(0, lastRc.bottom - rcLabel.bottom + 2 * rcLabel.Height());
            m_otherProjLabel.MoveWindow(rcLabel);
        }
        else
        {
            m_otherProjLabel.MoveWindow(rcMeLabel);
            rcLabel = rcMeLabel;
        }
        m_otherProjLabel.ShowWindow(SW_SHOW);

        auto drawCount = nOtherProjCnt;

        nTop = rcLabel.bottom + rcLabel.Height();
		auto currProjID = theApp.m_dlgProjMain.GetCurrProjID();
		int j = 0;
		int i;
        for (i = 0; i < drawCount; i++)
        {
			if (GetParent()->m_hWnd != theApp.m_dlgRepoPage.m_hWnd)
			{
				if (m_prjDataFromOthers[i].m_nID == currProjID)
				{
					continue;
				}
			}

			CProjByOthersDlg *pOtherProjDlg = (CProjByOthersDlg *)m_projOtherArr[j];
			pOtherProjDlg->UpdateProjectInfo(m_prjDataFromOthers[i]);


            pOtherProjDlg->GetWindowRect(&m_rcProj);
            m_nMaxPrjInRow = rc.Width() / (m_rcProj.Width()* NX_ONE_PROJECT_SPACE_RATIO);

            int nxLeft = nLeft + (j % m_nMaxPrjInRow) * m_rcProj.Width() * NX_ONE_PROJECT_SPACE_RATIO;
            int nxTop = nTop + (j / m_nMaxPrjInRow) * (m_rcProj.Height() * NX_ONE_PROJECT_SPACE_RATIO);

            pOtherProjDlg->SetWindowPos(this, nxLeft, nxTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			j++;
        }
		lastOtherIdx = j - 1;
		while (j < (int)m_projOtherArr.size())
		{
			m_projOtherArr[j]->ShowWindow(SW_HIDE);
			j++;
		}
    }
    else
    {
		for (auto x : m_projOtherArr)
		{
			x->ShowWindow(SW_HIDE);
		}
		m_otherProjLabel.ShowWindow(SW_HIDE);
    }

    if (!m_projOtherArr.size())
    {
        m_otherProjLabel.ShowWindow(SW_HIDE);
    }

    CRect lastRc = { 0, 0, 0, 0 };

    CRect rcDlg ;

    if (nOtherProjCnt > 0 && m_projOtherArr.size() && lastOtherIdx >= 0 )
    {
        m_projOtherArr[lastOtherIdx]->GetWindowRect(&lastRc);
    }
    else if (nCount > 0 && m_projArr.size() && lastMeIdx >= 0)
    {
        m_projArr[lastMeIdx]->GetWindowRect(&lastRc);
    }
    ScreenToClient(&lastRc);
    GetWindowRect(&rcDlg);
    ScreenToClient(&rcDlg);
    rcDlg.bottom = lastRc.bottom + lastRc.Height() / NX_PROJECT_SPACE_DIVISOR;
    SetWindowPos(GetParent(), 0, 0, rcDlg.Width(), rcDlg.Height(), SWP_NOMOVE | SWP_NOZORDER);

    GetWindowRect(&rcDlg);
    ScreenToClient(&rcDlg);
    m_rcBound = rcDlg;

}



HBRUSH CSwitchProj::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetBkColor(NX_NEXTLABS_STANDARD_WHITE);
        hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
    }
    return hbr;
}


void CSwitchProj::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CFlatDlgBase::OnPaint() for painting messages
}


BOOL CSwitchProj::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CFlatDlgBase::OnEraseBkgnd(pDC);
}


void CSwitchProj::OnDestroy()
{
	CFlatDlgBase::OnDestroy();

	for (auto x : m_projArr)
	{
		x->DestroyWindow();
		if (x->IsKindOf(RUNTIME_CLASS(COneProjDlg))) {
			delete (COneProjDlg*)x;
		}
		else if (x->IsKindOf(RUNTIME_CLASS(CAddProjDlg))) {
			delete (CAddProjDlg*)x;
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

void CSwitchProj::UpdateProjWnd(CProjData & projData)
{
    if (projData.m_flag == NXRMC_PROJ_BY_ME)
    {
        auto count = m_projArr.size();
        //Skip add button here
        for (size_t i = 1; i < count; i++)
        {
            auto x = m_projArr[i];
            ASSERT(x != NULL);
            if (((COneProjDlg*)x)->GetProjectID() == projData.m_nID)
            {
				((COneProjDlg*)x)->UpdateProjectInfo(projData);
            }
        }
    }
    else
    {
        auto count = m_projOtherArr.size();
        for (size_t i = 0; i < count; i++)
        {
            auto x = m_projOtherArr[i];
            ASSERT(x != NULL);
            if (((CProjByOthersDlg*)x)->GetProjectID() == projData.m_nID)
            {
				((CProjByOthersDlg*)x)->UpdateProjectInfo(projData);
            }
        }

    }
}


void CSwitchProj::SetChildRect(CWnd *pWnd)
{
    ASSERT(pWnd != NULL);
    CRect rc;
    CtrlSizeData ctrlSize;
    pWnd->GetWindowRect(&rc);
    ScreenToClient(&rc);
    ctrlSize.rc = rc;
    ctrlSize.pCtrl = pWnd;
    //m_originRects.push_back(rc);
    //m_ctrls.push_back(pWnd);
    m_ctrlData.push_back(ctrlSize);
}

void  CSwitchProj::SetAllOrignRects()
{
    CtrlSizeData ctrlSize;
    CRect rc;
    GetClientRect(&rc);

    ctrlSize.rc = rc;
    ctrlSize.pCtrl = this;
    m_ctrlData.push_back(ctrlSize);
    //SetChildRect(&)
    SetChildRect(&m_switchTitle);
    SetChildRect(&m_otherProjLabel);
    SetChildRect(&m_projByMeLabel);
}

int CSwitchProj::ResizeWindow(int cx, int cy)
{
    if (cx == 0 || cy == 0)
    {
        return cy;
    }
    CRect rc;
    GetWindowRect(&rc);
    MoveWindow(0, m_topPos.y, cx, rc.Height());
    int nCount = (int)m_prjDataFromMe.size();
    int nOtherProjCnt = (int)m_prjDataFromOthers.size();
    if (nCount + nOtherProjCnt != 0)
    {
        m_switchTitle.SetWindowPos(this, (cx - m_ctrlData[1].rc.Width())/2, m_ctrlData[1].rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        m_projByMeLabel.SetWindowPos(this, (cx - m_ctrlData[3].rc.Width()) / 2, m_ctrlData[3].rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        if (nOtherProjCnt > 0)
        {
            CRect rcOtherLabel;
            m_otherProjLabel.GetWindowRect(&rcOtherLabel);
            ScreenToClient(&rcOtherLabel);
            m_otherProjLabel.SetWindowPos(this, (cx - m_ctrlData[2].rc.Width()) / 2, rcOtherLabel.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }

        if (cx / (m_rcProj.Width() * NX_ONE_PROJECT_SPACE_RATIO) != m_nMaxPrjInRow)
        {
			DisplayProjectList();
        }
    }
    return m_topPos.y+ cy;
}

BOOL CSwitchProj::IsWindowVisible()
{
    return m_hWnd != 0 && CWnd::IsWindowVisible();
}

