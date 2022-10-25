// ManageFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ManageFileDlg.h"
#include "afxdialogex.h"
#include "nxrmRMC.h"


// CManageFileDlg dialog

IMPLEMENT_DYNAMIC(CManageFileDlg, CFlatDlgBase)

CManageFileDlg::CManageFileDlg(CWnd* pParent /*=NULL*/, BOOL bPropertyOnly /*FALSE*/)
	: CFlatDlgBase(IDD_MANAGEVAULT_DLG, pParent)
	, m_strSharedWithEmail(_T(""))
	, m_strFileName(_T(""))
{
	m_bPropertyOnly = bPropertyOnly;
	m_bManage = FALSE;
	m_bDeleted = FALSE;
	m_bRevoked = FALSE;
	m_bViewOnly = FALSE;
	m_bRepositoryFile = FALSE;
    m_bProjectFile = FALSE;
	m_bOwner = FALSE;
}

CManageFileDlg::~CManageFileDlg()
{
}

void CManageFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_REVOKE, m_btnRevoke);
	DDX_Text(pDX, IDC_STATIC_FILENAME, m_strFileName);
	DDX_Control(pDX, IDC_RIGHT_BUTTON1, m_btnRight1);
	DDX_Control(pDX, IDC_RIGHT_BUTTON2, m_btnRight2);
	DDX_Control(pDX, IDC_RIGHT_BUTTON3, m_btnRight3);
	DDX_Control(pDX, IDC_RIGHT_BUTTON4, m_btnRight4);
	DDX_Control(pDX, IDC_RIGHT_BUTTON5, m_btnRight5);
	DDX_Text(pDX, IDC_STATIC_SHAREDWITHEMAIL, m_strSharedWithEmail);
	DDX_Control(pDX, IDC_STATIC_SHAREWITHSTRING, m_stcShareWithText);
	DDX_Control(pDX, IDC_STATIC_RIGHTSSTRING, m_stcRightText);
	DDX_Control(pDX, IDC_STATIC_SHAREDWITHEMAIL, m_stcShareWithEmail);
	DDX_Control(pDX, IDC_BUTTON_MANAGE, m_btnManage);
}


BEGIN_MESSAGE_MAP(CManageFileDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_REVOKE, &CManageFileDlg::OnBnClickedBtnRevoke)
	ON_BN_CLICKED(IDC_BUTTON_MANAGE, &CManageFileDlg::OnBnClickedButtonManage)
END_MESSAGE_MAP()


// CManageFileDlg message handlers

BOOL CManageFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_btnRevoke.SetColors(NX_NEXTLABS_STANDARD_RED, RGB(255, 255, 255));
	CFont *m_pFont = new CFont();
	m_pFont->CreatePointFont(145, _T("Arial Bold"));
	GetDlgItem(IDC_STATIC_FILENAME)->SetFont(m_pFont, TRUE);
	m_pFont = new CFont();
	m_pFont->CreatePointFont(75, _T("Arial Bold"));
	GetDlgItem(IDC_STATIC_SHAREDWITHEMAIL)->SetFont(m_pFont, TRUE);

	m_btnManage.SetColors(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLUE);
	m_btnManage.SetTextAlignment(BS_LEFT);

	if (m_bPropertyOnly) {//Disable all buttons and move window and property controls up
		m_stcShareWithText.ShowWindow(SW_HIDE);
	}

	if (m_bRevoked || m_bDeleted || m_bPropertyOnly || m_bViewOnly) {
		std::wstring title = theApp.LoadStringW(IDS_DETAILS);
		if (m_bDeleted)
			title += theApp.LoadStringW(IDS_DELETE_SUFFIX);
		else if (m_bRevoked)
			title += theApp.LoadStringW(IDS_REVOKE_SUFFIX);

		SetWindowTextW(title.c_str());
		m_btnManage.EnableWindow(FALSE);
		m_btnManage.ShowWindow(SW_HIDE);
		m_btnRevoke.EnableWindow(FALSE);
		m_btnRevoke.ShowWindow(SW_HIDE);
		CString txt;
		txt.Format(theApp.LoadStringW(IDS_FMT_D_SHARED_WITH), (int)m_iSharedMembers);
		m_stcShareWithText.SetWindowTextW(txt);
	}
	else {
		m_btnRevoke.EnableWindow(m_fileRights.size() == 0 ? FALSE : TRUE);
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
        for (auto iter= m_fileRights.begin(); iter != m_fileRights.end(); iter++)
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

BOOL CManageFileDlg::LoadRightsIcon(int idx, std::wstring rightstring)
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
	else if (!lstrcmpi(rightstring.c_str(), L"SHARE")){
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

void CManageFileDlg::OnBnClickedBtnRevoke()
{
	if (MessageBox(theApp.LoadStringW(IDS_REVOKE_WARNING), theApp.LoadStringW(IDS_REVOKE_ALL_RIGHTS), MB_YESNOCANCEL | MB_ICONWARNING) == IDYES)
	{
		OnOK();
	}
}


void CManageFileDlg::OnBnClickedButtonManage()
{
	m_bManage = TRUE;
	OnOK();
}
