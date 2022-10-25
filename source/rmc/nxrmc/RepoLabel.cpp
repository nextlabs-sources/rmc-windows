// RepoLabel.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RepoLabel.h"
#include "afxdialogex.h"


// CRepoLabel dialog
extern std::map <int, int> repoTypeToIcon;

IMPLEMENT_DYNAMIC(CRepoLabel, CDialogEx)

//CRepoLabel::CRepoLabel(CWnd* pParent /*=NULL*/)
//	: CDialogEx(IDD_DIALOG_REPOSOTORY, pParent)
//{
//
//}

//CRepoLabel::CRepoLabel(const RepoData & repoData, CWnd * pParent)
//    :m_repoData(repoData)
//{
//}

CRepoLabel::CRepoLabel(const RepositoryInfo & repoData, CWnd * pParent)
    : CFlatDlgBase(IDD_DIALOG_REPOSOTORY, pParent), m_repoData(repoData)
{
	m_hIcon = NULL;
}

CRepoLabel::~CRepoLabel()
{
}

void CRepoLabel::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_REPO_ICON, m_repoIcon);
    DDX_Control(pDX, IDC_REPO_NAME, m_repoName);
    DDX_Control(pDX, IDC_EMAIL_ADDRESS, m_repoEmail);
}


BEGIN_MESSAGE_MAP(CRepoLabel, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CRepoLabel message handlers


BOOL CRepoLabel::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_ToolTip.Create(this);
	InitializeWindows();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CRepoLabel::InitializeWindows(void)
{
	int id = 0;
	try
	{
		id = repoTypeToIcon.at(m_repoData.Type);
	}
	catch (...)
	{
		id = IDI_SHAREPOINT_BLACK;
	}
	if (NULL != m_hIcon) {
		DestroyIcon(m_hIcon);
	}
	m_hIcon = LoadIcon(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(id));
	m_repoIcon.SetIcon(m_hIcon);
	m_repoName.SetWindowText(m_repoData.Name.c_str());
	m_repoEmail.SetWindowText(m_repoData.AccountName.c_str());
    AddTooTipWhenTruncated(this, &m_repoName, &m_ToolTip, m_repoData.Name.c_str(), 1);
}
void CRepoLabel::UpdateRepoInfo(const RepositoryInfo & repoData)
{
	m_repoData = repoData;
	InitializeWindows();
}

HBRUSH CRepoLabel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        if (pWnd->GetDlgCtrlID() != IDC_REPO_ICON)
        {
            hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
            pDC->SetBkMode(TRANSPARENT);
            if (pWnd->GetDlgCtrlID() == IDC_EMAIL_ADDRESS)
            {
                pDC->SetTextColor(RGB(127,127,127));
            }
        }
    }
    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


void CRepoLabel::OnDestroy()
{
    CDialogEx::OnDestroy();
    DeleteObject(m_hIcon);
}


void CRepoLabel::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CDialogEx::OnPaint() for painting messages
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

}


void CRepoLabel::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    theApp.ShowPage(MD_REPORITORYHOME);
    theApp.m_dlgRepoPage.GotoRepository(NX_BTN_OTHERREPOS, m_repoData.Name);
    CDialogEx::OnLButtonUp(nFlags, point);
}


BOOL CRepoLabel::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    m_ToolTip.RelayEvent(pMsg);
    return CFlatDlgBase::PreTranslateMessage(pMsg);
}
