// ChangePswd.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "ChangePswd.h"
#include "afxdialogex.h"


// CChangePswd dialog

IMPLEMENT_DYNAMIC(CChangePswd, CFlatDlgBase)

CChangePswd::CChangePswd(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_CHANGE_PSWD, pParent)
	, m_strConfirmPswd(_T(""))
	, m_strCurrPswd(_T(""))
	, m_strNewPswd(_T(""))
{

}

CChangePswd::~CChangePswd()
{
}

void CChangePswd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CONFIRM_PSWD, m_confirmPswd);
	DDX_Control(pDX, IDC_EDIT_NEW_PSWD, m_newPswd);
	DDX_Control(pDX, IDC_EDIT_CURR_PSWD, m_currPswd);
	DDX_Control(pDX, IDC_STATIC_USER_EMAIL, m_emailAddr);
	DDX_Control(pDX, IDOK, m_btnChange);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_EDIT_CONFIRM_PSWD, m_strConfirmPswd);
	DDV_MaxChars(pDX, m_strConfirmPswd, 64);
	DDX_Text(pDX, IDC_EDIT_CURR_PSWD, m_strCurrPswd);
	DDV_MaxChars(pDX, m_strCurrPswd, 64);
	DDX_Text(pDX, IDC_EDIT_NEW_PSWD, m_strNewPswd);
	DDV_MaxChars(pDX, m_strNewPswd, 64);
}


BEGIN_MESSAGE_MAP(CChangePswd, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDOK, &CChangePswd::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_NEW_PSWD, &CChangePswd::OnChangeEditNewPswd)
	ON_EN_CHANGE(IDC_EDIT_CURR_PSWD, &CChangePswd::OnChangeEditCurrPswd)
	ON_EN_CHANGE(IDC_EDIT_CONFIRM_PSWD, &CChangePswd::OnChangeEditConfirmPswd)
END_MESSAGE_MAP()


// CChangePswd message handlers


HBRUSH CChangePswd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);
	//auto emailAddress = theApp.m_pSession->GetCurrentUser().GetEmail();
	//m_emailAddr.SetWindowTextW(emailAddress.c_str());

    return hbr;
}


void CChangePswd::OnPaint()
{
    CFlatDlgBase::OnPaint();
}


void CChangePswd::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_strNewPswd.Trim();
	m_strConfirmPswd.Trim();
	m_strCurrPswd.Trim();
	if (m_strCurrPswd == L"")
	{
		MessageBox(theApp.LoadStringW(IDS_CUR_PSWD_CANNOT_EMPTY), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		m_newPswd.SetFocus();
		return;
	}
	if (m_strNewPswd == L"")
    {
        MessageBox(theApp.LoadStringW(IDS_NEW_PSWD_CONNOT_EMPTY), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		m_newPswd.SetFocus();
        return;
    }
	if (m_strNewPswd != m_strConfirmPswd)
	{
		MessageBox(theApp.LoadStringW(IDS_PSWD_NOTMATCH), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		m_confirmPswd.SetFocus();
		return;
	}
	if (m_strNewPswd == m_strCurrPswd)
	{
		MessageBox(theApp.LoadStringW(IDS_PSWD_IS_DUP), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		m_newPswd.SetFocus();
		return;
	}
	auto res = theApp.m_pSession->ChangePassword(m_strCurrPswd.GetBuffer(), m_strNewPswd.GetBuffer());
	if (!res) {
		switch (res.GetCode()) {
		case 4001:
			MessageBox(theApp.LoadStringW(IDS_PSWD_INCORRECT), theApp.LoadStringW(IDS_ERROR), MB_OK| MB_ICONERROR);
			return;
		case ERROR_INVALID_PASSWORD:
			MessageBox(theApp.LoadStringW(IDS_PSWD_REQUIREMENT), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
			return;
		}

		theApp.ShowTrayMsg(OutputError(res, theApp.LoadStringW(IDS_FAILED_TO_CHANGE_PSWD).GetBuffer()).c_str());
	}
	else {
		wstringstream txtOut;

		txtOut << theApp.LoadStringW(IDS_PSWD_CHANGED).GetBuffer();
		theApp.ShowTrayMsg(txtOut.str().c_str());
	}

	OnOK();
}


BOOL CChangePswd::OnInitDialog()
{
	CFlatDlgBase::OnInitDialog();

	// TODO:  Add extra initialization here
	auto emailAddress = theApp.m_pSession->GetCurrentUser().GetEmail();
	m_emailAddr.SetWindowTextW(emailAddress.c_str());
	m_btnChange.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CChangePswd::OnChangeEditNewPswd()
{
	UpdateData(TRUE);
	m_btnChange.EnableWindow((m_strNewPswd.GetLength() > 0 && m_strCurrPswd.GetLength() > 0 && m_strConfirmPswd.GetLength() > 0) ? TRUE : FALSE);
}


void CChangePswd::OnChangeEditCurrPswd()
{
	UpdateData(TRUE);
	m_btnChange.EnableWindow((m_strNewPswd.GetLength() > 0 && m_strCurrPswd.GetLength() > 0 && m_strConfirmPswd.GetLength() > 0) ? TRUE : FALSE);
}


void CChangePswd::OnChangeEditConfirmPswd()
{
	UpdateData(TRUE);
	m_btnChange.EnableWindow((m_strNewPswd.GetLength() > 0 && m_strCurrPswd.GetLength() > 0 && m_strConfirmPswd.GetLength() > 0) ? TRUE : FALSE);
}
