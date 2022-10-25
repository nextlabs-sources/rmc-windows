// ProjMembers.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmc.h"
#include "nxrmRMC.h"
#include "ProjMembers.h"
#include "ProjMember.h"
#include "afxdialogex.h"
#include "resource.h"

void InitScrollBars(HWND hWnd, CRect &rc);
BOOL OnVScroll(WPARAM wParam, LPARAM lParam);
BOOL OnHScroll(WPARAM wParam, LPARAM lParam);
// CProjMembers dialog

IMPLEMENT_DYNAMIC(CProjMembers, CFlatDlgBase)

CProjMembers::CProjMembers(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_PROJ_MEMBERS, pParent), m_pProjData(NULL)
{
    m_nMaxPrjInRow = 0;
}

CProjMembers::~CProjMembers()
{
    for (auto &m : m_memberArr)
    {
		m->DestroyWindow();
		if (m->IsKindOf(RUNTIME_CLASS(CProjMember))) {
			delete (CProjMember*)m;
		}
	}
    m_memberArr.clear();

    for (auto &m : m_pendingMemberArr)
    {
		m->DestroyWindow();
		if (m->IsKindOf(RUNTIME_CLASS(CProjMember))) {
			delete (CProjMember*)m;
		}
	}
	m_pendingMemberArr.clear();
}

void CProjMembers::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_MEMBER, m_titleName);
    DDX_Control(pDX, IDC_BUTTON_INVITE, m_inviteBtn);
    DDX_Control(pDX, IDC_BUTTON_SEARCH2, m_searchBtn);
    DDX_Control(pDX, IDC_STATIC_PROJ_NAME, m_projName);
    DDX_Control(pDX, IDC_BUTTON_REFRESH, m_btnRefresh);
    DDX_Control(pDX, IDC_EDIT_SEARCH, m_searchEditBox);
    DDX_Control(pDX, IDC_BUTTON_RESET_SEARCH, m_resetSearchBtn);
    DDX_Control(pDX, IDC_STATIC_NAVIGATION_BAND, m_navBand);
    DDX_Control(pDX, IDC_PENDING_LABEL, m_pendingLabel);
    DDX_Control(pDX, IDC_VERT_SEPARATOR, m_vertSep);
    DDX_Control(pDX, IDC_STATIC_HORIZ, m_stcHorizLine);
    DDX_Control(pDX, IDC_STATIC_VERT_SEP_SEARCH, m_searchSep);
}


BEGIN_MESSAGE_MAP(CProjMembers, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BUTTON_GOBACK2, &CProjMembers::OnBnClickedButtonGoback2)
    ON_BN_CLICKED(IDC_BUTTON_FORWARD, &CProjMembers::OnBnClickedButtonForward)
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CProjMembers::OnBnClickedButtonRefresh)
    ON_BN_CLICKED(IDC_BUTTON_SEARCH2, &CProjMembers::OnBnClickedButtonSearch2)
    ON_BN_CLICKED(IDC_BUTTON_INVITE, &CProjMembers::OnBnClickedButtonInvite)
    ON_BN_CLICKED(IDC_BUTTON_RESET_SEARCH, &CProjMembers::OnClickedButtonResetSearch)
END_MESSAGE_MAP()



BOOL CProjMembers::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    GetWindowRect(&m_rcBound);
    ScreenToClient(&m_rcBound);
    m_gray.CreateSolidBrush(NX_NEXTLABS_STANDARD_GRAY);
    m_searchBtn.SetImages(IDB_PNG_SEARCH, 0, 0);
    m_resetSearchBtn.SetImages(IDB_PNG_CLOSE, 0, 0);

    m_btnRefresh.SetImages(IDB_PNG_REFRESH, 0, 0, NX_NEXTLABS_STANDARD_GRAY);


    AddFontSize(60, m_titleName, true);
    AddFontSize(30, m_pendingLabel, true);
    m_inviteBtn.SetImages(IDB_PNG_INVITE, 0, 0, NX_NEXTLABS_STANDARD_WHITE);
    m_searchEditBox.SendMessage(EM_SETCUEBANNER, 0, (LPARAM)(LPCWSTR)theApp.LoadStringW(IDS_SEARCH_MEMBERS).GetBuffer()/*L"Search for members"*/);

    GetParent()->GetWindowRect(&m_originRect);  //ProjMain window size
    GetParent()->ScreenToClient(&m_originRect);
   
    SetAllOrignRects();
    SetBackgroundColor(NX_NEXTLABS_STANDARD_WHITE);
   

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CProjMembers::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        auto ctlID = pWnd->GetDlgCtrlID();
        if (ctlID == IDC_STATIC_MEMBER || ctlID == IDC_PENDING_LABEL)
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


