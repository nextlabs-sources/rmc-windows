// DlgFileInfo.cpp : implementation file
//

#include "stdafx.h"
#include "DlgFileInfo.h"
#include "afxdialogex.h"
#include "nxrmRMC.h"

// CDlgFileInfo dialog

IMPLEMENT_DYNAMIC(CDlgFileInfo, CFlatDlgBase)

CDlgFileInfo::CDlgFileInfo(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_FILEINFO, pParent)
	, m_strFileModify(_T(""))
	, m_strFileName(_T(""))
	, m_strFilePath(_T(""))
	, m_strFileSize(_T(""))
{
	m_bDeleted = FALSE;
	m_bRevoked = FALSE;
	m_bViewOnly = FALSE;
	m_bRepositoryFile = FALSE;
	m_bProjectFile = FALSE;
	m_bOwner = FALSE;
}

CDlgFileInfo::~CDlgFileInfo()
{
}

void CDlgFileInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RIGHT_BUTTON1, m_btnRight1);
	DDX_Control(pDX, IDC_RIGHT_BUTTON2, m_btnRight2);
	DDX_Control(pDX, IDC_RIGHT_BUTTON3, m_btnRight3);
	DDX_Control(pDX, IDC_RIGHT_BUTTON4, m_btnRight4);
	DDX_Control(pDX, IDC_RIGHT_BUTTON5, m_btnRight5);
	DDX_Text(pDX, IDC_STATIC_FILELASTMODIFY, m_strFileModify);
	DDX_Text(pDX, IDC_STATIC_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_STATIC_FILEPATH, m_strFilePath);
	DDX_Text(pDX, IDC_STATIC_FILESIZE, m_strFileSize);
	DDX_Control(pDX, IDC_STATIC_ORIGINALFILESTRING, m_stcOriginalFileText);
	DDX_Control(pDX, IDC_STATIC_FILEPATH, m_stcFilePath);
	DDX_Control(pDX, IDC_STATIC_RIGHTSSTRING, m_stcRightText);
}


BEGIN_MESSAGE_MAP(CDlgFileInfo, CDialogEx)
END_MESSAGE_MAP()


// CDlgFileInfo message handlers

BOOL CDlgFileInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CFont *m_pFont = new CFont();
	m_pFont->CreatePointFont(145, _T("Arial Bold"));
	GetDlgItem(IDC_STATIC_FILENAME)->SetFont(m_pFont, TRUE);
	m_pFont = new CFont();
	m_pFont->CreatePointFont(75, _T("Arial Bold"));
	GetDlgItem(IDC_STATIC_FILESIZE)->SetFont(m_pFont, TRUE);
	GetDlgItem(IDC_STATIC_FILELASTMODIFY)->SetFont(m_pFont, TRUE);
	GetDlgItem(IDC_STATIC_FILEPATH)->SetFont(m_pFont, TRUE);


	if (m_bRevoked || m_bDeleted || m_bViewOnly) {
		std::wstring title = theApp.LoadStringW(IDS_FILE_INFO).GetBuffer();
		if (m_bDeleted)
			title += theApp.LoadStringW(IDS_DELETE_SUFFIX).GetBuffer();
		else if (m_bRevoked)
			title += theApp.LoadStringW(IDS_REVOKE_SUFFIX).GetBuffer();
		SetWindowTextW(m_strFilePath);
	}

	if (m_bRepositoryFile) {
		m_stcOriginalFileText.SetWindowTextW(theApp.LoadStringW(IDS_LOCATION));
	}

	if (m_bOwner) {
		m_stcRightText.SetWindowTextW(theApp.LoadStringW(IDS_RIGHTS_TO_OTHERS));
	}

	m_btnRight1.ShowWindow(SW_HIDE);
	m_btnRight2.ShowWindow(SW_HIDE);
	m_btnRight3.ShowWindow(SW_HIDE);
	m_btnRight4.ShowWindow(SW_HIDE);
	m_btnRight5.ShowWindow(SW_HIDE);
	int index = 0;
	vector <wstring> orderedRighs = { L"VIEW",L"PRINT",L"DOWNLOAD",L"WATERMARK", L"SHARE" };
	for (size_t i = 0; i < orderedRighs.size(); i++)
	{
		//for (size_t sz = 0; sz < m_fileRights.size(); sz++) {
		for (auto iter = m_fileRights.begin(); iter != m_fileRights.end(); iter++)
		{
			if (*iter == orderedRighs[i])
			{
				if (LoadRightsIcon(index, *iter))
				{
					m_fileRights.erase(iter);
					index++;
					break;
				}
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgFileInfo::LoadRightsIcon(int idx, std::wstring rightstring)
{
	CBtnClass * pbutton;
	switch (idx) {
	case 0:
		pbutton = &m_btnRight1;
		break;
	case 1:
		pbutton = &m_btnRight2;
		break;
	case 2:
		pbutton = &m_btnRight3;
		break;
	case 3:
		pbutton = &m_btnRight4;
		break;
	case 4:
		pbutton = &m_btnRight5;
		break;
	default:
		ASSERT(TRUE);
		return FALSE;
	}

	if (!lstrcmpi(rightstring.c_str(), L"VIEW")) {
		pbutton->SetImages(IDB_PNG_RIGHTVIEW, 0, 0, NX_NEXTLABS_STANDARD_GRAY);;
	}
	else if (!lstrcmpi(rightstring.c_str(), L"SHARE")) {
		pbutton->SetImages(IDB_PNG_RIGHTSHARE, 0, 0, NX_NEXTLABS_STANDARD_GRAY);;
	}
	else if (!lstrcmpi(rightstring.c_str(), L"PRINT")) {
		pbutton->SetImages(IDB_PNG_RIGHTPRINT, 0, 0, NX_NEXTLABS_STANDARD_GRAY);;
	}
	else if (!lstrcmpi(rightstring.c_str(), L"DOWNLOAD")) {
		pbutton->SetImages(IDB_PNG_RIGHTDOWNLOAD, 0, 0, NX_NEXTLABS_STANDARD_GRAY);;
	}
	else if (!lstrcmpi(rightstring.c_str(), L"WATERMARK")) {
		pbutton->SetImages(IDB_PNG_RIGHTWATERMARK, 0, 0, NX_NEXTLABS_STANDARD_GRAY);;
	}
	else {
        MessageBox(rightstring.c_str(), theApp.LoadStringW(IDS_UNKNOWN_RIGHTS), MB_OK);
		return FALSE;
	}

	pbutton->ShowWindow(SW_SHOW);

	return TRUE;
}