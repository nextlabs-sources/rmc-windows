// AddRepository.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "AddRepository.h"
#include "afxdialogex.h"
#include "repopage.h"
#include "weblogondlg.hpp"
#include <VersionHelpers.h>
#include "wininet.h"
#include <nx/common/stringex.h>


// CAddRepository dialog

vector<DriveTypes> drvTypeArray =
{
	{ REPO_DROPBOX, IDI_DROPBOX_BLACK,IDB_DROPBOX, L"" },
	{ REPO_GOOGLE, IDI_GOOGLEDRIVE_BLACK,IDB_GOOGLEDRIVE, L"" },
	//{ REPO_NEXTLABS, IDI_MYDRIVE_BLACK, IDB_MYDRIVE,L"NextLabs Drive"},
	{ REPO_ONEDRIVE, IDI_ONEDRIVE_BLACK, IDB_ONEDRIVE, L""},
	//{ REPO_SHAREPOINT, IDI_SHAREPOINT_BLACK, IDB_ONEDRIVE, L"SharePoint Online" }
};

IMPLEMENT_DYNAMIC(CAddRepository, CFlatDlgBase)

CAddRepository::CAddRepository(CStringArray &DispNames, CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_ADD_REPO, pParent)
	, m_strSiteURL(_T(""))
	, m_strDisplayName(_T(""))
{
	for (int i = 0; i < DispNames.GetCount(); i++) {
		m_strDisplayNames.Add(DispNames[i]);
	}
}

CAddRepository::~CAddRepository()
{
	m_strDisplayNames.RemoveAll();
}

void CAddRepository::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_COMBO1, m_repoTypeList);
	DDX_Control(pDX, IDC_COMBOBOXEX_REPOTYPE, m_typeList);
	DDX_Control(pDX, IDC_EDIT_LOGINNAME2, m_edtDisplayName);
	DDX_Control(pDX, IDC_EDT_SITEURL, m_edtSiteURL);
	DDX_Control(pDX, IDC_ONLINEURL, m_stcSiteURL);
	DDX_Text(pDX, IDC_EDT_SITEURL, m_strSiteURL);
	DDX_Control(pDX, ID_CANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_ADD_REPO, m_btnConnect);
	DDX_Text(pDX, IDC_EDIT_LOGINNAME2, m_strDisplayName);
	DDV_MaxChars(pDX, m_strDisplayName, 40);
}


BEGIN_MESSAGE_MAP(CAddRepository, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_ADD_REPO, &CAddRepository::OnBnClickedButtonAddRepo)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_REPOTYPE, &CAddRepository::OnCbnSelchangeComboboxexRepotype)
	ON_BN_CLICKED(ID_CANCEL, &CAddRepository::OnClickedCancel)
	ON_EN_CHANGE(IDC_EDT_SITEURL, &CAddRepository::OnChangeEdtSiteurl)
	ON_EN_CHANGE(IDC_EDIT_LOGINNAME2, &CAddRepository::OnChangeEditLoginname2)
	ON_COMMAND(IDOK, &CAddRepository::OnIdok)
END_MESSAGE_MAP()


// CAddRepository message handlers