void CProjMembers::OnDestroy()
{
    CDialogEx::OnDestroy();
    m_gray.DeleteObject();
    // TODO: Add your message handler code here
}

void CProjMembers::SearchMembers(const wstring &keyWord)
{
	m_members.clear();
	m_pendingMembers.clear();
	theApp.FindProjectMembers(*m_pProjData, keyWord, m_ownerId, m_members);
    theApp.ProjectFindPendingMembers(*m_pProjData, keyWord, m_pendingMembers);

	CreateMemberWindows();
	ListMembers();
	if (IsWindowVisible())
	{
		GetParent()->PostMessage(WM_NX_UPDATE_WINDOW_SIZE, 0, (LPARAM)this);
	}

}


void CProjMembers::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    auto pWnd = GetFocus();

    if (pWnd->GetSafeHwnd() == m_searchEditBox.GetSafeHwnd())
    {
        CString wndText;
        m_searchEditBox.GetWindowText(wndText);
        {
            m_searchMode = true;
            m_searchString = wndText;

            SearchMembers(wndText.GetBuffer());
            m_resetSearchBtn.ShowWindow(SW_SHOW);
        }

    }
}


void CProjMembers::OnCancel()
{
    // TODO: Add your specialized code here and/or call the base class

    //CFlatDlgBase::OnCancel();
}


BOOL CProjMembers::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CDialogEx::OnEraseBkgnd(pDC);
}


void CProjMembers::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CFlatDlgBase::OnPaint() for painting messages
}


void CProjMembers::OnBnClickedButtonGoback2()
{
    // TODO: Add your control notification handler code here
    GetParent()->PostMessage(WM_NX_PREV_PROJECT, 0, 0);
}


void CProjMembers::OnBnClickedButtonForward()
{
    // TODO: Add your control notification handler code here
    GetParent()->PostMessage(WM_NX_NEXT_PROJECT, 0, 0);
}


void CProjMembers::OnBnClickedButtonRefresh()
{
    // TODO: Add your control notification handler code here

	if (!m_pProjData)
	{
		return;
	}

    if (m_searchMode)
    {
        SearchMembers(m_searchString.GetBuffer());
    }
    else
    {
        SetProject(m_pProjData);
    }
}


void CProjMembers::OnBnClickedButtonSearch2()
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


void CProjMembers::OnBnClickedButtonInvite()
{
    if (theApp.InvitePeopleToProject(*m_pProjData))
    {
        OnBnClickedButtonRefresh();
    }
}

void  CProjMembers::CreateMemberWindows()
{
	ProjMember memberData;
	memberData.memberName = L"Reserved";	//Set default for diagnostics
	memberData.projID = L"";				//Set ID is equal to empty

	int diff = (int)(m_members.size() - m_memberArr.size());
	if (diff > 0)
	{
		for (int i = 0; i < diff; i++)
		{
			CProjMember *pProjMember = new CProjMember(memberData);

			if (pProjMember->Create(IDD_DIALOG_MEMBER, this))
			{
				m_memberArr.push_back(pProjMember);
			}
		}
	}
	diff = (int)(m_pendingMembers.size() - m_pendingMemberArr.size());
	if (diff > 0)
	{
		for (int i = 0; i < diff; i++)
		{
			CProjMember *pProjMember = new CProjMember(memberData, true);

			if (pProjMember->Create(IDD_DIALOG_MEMBER, this))
			{
				m_pendingMemberArr.push_back(pProjMember);
			}
		}
	}
}


