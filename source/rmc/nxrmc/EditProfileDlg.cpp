// EditProfileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "EditProfileDlg.h"
#include "afxdialogex.h"
#include "nxrmRMC.h"



// CEditProfileDlg dialog

IMPLEMENT_DYNAMIC(CEditProfileDlg, CFlatDlgBase)

CEditProfileDlg::CEditProfileDlg(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_MANAGE_PROFILE, pParent)
	, m_strDispName(_T(""))
{

}

CEditProfileDlg::~CEditProfileDlg()
{
}

void CEditProfileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DISPLAYNAME, m_dispName);
	DDX_Control(pDX, IDC_EDIT_EMAILADDRESS, m_emailEdit);
	DDX_Control(pDX, IDC_EDIT_PHOBENUMBER, m_phoneNumber);
	DDX_Text(pDX, IDC_EDIT_DISPLAYNAME, m_strDispName);
	DDV_MaxChars(pDX, m_strDispName, 150);
	DDX_Control(pDX, IDOK, m_btnUpdate);
}


BEGIN_MESSAGE_MAP(CEditProfileDlg, CDialogEx)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CEditProfileDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_DISPLAYNAME, &CEditProfileDlg::OnChangeEditDisplayname)
END_MESSAGE_MAP()


// CEditProfileDlg message handlers


BOOL CEditProfileDlg::OnInitDialog()
{
    CFlatDlgBase::OnInitDialog();

    // TODO:  Add extra initialization here

	auto dispName = theApp.m_pSession->GetCurrentUser().GetName();
	auto emailAddress = theApp.m_pSession->GetCurrentUser().GetEmail();
	m_dispName.SetWindowText(dispName.c_str());
	m_emailEdit.SetWindowText(emailAddress.c_str());
	
	//m_emailAddr.SetWindowTextW(emailAddress.c_str());


    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CEditProfileDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CFlatDlgBase::OnEraseBkgnd(pDC);
}


HBRUSH CEditProfileDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


void CEditProfileDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_strDispName.Trim();
	if (m_strDispName.GetLength() == 0) {
		m_dispName.SetFocus();
		MessageBox(theApp.LoadStringW(IDS_DISPNAME_CANNOT_EMPTY), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		return;
	}
	auto res = theApp.m_pSession->UpdateProfileDisplayName(m_strDispName.GetBuffer());
	if (!res) {
		theApp.ShowTrayMsg(OutputError(res, (wstring)theApp.LoadStringW(IDS_FAIL_CHANGE_NAME)).c_str());
		return;
	}
	else {
		wstringstream txtOut;

		txtOut << theApp.LoadStringW(IDS_DISPNAME_CHANGED).GetBuffer();
		theApp.ShowTrayMsg(txtOut.str().c_str());
	}
	OnOK();
}


void CEditProfileDlg::OnChangeEditDisplayname()
{
	UpdateData(TRUE);
	m_btnUpdate.EnableWindow((m_strDispName.GetLength() == 0) ? FALSE: TRUE);
}
