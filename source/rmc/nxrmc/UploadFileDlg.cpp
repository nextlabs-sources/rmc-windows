// UploadFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UploadFileDlg.h"
#include "afxdialogex.h"
#include "nxrmRMC.h"


// CUploadFileDlg dialog

IMPLEMENT_DYNAMIC(CUploadFileDlg, CDialogEx)

CUploadFileDlg::CUploadFileDlg(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_UPLOADFILE, pParent)
	, m_strFileName(_T(""))
	, m_strLastModify(_T(""))
	, m_strPath(_T(""))
	, m_strSize(_T(""))
{

}

CUploadFileDlg::~CUploadFileDlg()
{
}

void CUploadFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_STATIC_PATH, m_strPath);
	DDX_Control(pDX, ID_CANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnProtect);
}


BEGIN_MESSAGE_MAP(CUploadFileDlg, CDialogEx)
	ON_BN_CLICKED(ID_CANCEL, &CUploadFileDlg::OnClickedCancel)
END_MESSAGE_MAP()


// CUploadFileDlg message handlers


BOOL CUploadFileDlg::OnInitDialog()
{
	CFlatDlgBase::OnInitDialog();

	m_btnProtect.SetColors(NX_NEXTLABS_STANDARD_GREEN, RGB(255, 255, 255));

	CFont *m_pFont = new CFont();
	m_pFont->CreatePointFont(160, _T("Arial Bold"));
	GetDlgItem(IDC_STATIC_PATH)->SetFont(m_pFont, TRUE);
	GetDlgItem(IDC_STATIC_FILENAME)->SetFont(m_pFont, TRUE);

	m_bUpdateOnly = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CUploadFileDlg::OnClickedCancel()
{
	m_bUpdateOnly = TRUE;
	OnCancel();
}