void CProjMembers::SetProject(const CProjData* projData)
{
    m_pProjData = projData;
    auto desc = m_pProjData->m_projName;
    m_projName.SetWindowTextW(desc);
    m_ownerId = m_pProjData->m_ownerID;

    SearchMembers(L"");

}

void CProjMembers::ListMembers()
{
    int nCount = (int)m_members.size();
    auto pendingCount = (int)m_pendingMembers.size();
    

    CRect rc;
    GetWindowRect(&rc);
    ScreenToClient(&rc);
    //GetClientRect(&rc);
    if (!rc.Width() || !rc.Height())
    {
        return;
    }
    CRect rcTitleLabel;
    m_titleName.GetWindowRect(&rcTitleLabel);
    ScreenToClient(&rcTitleLabel);


    int nLeft = 0;
    int nTop = rcTitleLabel.bottom + rcTitleLabel.Height() /2 ;

    if (nCount > 0)
    {
        auto drawCount = nCount;
		int i;
        for (i = 0; i < drawCount; i++)
        {
            CProjMember *pProjMember = m_memberArr[i];

			pProjMember->UpdateMember(m_members[i]);
            pProjMember->GetWindowRect(&m_rcMember);
            m_nMaxPrjInRow = rc.Width() / (m_rcMember.Width() * NX_MEMBER_WINDOW_SPACE_RATIO);
            nLeft = m_rcMember.Width() * NX_MEMBER_WINDOW_SPACE_RATIO - m_rcMember.Width();
            int nxLeft = nLeft + (i % m_nMaxPrjInRow) * m_rcMember.Width() * NX_MEMBER_WINDOW_SPACE_RATIO;
            int nxTop = nTop + (i / m_nMaxPrjInRow) * (m_rcMember.Height() *NX_MEMBER_WINDOW_SPACE_RATIO);

            pProjMember->SetWindowPos(this, nxLeft, nxTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
		while (i < (int)m_memberArr.size())
		{
			m_memberArr[i]->ShowWindow(SW_HIDE);
			i++;
		}
	}
	else
	{
		for(auto x : m_memberArr)
		{
			x->ShowWindow(SW_HIDE);
		}

	}

    if (pendingCount)
    {
        m_pendingLabel.ShowWindow(SW_SHOW);

        CRect rcLabel;
        m_pendingLabel.GetWindowRect(&rcLabel);
        ScreenToClient(&rcLabel);
        CRect lastRc;
        if (nCount)
        {
            m_memberArr[nCount - 1]->GetWindowRect(&lastRc);
            ScreenToClient(&lastRc);
        }
        else
        {
            m_titleName.GetWindowRect(&lastRc);
            ScreenToClient(&lastRc);
        }
        rcLabel.OffsetRect(0, lastRc.bottom - rcLabel.bottom + 2 * rcLabel.Height());
        m_pendingLabel.MoveWindow(rcLabel);

        nTop = rcLabel.bottom + rcLabel.Height();
		int i;
        for (i = 0; i < pendingCount; i++)
        {
            CProjMember *pProjMember = m_pendingMemberArr[i];
			pProjMember->ShowWindow(SW_HIDE);
			pProjMember->UpdateMember(m_pendingMembers[i]);
			pProjMember->GetWindowRect(&m_rcMember);
            m_nMaxPrjInRow = rc.Width() / (m_rcMember.Width() * NX_MEMBER_WINDOW_SPACE_RATIO);
            nLeft = m_rcMember.Width() * NX_MEMBER_WINDOW_SPACE_RATIO - m_rcMember.Width();
            int nxLeft = nLeft + (i % m_nMaxPrjInRow) * m_rcMember.Width() * NX_MEMBER_WINDOW_SPACE_RATIO;
            int nxTop = nTop + (i / m_nMaxPrjInRow) * (m_rcMember.Height() *NX_MEMBER_WINDOW_SPACE_RATIO);

            pProjMember->SetWindowPos(this, nxLeft, nxTop, rc.Width(), rc.Height(), SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
		while (i < (int)m_pendingMemberArr.size())
		{
			m_pendingMemberArr[i]->ShowWindow(SW_HIDE);
			i++;
		}
    }
    else
    {
        m_pendingLabel.ShowWindow(SW_HIDE);
		for (auto x : m_pendingMemberArr)
		{
			x->ShowWindow(SW_HIDE);
		}
    }

    CRect lastRc;
    CRect rcDlg;
    if (pendingCount > 0)
    {
        m_pendingMemberArr[pendingCount - 1]->GetWindowRect(&lastRc);
    }
    else
    {//At lest we have one member in one project.
        m_memberArr[nCount - 1]->GetWindowRect(&lastRc);
    }

    ScreenToClient(&lastRc);
    ::GetClientRect(theApp.m_mainhWnd, &rcDlg);
    auto bottom = max(lastRc.bottom + lastRc.Height() / 10, rcDlg.Height() - m_topPos.y);
    SetWindowPos(GetParent(), 0, 0, rc.Width(), bottom, SWP_NOMOVE | SWP_NOZORDER);
    
    m_rcBound = CRect(0, 0, rc.Width(), lastRc.bottom + lastRc.Height() / 10);
    m_vertSep.SetWindowPos(0, 0, 0, 1, max(rcDlg.Height(), m_rcBound.Height()), SWP_NOMOVE | SWP_NOZORDER);
}

void CProjMembers::OnClickedButtonResetSearch()
{
    // TODO: Add your control notification handler code here
    SetProject(m_pProjData);
    ResetSearch();
}

void CProjMembers::ResetSearch()
{

    m_searchEditBox.SetWindowTextW(L"");
    m_resetSearchBtn.ShowWindow(SW_HIDE);
    m_searchMode = false;
    m_searchString = L"";
}

int CProjMembers::ResizeWindow(int cx, int cy)
{
    if (cx == 0 || cy == 0)
    {
        return cy;
    }

    cx -= m_topPos.x;
    cy -= m_topPos.y;
    CRect rc;
    GetWindowRect(&rc);


    MoveWindowPos(&m_stcHorizLine, cx - 1, 0, TRUE);
    MoveWindowPos(&m_vertSep, 0, cy - 1, TRUE);
    SetWindowPos(this, 0, 0, cx, rc.Height(), SWP_NOMOVE | SWP_NOZORDER); //We need width to be set here for the call ListMembers()
    BOOL bResetVisible = m_resetSearchBtn.IsWindowVisible();

    for (int i = 2; i <= 6; i++)
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

    for (int i = 2; i <= 6; i++)
    {
        if (i == 4 && !bResetVisible)
        {   //We do not show reset button when it was hidden originally
            continue;
        }

        m_ctrlData[i].pCtrl->ShowWindow(SW_SHOW);
    }


    if ((m_memberArr.size() + m_pendingMemberArr.size()) && m_rcMember.Width())
    {

        if (cx / (m_rcMember.Width() * NX_MEMBER_WINDOW_SPACE_RATIO) != m_nMaxPrjInRow)
        {
            CreateMemberWindows();
            ListMembers();
        }

        CRect currRC;
        GetWindowRect(&currRC);
        int height = max(m_rcBound.Height(), cy);
        SetWindowPos(GetParent(), 0, 0, cx, height, SWP_NOMOVE | SWP_NOZORDER);
        m_vertSep.SetWindowPos(0, 0, 0, 1, height, SWP_NOMOVE | SWP_NOZORDER);

    }

    return cy;
}


void  CProjMembers::SetAllOrignRects()
{
    CtrlSizeData ctrlSize;
    CRect rc;
    GetClientRect(&rc);

    ctrlSize.rc = rc;
    ctrlSize.pCtrl = this;
    m_ctrlData.push_back(ctrlSize);
    SetChildRect(&m_inviteBtn, m_ctrlData);
    SetChildRect(&m_searchBtn, m_ctrlData);
    SetChildRect(&m_searchEditBox, m_ctrlData);
    SetChildRect(&m_resetSearchBtn, m_ctrlData);
    SetChildRect(&m_searchSep, m_ctrlData);
    SetChildRect(&m_btnRefresh, m_ctrlData);
    SetChildRect(&m_navBand, m_ctrlData);
    SetChildRect(&m_projName, m_ctrlData);
}

BOOL CProjMembers::IsWindowVisible()
{
    return m_hWnd != 0 && CWnd::IsWindowVisible();
}