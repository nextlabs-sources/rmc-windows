// ProjByOthersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "nxrmc.h"
#include "ProjByOthersDlg.h"
#include "InvitationDecline.h"
#include "afxdialogex.h"


// CProjByOthersDlg dialog

IMPLEMENT_DYNAMIC(CProjByOthersDlg, CFlatDlgBase)

CProjByOthersDlg::CProjByOthersDlg(CProjData &prjData, CWnd * pParent)
    : CFlatDlgBase(IDD_PROJ_BY_OTHERS, pParent)
    , m_projData(prjData)
{
}

CProjByOthersDlg::CProjByOthersDlg(CWnd * pParent)
	: CFlatDlgBase(IDD_PROJ_BY_OTHERS, pParent)
{
}


CProjByOthersDlg::~CProjByOthersDlg()
{
}

void CProjByOthersDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROJECT_NAME, m_projName);
    DDX_Control(pDX, IDC_OWNER, m_owner);
    DDX_Control(pDX, IDC_OWNER_NAME, m_ownerName);
    DDX_Control(pDX, IDC_STATIC_PIC, m_userPics);
    DDX_Control(pDX, IDC_BUTTON_ACCEPT, m_btnAccept);
    DDX_Control(pDX, IDC_BUTTON_DECLINE, m_btnDecline);
}


BEGIN_MESSAGE_MAP(CProjByOthersDlg, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_JOIN, &CProjByOthersDlg::OnNMClickSyslinkJoin)
	ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BUTTON_MD_VIEWFILE, &CProjByOthersDlg::OnBnClickedButtonMdViewMyDrivefile)
    ON_WM_LBUTTONUP()
    ON_BN_CLICKED(IDC_BUTTON_DECLINE, &CProjByOthersDlg::OnBnClickedButtonDecline)
    ON_BN_CLICKED(IDC_BUTTON_ACCEPT, &CProjByOthersDlg::OnBnClickedButtonAccept)
END_MESSAGE_MAP()


// CProjByOthersDlg message handlers


BOOL CProjByOthersDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_ToolTip.Create(this);
    AddFontSize(50, m_projName, false);

    m_userPics.SetDrawType(NX_DRAWTYPE_USERS_IMAGE);
	if (m_projData.m_flag != NXRMC_PROJ_NOT_VALID)
	{
		ProjectRefresh();
	}

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CProjByOthersDlg::UpdateProjectInfo(CProjData &prjData)
{
	m_projData = prjData;
	ProjectRefresh();
}
void CProjByOthersDlg::ProjectRefresh()
{
    m_userPics.SetTotalPics(m_projData.m_imembercount);
    vector<CString> &initialArr = m_userPics.GetInitials();
    initialArr.clear();

    for (auto x : m_projData.m_initials)
    {
        CString initial = L"";
        int pos = 0;
        pos = x.Find(L' ');
        if (pos == -1)
        {
            CString first = x;
            first.TrimLeft();
            initial.AppendChar(first[0]);
            initial.MakeUpper();
            initialArr.push_back(initial);
            continue;
        }
        auto firstName = x.Left(pos);
        firstName.TrimLeft();
        initial.AppendChar(firstName[0]);
        pos = x.ReverseFind(L' ');
        auto beforeLast = x.Left(pos);
        beforeLast.TrimLeft();
        auto lastName = x.Right(x.GetLength() - pos);
        lastName.TrimLeft();
        if (beforeLast.GetLength() && lastName.GetLength())
        {
            initial.AppendChar(lastName[0]);
        }
        initial.MakeUpper();
        initialArr.push_back(initial);
    }

    m_projName.SetWindowText(m_projData.m_projName);
    m_ToolTip.UpdateTipText(m_projData.m_projName, this, 1);
    if (IsTextTruncated(&m_projName, 50))
    {
        m_ToolTip.Activate(TRUE);
    }
    else
    {
        m_ToolTip.Activate(FALSE);
    }

    m_ownerName.SetWindowTextW(m_projData.m_ownerName);

    if (m_projData.m_flag == NXRMC_PROJ_BY_OTHERS)
    {
        m_btnAccept.ShowWindow(SW_HIDE);
        m_btnDecline.ShowWindow(SW_HIDE);
        m_userPics.ShowWindow(SW_SHOW);
    }
    else
    {
        m_btnAccept.ShowWindow(SW_SHOW);
        m_btnDecline.ShowWindow(SW_SHOW);
        m_btnAccept.SetColors(NX_NEXTLABS_STANDARD_GREEN, NX_NEXTLABS_STANDARD_WHITE);
        m_btnDecline.SetColors(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK, TRUE);
        m_owner.SetWindowText(L"Invited by");
        //m_ownerName.SetWindowTextW(m_projData.m_invitorName);
        m_ownerName.ShowWindow(SW_SHOW);
        m_userPics.ShowWindow(SW_HIDE);
    }
    AddTooTipWhenTruncated(this, &m_projName, &m_ToolTip, m_projData.m_projName, 1);

}



