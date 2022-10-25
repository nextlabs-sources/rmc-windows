// ProjMember.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "FlatDlgBase.h"
#include "ProjMember.h"
#include "nxrmrmc.h"
#include "afxdialogex.h"
#include <iomanip>
#include "MemberInfo.h"

extern CString GetFileTimeString(NX::time::datetime dt);

CString GetInitial(CString fullName);


// CProjMember dialog

IMPLEMENT_DYNAMIC(CProjMember, CDialogEx)


CProjMember::CProjMember(ProjMember & memberData, bool bPending,  CWnd * pParent)
    : CFlatDlgBase(IDD_DIALOG_MEMBER, pParent), m_memberData(memberData), m_bPending(bPending)
{
}

CProjMember::~CProjMember()
{
}

void CProjMember::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_JOIN_DATE, m_joinDate);
    DDX_Control(pDX, IDC_MEMBER_NAME, m_memberName);
    DDX_Control(pDX, IDC_BUTTON_HOST, m_btnHost);
    //DDX_Control(pDX, IDC_STATIC_USERPIC, m_userPic);
    DDX_Control(pDX, IDC_BUTTON_PERSON, m_person);
}


BEGIN_MESSAGE_MAP(CProjMember, CDialogEx)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_WM_LBUTTONUP()
    ON_BN_CLICKED(IDC_BUTTON_PERSON, &CProjMember::OnBnClickedButtonPerson)
END_MESSAGE_MAP()


// CProjMember message handlers


BOOL CProjMember::OnInitDialog()
{
    CFlatDlgBase::OnInitDialog();
	RefreshMember();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CProjMember::UpdateMember(ProjMember &member)
{
	m_memberData = member;
	RefreshMember();
}


void CProjMember::RefreshMember()
{
	m_memberName.SetWindowTextW(m_memberData.memberName.c_str());
	m_btnHost.SetColors(NX_NEXTLABS_STANDARD_GREEN, NX_NEXTLABS_STANDARD_WHITE);

	if (m_memberData.bOwner && !m_bPending)
	{
		m_btnHost.ShowWindow(SW_SHOW);
	}
	else
	{
		m_btnHost.ShowWindow(SW_HIDE);
	}

	//m_userPic.SetDrawType(NX_DRAWTYPE_USERS_IMAGE);
	//vector<CString> &initialArr = m_userPic.GetInitials();
	//auto initial = GetInitial(m_memberData.memberName.c_str());
	//initialArr.push_back(initial);

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

	m_person.SetColorsCircle(clrScheme.bg, clrScheme.fg, RGB(204, 204, 204), RGB(127, 127, 127), 16);
	m_person.InvalidateRect(NULL);
        auto joinDate = GetFileTimeString(m_memberData.dt);
	m_memberData.desc = joinDate;
	if (m_bPending)
	{
		joinDate = theApp.LoadString(IDS_INVITED_ON)/*L"Invited on "*/ + joinDate;
	}
	else
	{
		joinDate = theApp.LoadString(IDS_JOINED_ON)/*L"Joined on "*/ + joinDate;
	}
	m_joinDate.SetWindowTextW(joinDate);
}


BOOL CProjMember::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CFlatDlgBase::OnEraseBkgnd(pDC);
}


HBRUSH CProjMember::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


void CProjMember::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CFlatDlgBase::OnPaint() for painting messages
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


void CProjMember::OnDestroy()
{
    CFlatDlgBase::OnDestroy();
    //delete this;
   
}


void CProjMember::OnLButtonUp(UINT nFlags, CPoint point)
{
    CFlatDlgBase::OnLButtonUp(nFlags, point);
    OnBnClickedButtonPerson();
}


void CProjMember::OnBnClickedButtonPerson()
{
    CMemberInfo dlg(m_memberData, this);
    dlg.DoModal();
    // TODO: Add your control notification handler code here
}


void CProjMember::OnOK()
{
    // TODO: Add your specialized code here and/or call the base class

}


void CProjMember::OnCancel()
{
    // TODO: Add your specialized code here and/or call the base class

}
