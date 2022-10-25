// OneProject.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "OneProject.h"
#include "afxdialogex.h"
#include "ProjData.h"
#include "nxrmc.h"


// COneProjDlg dialog

IMPLEMENT_DYNAMIC(COneProjDlg, CFlatDlgBase)

COneProjDlg::COneProjDlg(CProjData &projData, CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_ONEPROJ, pParent),
    m_projData(projData)
{

}

COneProjDlg::COneProjDlg(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_ONEPROJ, pParent)
{

}


COneProjDlg::~COneProjDlg()
{
}

void COneProjDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROJ_USERS_PICTURES, m_userPictures);
    DDX_Control(pDX, IDC_PROJ_NAME, m_projName);
    DDX_Control(pDX, IDC_FILE_COUNT, m_fileCount);
    DDX_Control(pDX, IDC_SYSLINK_INVITE, m_inviteLink);
}


BEGIN_MESSAGE_MAP(COneProjDlg, CDialogEx)
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_STN_CLICKED(IDC_PROJ_USERS_PICTURES, &COneProjDlg::OnStnClickedProjUsersPictures)
    ON_BN_CLICKED(IDC_BUTTON_TEST, &COneProjDlg::OnBnClickedButtonTest)
    ON_STN_CLICKED(IDC_FILE_COUNT, &COneProjDlg::OnStnClickedFileCount)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_INVITE, &COneProjDlg::OnNMClickSyslinkInvite)
	ON_WM_PAINT()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// COneProjDlg message handlers


void COneProjDlg::OnDestroy()
{
    CFlatDlgBase::OnDestroy();
    // TODO: Add your message handler code here
}


BOOL COneProjDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CFlatDlgBase::OnEraseBkgnd(pDC);
}


HBRUSH COneProjDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


BOOL COneProjDlg::OnInitDialog()
{
    CFlatDlgBase::OnInitDialog();
    m_ToolTip.Create(this);

    AddFontSize(50, m_projName, false);

    m_userPictures.SetDrawType(NX_DRAWTYPE_USERS_IMAGE);

	if (m_projData.m_flag != NXRMC_PROJ_NOT_VALID)
	{
		ProjectRefresh();
	}

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void COneProjDlg::OnStnClickedProjUsersPictures()
{
    // TODO: Add your control notification handler code here
    MessageBox(L"Clicked on picture");
}


void COneProjDlg::OnBnClickedMfclinkInvite()
{
    MessageBox(L"Invite People", m_projData.m_projName);
    // TODO: Add your control notification handler code here
}


void COneProjDlg::OnBnClickedButtonTest()
{
    // TODO: Add your control notification handler code here
    MessageBox(L"Button Clicked");
}


void COneProjDlg::OnStnClickedFileCount()
{
    // TODO: Add your control notification handler code here
    MessageBox(L"Static Clicked");
}


void COneProjDlg::OnNMClickSyslinkInvite(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    MessageBox(L"Invite Clicked", m_projData.m_projName);
    *pResult = 0;
}


void COneProjDlg::OnPaint()
{
	//CFlatDlgBase::OnPaint();// for painting messages
    CPaintDC dc(this);
    CPen Pen(PS_SOLID, 1, NX_NEXTLABS_STANDARD_GRAY_BORDER);
    CRect rc;
    GetClientRect(&rc);
    auto oldPen = dc.SelectObject(&Pen);
    dc.MoveTo(rc.left, rc.top);
    dc.LineTo(rc.right-1, rc.top);
    dc.LineTo(rc.right-1, rc.bottom-1);
    dc.LineTo(rc.left, rc.bottom-1);
    dc.LineTo(rc.left, rc.top);
    dc.SelectObject(&oldPen);
	m_userPictures.InvalidateRect(NULL);

}


void COneProjDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	//65535 need change to project index
    ::PostMessage(theApp.m_mainhWnd, WM_NX_OPEN_PROJECT, 65535, (LPARAM)&m_projData);
    CFlatDlgBase::OnLButtonUp(nFlags, point);
}

void COneProjDlg::UpdateProjectInfo(CProjData &projData)
{
	m_projData = projData;
	ProjectRefresh();
}

void COneProjDlg::ProjectRefresh()
{
    vector<CString> &initialArr = m_userPictures.GetInitials();
    initialArr.clear();

    m_userPictures.SetTotalPics(m_projData.m_imembercount);

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

    AddTooTipWhenTruncated(this, &m_projName, &m_ToolTip, m_projData.m_projName, 1);


    wchar_t szText[64];
    wsprintf(szText, theApp.LoadString(IDS_FMT_D_FILES)/*L"%d files"*/, m_projData.m_fileCount);
    m_fileCount.SetWindowTextW(szText);

}


BOOL COneProjDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    m_ToolTip.RelayEvent(pMsg);
    return CFlatDlgBase::PreTranslateMessage(pMsg);
}