HBRUSH CProjByOthersDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


BOOL CProjByOthersDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CFlatDlgBase::OnEraseBkgnd(pDC);
}


void CProjByOthersDlg::OnNMClickSyslinkJoin(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    MessageBox(L"Join to this project", m_projData.m_projName);
    *pResult = 0;
}


void CProjByOthersDlg::OnPaint()
{
	//CFlatDlgBase::OnPaint();
    CPaintDC dc(this);
    CPen Pen(PS_SOLID, 1, NX_NEXTLABS_STANDARD_GRAY_BORDER);
    CRect rc;
    GetClientRect(&rc);
    auto oldPen = dc.SelectObject(&Pen);
    dc.MoveTo(rc.left, rc.top);
    dc.LineTo(rc.right - 1, rc.top);
    dc.LineTo(rc.right - 1, rc.bottom - 1);
    dc.LineTo(rc.left, rc.bottom - 1);
    dc.LineTo(rc.left, rc.top);
    dc.SelectObject(&oldPen);
	m_userPics.InvalidateRect(NULL);
}


void CProjByOthersDlg::OnBnClickedButtonMdViewMyDrivefile()
{
    // TODO: Add your control notification handler code here
}


void CProjByOthersDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (m_projData.m_flag == NXRMC_PROJ_BY_OTHERS)
    {
		//65535 need change to project index 
        ::PostMessage(theApp.m_mainhWnd, WM_NX_OPEN_PROJECT, 65535, (LPARAM)&m_projData);
    }
    CFlatDlgBase::OnLButtonUp(nFlags, point);
}


void CProjByOthersDlg::OnBnClickedButtonAccept()
{
    wstring projectId;
    if (theApp.ProjectAcceptInvitation(m_projData.m_nID, m_projData.m_nCode, projectId))
    {
        theApp.m_homePage.PostMessage(WM_NX_REFRESH_PROJ_LIST);
        theApp.m_dlgProjPage.PostMessage(WM_NX_REFRESH_PROJ_LIST);
    }
}
void CProjByOthersDlg::OnBnClickedButtonDecline()
{
    CInvitationDecline dlg(CWnd::FromHandle(theApp.m_mainhWnd));
    if (dlg.DoModal() == IDOK)
    {
        wstring projectId;
        if (theApp.ProjectDeclineInvitation(m_projData.m_nID, m_projData.m_nCode, dlg.m_outText.GetBuffer()))
        {
            theApp.m_homePage.PostMessage(WM_NX_REFRESH_PROJ_LIST);
            theApp.m_dlgProjPage.PostMessage(WM_NX_REFRESH_PROJ_LIST);
        }
    }
}




BOOL CProjByOthersDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    m_ToolTip.RelayEvent(pMsg);
    return CFlatDlgBase::PreTranslateMessage(pMsg);
}
