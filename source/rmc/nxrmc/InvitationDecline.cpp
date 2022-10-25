// InvitationDecline.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "InvitationDecline.h"
#include "afxdialogex.h"


// CInvitationDecline dialog

IMPLEMENT_DYNAMIC(CInvitationDecline, CDialogEx)

CInvitationDecline::CInvitationDecline(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_PROJ_INVITE_DECLINE, pParent)
{

}

CInvitationDecline::~CInvitationDecline()
{
}

void CInvitationDecline::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_DECLINE_TEXT, m_declineText);
    DDX_Control(pDX, IDOK, m_btnDecline);
    DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CInvitationDecline, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CInvitationDecline message handlers


BOOL CInvitationDecline::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here
    m_declineText.SetCueBanner(theApp.LoadStringW(IDS_DECLINE_REASON_PROMPT));
    m_declineText.SetLimitText(249);
    m_btnDecline.SetColors(NX_NEXTLABS_STANDARD_GREEN, NX_NEXTLABS_STANDARD_WHITE);
    m_btnCancel.SetColors(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_BLACK, true);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CInvitationDecline::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class
    m_declineText.GetWindowText(m_outText);
    CDialogEx::OnOK();
}


void CInvitationDecline::OnCancel()
{
    // TODO: Add your specialized code here and/or call the base class

    CDialogEx::OnCancel();
}


void CInvitationDecline::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CDialogEx::OnPaint() for painting messages
}


BOOL CInvitationDecline::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CFlatDlgBase::OnEraseBkgnd(pDC);
}
