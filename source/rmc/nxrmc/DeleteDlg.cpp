// DeleteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeleteDlg.h"
#include "afxdialogex.h"
#include "nxrmRMC.h"

// DeleteDlg dialog

IMPLEMENT_DYNAMIC(CDeleteDlg, CFlatDlgBase)

CDeleteDlg::CDeleteDlg(BOOL bdirectory, CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_DELETE, pParent)
	, m_strFileDate(_T(""))
	, m_strFileName(_T(""))
	, m_strFileSize(_T(""))
{
	m_bIsDirectory = bdirectory;
}

CDeleteDlg::~CDeleteDlg()
{
}

void CDeleteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_FILEDATE, m_stcFileDate);
	DDX_Text(pDX, IDC_STATIC_FILEDATE, m_strFileDate);
	DDX_Text(pDX, IDC_STATIC_FILENAME, m_strFileName);
	DDX_Control(pDX, IDC_STATIC_FILESIZE, m_stcFileSize);
	DDX_Text(pDX, IDC_STATIC_FILESIZE, m_strFileSize);
	DDX_Control(pDX, IDC_STATIC_LASTMODIFYSTRING, m_stcLastModify);
	DDX_Control(pDX, IDC_STATIC_SIZESTRING, m_stcSize);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDDELETE, m_btnDelete);
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_stcFileName);
}


BEGIN_MESSAGE_MAP(CDeleteDlg, CDialogEx)
	ON_BN_CLICKED(IDDELETE, &CDeleteDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDCANCEL, &CDeleteDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// DeleteDlg message handlers


void CDeleteDlg::OnBnClickedDelete()
{
	OnOK();
}


BOOL CDeleteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString title;
	GetWindowText(title);
    CString dirName = theApp.LoadStringW(IDS_DIRECTORY);
    CString fName = theApp.LoadStringW(IDS_FILE);
	title.Format(title, m_bIsDirectory? dirName.GetBuffer() : fName.GetBuffer());
	CString str;
	GetDlgItem(IDC_STATIC_DELETESTRING)->GetWindowTextW(str);
    CString dirName2 = theApp.LoadStringW(IDS_DIRECTORY2);
    CString fName2 = theApp.LoadStringW(IDS_FILE2);
	str.Format(theApp.LoadStringW(IDS_FMT_S_CONFIRM_DELETE), m_bIsDirectory? dirName2.GetBuffer() : fName2.GetBuffer());
	GetDlgItem(IDC_STATIC_DELETESTRING)->SetWindowTextW(str);
	m_stcFileSize.ShowWindow(m_bIsDirectory? SW_HIDE: SW_SHOW);
	m_stcFileDate.ShowWindow(m_bIsDirectory ? SW_HIDE : SW_SHOW);
	m_stcSize.ShowWindow(m_bIsDirectory ? SW_HIDE : SW_SHOW);
	m_stcLastModify.ShowWindow(m_bIsDirectory ? SW_HIDE : SW_SHOW);
	SetWindowText(title);

	CFont *m_pFont = new CFont();
	m_pFont->CreatePointFont(145, _T("Arial Bold"));
	m_stcFileName.SetFont(m_pFont, TRUE);
	m_pFont = new CFont();
	m_pFont->CreatePointFont(75, _T("Arial Bold"));
	m_stcFileSize.SetFont(m_pFont, TRUE);
	m_stcFileDate.SetFont(m_pFont, TRUE);

	m_btnDelete.SetColors(NX_NEXTLABS_STANDARD_RED, RGB(255, 255, 255), TRUE);
	m_btnCancel.SetColorsWithBorder(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDeleteDlg::OnBnClickedCancel()
{
	OnCancel();
}
