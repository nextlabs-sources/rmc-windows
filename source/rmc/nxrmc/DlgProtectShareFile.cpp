// DlgProtectShareFile.cpp : implementation file
//

#include "stdafx.h"
#include "DlgProtectShareFile.h"
#include "afxdialogex.h"
#include "nxrmRMC.h"


// CDlgProtectShareFile dialog

IMPLEMENT_DYNAMIC(CDlgProtectShareFile, CFlatDlgBase)

CDlgProtectShareFile::CDlgProtectShareFile(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_PROSHARELOCALFILE, pParent)
	, m_strFileName(_T(""))
{
	m_bShareFile = FALSE;
	m_bProtectFile = FALSE;
}

CDlgProtectShareFile::~CDlgProtectShareFile()
{
}

void CDlgProtectShareFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_FILENAME, m_strFileName);
	DDX_Control(pDX, ID_PROTECT, m_btnProtect);
	DDX_Control(pDX, ID_SHARED, m_btnShare);
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_stcFileName);
}


BEGIN_MESSAGE_MAP(CDlgProtectShareFile, CDialogEx)
	ON_BN_CLICKED(ID_PROTECT, &CDlgProtectShareFile::OnBnClickedProtect)
	ON_BN_CLICKED(ID_SHARED, &CDlgProtectShareFile::OnBnClickedShared)
END_MESSAGE_MAP()


// CDlgProtectShareFile message handlers


void CDlgProtectShareFile::OnBnClickedProtect()
{
	m_bProtectFile = TRUE;
	OnOK();
}


void CDlgProtectShareFile::OnBnClickedShared()
{
	m_bShareFile = TRUE;
	OnOK();
}


BOOL CDlgProtectShareFile::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_FilePath);

	CFont *m_pFont = new CFont();
	m_pFont->CreatePointFont(145, _T("Arial Bold"));
	m_stcFileName.SetFont(m_pFont, TRUE);
// 	m_pFont = new CFont();
// 	m_pFont->CreatePointFont(75, _T("Arial Bold"));
// 	m_stcFileSize.SetFont(m_pFont, TRUE);
// 	m_stcFileDate.SetFont(m_pFont, TRUE);

	m_btnShare.SetColorsWithBorder(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK);
	m_btnProtect.SetColorsWithBorder(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