BOOL CAddRepository::OnInitDialog()
{
    CDialogEx::OnInitDialog();
	//m_repoInageList.Create(32, 32, ILC_COLOR8, 0, 5);
	m_repoImageList.Create(32, 32, ILC_COLOR32, 0, 100);
	CreateIconList(m_repoImageList);

	m_typeList.SetImageList(&m_repoImageList);
	for (int i=0; i <(int) drvTypeArray.size(); i++)
	{
		COMBOBOXEXITEM item;
		item.mask = CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE;
		item.iItem = i;
		item.iImage = i;
		item.iSelectedImage = i;
		item.pszText = drvTypeArray[i].captionText.GetBuffer();
		item.cchTextMax = drvTypeArray[i].captionText.GetLength()+1;
		m_typeList.InsertItem(&item);
	}
	m_typeList.SetCurSel(0);
	m_iSelType = 0;
	m_stcSiteURL.ShowWindow(SW_HIDE);
	m_edtSiteURL.ShowWindow(SW_HIDE);

	m_btnConnect.SetColors(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK, TRUE);
	m_btnCancel.SetColors(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK, TRUE);

	m_btnConnect.EnableWindow(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddRepository::OnBnClickedButtonAddRepo()
{
    // TODO: Add your control notification handler code here

	UpdateData(TRUE);
	m_strDisplayName.Trim();
	if (m_strDisplayName.GetLength() == 0) {
		MessageBox(theApp.LoadStringW(IDS_DISPNAME_EMPTY), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		m_edtDisplayName.SetFocus();
		return;
	}
	for (int i = 0; i < m_strDisplayNames.GetCount(); i++) {
		if (0 == m_strDisplayNames[i].CompareNoCase(m_strDisplayName)) {
			MessageBox(theApp.LoadStringW(IDS_DISPNAME_DUP), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
			m_edtDisplayName.SetFocus();
			return;
		}
	}
	std::wstring szURL;
	std::wstring szSiteURL = L"";
	NX::RmRepository::REPO_TYPE rType;
	switch (m_typeList.GetCurSel())
	{
	case 0:
		rType = NX::RmRepository::DROPBOX;
		break;
	case 1:
		rType = NX::RmRepository::GOOGLEDRIVE;
		break;
	case 2:
		rType = NX::RmRepository::ONEDRIVE;
		break;
	case 3:
		rType = NX::RmRepository::SHAREPOINTONLINE;
		szSiteURL = m_strSiteURL.Trim();
		if (szSiteURL.length() == 0) {
			MessageBox(theApp.LoadStringW(IDS_URL_EMPTY), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
			m_edtSiteURL.SetFocus();
			return;
		}
		break;
	default:
		MessageBox(theApp.LoadString(IDS_NOTSUPPORTED_DRVTYPE), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		return;
	}

	NX::Result nResult;
	nResult = theApp.m_pSession->RepoGetAuthUrl(m_strDisplayName.GetString(), rType, szSiteURL, szURL);
	if (!nResult) {
		std::wstring errMsg;
		errMsg = NX::FormatString(theApp.LoadStringW(IDS_FMT_S_D_ERROR), theApp.LoadStringW(IDS_FAIL_TO_GET_AUTH_URL), nResult.GetCode());
		MessageBox(errMsg.c_str(), theApp.LoadStringW(IDS_ERROR), MB_OK | MB_ICONERROR);
		return;
	}

	CMainDlg dlg(AfxGetInstanceHandle(), GetSafeHwnd(), IDD_DIALOG_WEBLOGON_OAUTH, IsWindows10OrGreater());
    auto currUser = theApp.m_pSession->GetCurrentUser();
    auto userid = currUser.GetId();
    auto ticket = currUser.GetTicket();
    auto ticketStr = ticket.GetTicket();
	auto clientid = theApp.m_pSession->GetClientId().GetClientId();

	INT_PTR nRet = dlg.LogOnToProvider(szURL, L"/rms/main.jsp", clientid.c_str(), userid.c_str(), ticketStr.c_str());
	if (nRet == IDOK)
	{
		return OnOK();
	}
}

BOOL CAddRepository::CreateIconList(CImageList &imgList)
{
	
	for (auto drv : drvTypeArray)
	{
		HICON icon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(drv.nIcoID));
		auto n = imgList.Add(icon);
		DestroyIcon(icon);
	}
	return TRUE;
}

void CAddRepository::OnCbnSelchangeComboboxexRepotype()
{
	if (m_iSelType == m_typeList.GetCurSel())
		return;
	m_stcSiteURL.ShowWindow((m_typeList.GetCurSel() == 3) ? SW_SHOW : SW_HIDE);
	m_edtSiteURL.ShowWindow((m_typeList.GetCurSel() == 3) ? SW_SHOW : SW_HIDE);
	m_strDisplayName = L"";
	m_btnConnect.EnableWindow(FALSE);
	m_iSelType = m_typeList.GetCurSel();
	UpdateData(FALSE);
}


void CAddRepository::OnClickedCancel()
{
	OnCancel();
}


void CAddRepository::OnChangeEdtSiteurl()
{
	UpdateData(TRUE);
	m_btnConnect.EnableWindow(m_strDisplayName.Trim().GetLength() > 0 && m_strSiteURL.Trim().GetLength() > 0);
}


void CAddRepository::OnChangeEditLoginname2()
{
	UpdateData(TRUE);
	if (m_typeList.GetCurSel() != 3) {
		m_btnConnect.EnableWindow(m_strDisplayName.Trim().GetLength() > 0);
	}
	else {
		m_btnConnect.EnableWindow(m_strDisplayName.Trim().GetLength() > 0 && m_strSiteURL.Trim().GetLength() > 0);
	}
}


void CAddRepository::OnIdok()
{
	if (GetFocus()->m_hWnd == m_btnCancel.m_hWnd) {
		OnCancel();
	}
	else if (m_strDisplayName.GetLength() != 0){
		OnBnClickedButtonAddRepo();
	}
}
