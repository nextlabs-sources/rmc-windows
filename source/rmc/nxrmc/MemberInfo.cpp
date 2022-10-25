// MemberInfo.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MemberInfo.h"
#include "afxdialogex.h"
#include "nxrmRMC.h"
#include "ProjMembers.h"

// CMemberInfo dialog
CString GetInitial(CString fullName);


IMPLEMENT_DYNAMIC(CMemberInfo, CDialogEx)

CMemberInfo::CMemberInfo(const ProjMember & memberData, CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_PROJ_MEMBER_INFO, pParent), m_memberData(memberData)
{

}

//CMemberInfo::CMemberInfo(const ProjMember & memberData, CWnd * pParent) : m_memberData(memberData)
//{
//}

CMemberInfo::~CMemberInfo()
{
}

void CMemberInfo::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_PERSON, m_person);
    DDX_Control(pDX, IDC_STATIC_NAME, m_name);
    DDX_Control(pDX, IDC_STATIC_EMAIL, m_email);
    DDX_Control(pDX, IDC_STATIC_DATE, m_date);
    DDX_Control(pDX, IDC_BUTTON_RESEND, m_btnResend);
    DDX_Control(pDX, IDC_BUTTON_REVOKE, m_btnRevoke);
    DDX_Control(pDX, IDC_BUTTON_REMOVE, m_btnRemove);
    DDX_Control(pDX, IDC_STATIC_INVITEDBY, n_invite);
}


BEGIN_MESSAGE_MAP(CMemberInfo, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BUTTON_REVOKE, &CMemberInfo::OnBnClickedButtonRevoke)
    ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CMemberInfo::OnBnClickedButtonRemove)
    ON_BN_CLICKED(IDC_BUTTON_RESEND, &CMemberInfo::OnBnClickedButtonResend)
END_MESSAGE_MAP()


// CMemberInfo message handlers


BOOL CMemberInfo::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_btnRemove.SetColors(NX_NEXTLABS_STANDARD_RED, NX_NEXTLABS_STANDARD_WHITE);
    m_btnRevoke.SetColors(NX_NEXTLABS_STANDARD_RED, NX_NEXTLABS_STANDARD_WHITE);
    m_btnResend.SetColors(NX_NEXTLABS_STANDARD_GREEN, NX_NEXTLABS_STANDARD_WHITE);



    AddFontSize(80, m_name, false);
    m_name.SetWindowText(m_memberData.memberName.c_str());
    // TODO:  Add extra initialization here
    auto initial = GetInitial(m_memberData.memberName.c_str());
    m_person.SetWindowText(initial);

    RmColorSchemeEntry clrScheme;
    if (initial.GetLength())
    {
        clrScheme = GetColorScheme(initial.GetAt(0));
    }
    else
    {
        clrScheme = GetColorScheme(L' '); //Get default color here
    }

    m_person.SetColorsCircle(clrScheme.bg, clrScheme.fg);

    if (m_memberData.bOwner && m_memberData.bProjByMe)
    {
        m_email.SetWindowText(m_memberData.emailAddr.c_str());
        CString joindDate = theApp.LoadString(IDS_JOINED_ON) + m_memberData.desc.c_str();
        m_date.SetWindowText(joindDate);

        m_btnResend.ShowWindow(SW_HIDE);
        m_btnRevoke.ShowWindow(SW_HIDE);
        m_btnRemove.ShowWindow(SW_HIDE);
        n_invite.ShowWindow(SW_HIDE);
    } 
    else if (!m_memberData.bOwner && m_memberData.bProjByMe)
    {
        m_email.SetWindowText(m_memberData.emailAddr.c_str());

        if (m_memberData.bPending)
        {
            CString joindDate = theApp.LoadString(IDS_INVITED_ON) + m_memberData.desc.c_str();
            m_date.SetWindowText(joindDate);

            m_btnRemove.ShowWindow(SW_HIDE);
            n_invite.ShowWindow(SW_HIDE);
            CString invitestr = theApp.LoadString(IDS_INVITED_BY) + m_memberData.invitorName.c_str();
            m_email.SetWindowText(invitestr);
        }
        else
        {
            CString joindDate = theApp.LoadString(IDS_JOINED_ON) + m_memberData.desc.c_str();
            m_date.SetWindowText(joindDate);
            m_btnResend.ShowWindow(SW_HIDE);
            m_btnRevoke.ShowWindow(SW_HIDE);
            if (m_memberData.invitorName == L"")
            {
                n_invite.ShowWindow(SW_HIDE);
            }
            else
            {
                CString invitestr = theApp.LoadString(IDS_INVITED_BY) + m_memberData.invitorName.c_str();
                n_invite.SetWindowText(invitestr);
            }
        }

    }
    else if (m_memberData.bOwner && !m_memberData.bProjByMe)
    {
        m_email.SetWindowText(m_memberData.emailAddr.c_str());
        CString joindDate = theApp.LoadString(IDS_JOINED_ON) + m_memberData.desc.c_str();
        m_date.SetWindowText(joindDate);
        m_btnResend.ShowWindow(SW_HIDE);
        m_btnRevoke.ShowWindow(SW_HIDE);
        m_btnRemove.ShowWindow(SW_HIDE);
        n_invite.ShowWindow(SW_HIDE);
        //m_date.ShowWindow(SW_HIDE);
    } 
    else if (!m_memberData.bOwner && !m_memberData.bProjByMe)
    {
       
        
        if (m_memberData.bPending)
        {
            CString invitestr = theApp.LoadString(IDS_INVITED_BY) + m_memberData.invitorName.c_str();
            m_email.SetWindowText(invitestr);
            invitestr = theApp.LoadString(IDS_INVITED_ON) + m_memberData.desc.c_str();
            m_date.SetWindowText(invitestr);
            m_btnResend.ShowWindow(SW_HIDE);
            m_btnRevoke.ShowWindow(SW_HIDE);
            m_btnRemove.ShowWindow(SW_HIDE);
            n_invite.ShowWindow(SW_HIDE);
        }
        else
        {
            m_email.SetWindowText(m_memberData.emailAddr.c_str());
            CString invitestr = theApp.LoadString(IDS_JOINED_ON) + m_memberData.desc.c_str();

            m_date.SetWindowText(invitestr);
            if (m_memberData.invitorName == L"")
            {
                n_invite.ShowWindow(SW_HIDE);
            }
            else
            {
                invitestr = theApp.LoadString(IDS_INVITED_BY) + m_memberData.invitorName.c_str();
                n_invite.SetWindowText(invitestr);
            }
            m_btnResend.ShowWindow(SW_HIDE);
            m_btnRevoke.ShowWindow(SW_HIDE);
            m_btnRemove.ShowWindow(SW_HIDE);

        }
    }





    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CMemberInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
        pDC->SetBkMode(TRANSPARENT);
    }

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


