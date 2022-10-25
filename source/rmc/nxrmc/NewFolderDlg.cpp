// NewFolderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewFolderDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "nxrmRMC.h"
// NewFolderDlg dialog

IMPLEMENT_DYNAMIC(NewFolderDlg, CDialogEx)

NewFolderDlg::NewFolderDlg(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_NEWFOLDER, pParent)
	, m_strFolderName(_T(""))
{

}

NewFolderDlg::~NewFolderDlg()
{
}

void NewFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FOLDERNAME, m_strFolderName);
	DDV_MaxChars(pDX, m_strFolderName, 255);
	DDX_Control(pDX, IDC_EDIT_FOLDERNAME, m_edtFolderName);
	DDX_Control(pDX, ID_CREATEFOLDER, m_btnCreate);
	DDX_Control(pDX, ID_CANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(NewFolderDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &NewFolderDlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_CANCEL, &NewFolderDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_CREATEFOLDER, &NewFolderDlg::OnBnClickedCreatefolder)
	ON_EN_CHANGE(IDC_EDIT_FOLDERNAME, &NewFolderDlg::OnChangeEditFoldername)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// NewFolderDlg message handlers


void NewFolderDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedCreatefolder();
}


void NewFolderDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void NewFolderDlg::OnBnClickedCreatefolder()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_strFolderName.Trim();
	if (m_strFolderName.GetLength() == 0) {
		MessageBox(theApp.LoadString(IDS_FOLDER_NAME_EMPTY), theApp.LoadString(IDS_ERROR), MB_OK | MB_ICONERROR);
		m_edtFolderName.SetFocus();
		return;
	}
	//leave the folder name restriction check to RMS. it may changed time by time or different server.

	for (size_t st = 0; st < m_strExistFolders.size(); st++) {
		if (0 == m_strFolderName.CompareNoCase(m_strExistFolders[st].c_str())) {
			MessageBox(theApp.LoadString(IDS_FOLDER_NAME_EXISTS), theApp.LoadString(IDS_ERROR), MB_OK | MB_ICONERROR);
			m_edtFolderName.SetFocus();
			return;
		}
	}

	UpdateData(FALSE);//update the string value
	OnOK();
}


BOOL NewFolderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_btnCreate.SetColors(NX_NEXTLABS_STANDARD_GREEN, RGB(255, 255, 255));
	m_btnCreate.EnableWindow(FALSE);
	return FALSE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void NewFolderDlg::OnChangeEditFoldername()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFlatDlgBase::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_btnCreate.EnableWindow(m_strFolderName.GetLength() > 0);

}