BOOL CMemberInfo::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CFlatDlgBase::OnEraseBkgnd(pDC);
}


void CMemberInfo::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CFlatDlgBase::OnPaint() for painting messages
}


void CMemberInfo::OnBnClickedButtonRevoke()
{
    // TODO: Add your control notification handler code here

    if (IDYES == MessageBox(theApp.LoadString(IDS_CONFIRM_REVOKE), theApp.LoadString(IDS_REVOKE_INVITATION), MB_ICONQUESTION | MB_YESNO))
    {
        if (theApp.RevokeInvitation(m_memberData.memberID))
        {
            //theApp.m_dlgProjMain.m_membersDlg.OnBnClickedButtonRefresh();
            theApp.m_dlgProjMain.m_membersDlg.PostMessage(WM_COMMAND, IDC_BUTTON_REFRESH, 0L);
        }
        CDialogEx::OnOK();
    }
}

void CMemberInfo::OnBnClickedButtonRemove()
{
    if (IDYES == MessageBox(theApp.LoadString(IDS_CONFIRM_REMOVE), theApp.LoadString(IDS_REMOVE_MEMBER), MB_ICONQUESTION | MB_YESNO))
    {
        if (theApp.RemoveProjMember(m_memberData.projID, m_memberData.memberID))
        {
            theApp.m_dlgProjMain.m_summaryDlg.SetProject();
            theApp.m_dlgProjMain.m_membersDlg.PostMessage(WM_COMMAND, IDC_BUTTON_REFRESH, 0L);
        }
        CDialogEx::OnOK();
    }
}


void CMemberInfo::OnBnClickedButtonResend()
{
    if (theApp.ResendInvitation(m_memberData.memberID, m_memberData.emailAddr))
    {
        if (theApp.m_dlgProjMain.m_membersDlg.GetSafeHwnd())
        {
            theApp.m_dlgProjMain.m_membersDlg.PostMessage(WM_COMMAND, IDC_BUTTON_REFRESH, 0L);
        }
    }
    CDialogEx::OnOK();
}
