// RepoEditor.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "RepoEditor.h"
#include "afxdialogex.h"
#include "ListBoxCH.h"
#include "RepoPage.h"



// CRepoEditor dialog

IMPLEMENT_DYNAMIC(CRepoEditor, CFlatDlgBase)

CRepoEditor::CRepoEditor(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_REPO_EDITOR, pParent)
	, m_strDisplayName(_T(""))
{
	m_bRenameOnly = FALSE;
	m_bDisconnectOnly = FALSE;
}

CRepoEditor::~CRepoEditor()
{
}

void CRepoEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_REPONAME, m_currRepoName);
	DDX_Control(pDX, IDC_STATIC_EMAIL_ADDRESS, m_emailAddress);
	DDX_Control(pDX, IDC_REPO_IMAGE, m_repoPicture);
	DDX_Control(pDX, IDC_BUTTON_DELETE_REPO, m_disconnectBtn);
	DDX_Control(pDX, IDC_STATIC_REPOTYPE, m_repoTypeName);
	DDX_Control(pDX, IDC_EDIT_DISPNAME, m_repoDispNameEdit);
	DDX_Control(pDX, IDC_BUTTON_UPDATE, m_btnUpdate);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_EDIT_DISPNAME, m_strDisplayName);
	DDV_MaxChars(pDX, m_strDisplayName, 40);
}


BEGIN_MESSAGE_MAP(CRepoEditor, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_REPO, &CRepoEditor::OnBnClickedButtonDeleteRepo)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CRepoEditor::OnBnClickedButtonUpdate)
	ON_EN_CHANGE(IDC_EDIT_DISPNAME, &CRepoEditor::OnChangeEditDispname)
END_MESSAGE_MAP()


// CRepoEditor message handlers

BOOL CRepoEditor::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    //for (auto a : repoArray)
    //{
    //    m_RepoList.AddItem(a.captionText, a.nIcoID, a.emailAddress);
    //}
    int idx = 0;
    //m_RepoList.SetCurSel(idx);

    static CBitmap bmp;
    if (bmp.GetSafeHandle())
    {
        bmp.DeleteObject();
    }
	auto imgID = GetRepoImageID(m_repository.Type);
    auto hBmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(imgID));
    m_repoPicture.SetBitmap(hBmp);
    m_currRepoName.SetWindowText(m_repository.Name.c_str());
	m_emailAddress.SetWindowText(m_repository.AccountName.c_str());
	m_repoDispNameEdit.SetWindowTextW(m_repository.Name.c_str());

	auto driveName = GetDriveTypeName(m_repository.Type);
	m_repoTypeName.SetWindowText(driveName.c_str());

    auto pfont = GetFont(); //(m_currRepoName.GetSafeHwnd());
    LOGFONT lf;
    pfont->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    lf.lfQuality = CLEARTYPE_QUALITY;
    auto hBoldFont = CreateFontIndirect(&lf);
    auto pBold = CFont::FromHandle(hBoldFont);

    m_currRepoName.SetFont(pBold);

    m_disconnectBtn.SetColors(NX_NEXTLABS_STANDARD_RED, RGB(255, 255, 255));
	m_btnCancel.SetColors(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK, TRUE);
	m_btnUpdate.SetColors(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK, TRUE);

	if (m_bRenameOnly) {
		SetWindowText(theApp.LoadString(IDS_RENAME_REPOSITORY)/*L"Rename Repository"*/);
		m_disconnectBtn.ShowWindow(SW_HIDE);
		m_disconnectBtn.EnableWindow(FALSE);
		m_repoDispNameEdit.SetFocus();
	}else if (m_bDisconnectOnly) {
		SetWindowText(theApp.LoadString(IDS_DELETE_REPOSOTPRY)/*L"Delete Repository"*/);
		m_repoDispNameEdit.EnableWindow(FALSE);
		m_btnUpdate.ShowWindow(SW_HIDE);
		m_btnUpdate.EnableWindow(FALSE);

		RECT rc;
		int dlgwidth, btnwidth;

		GetWindowRect(&rc);
		dlgwidth = rc.right - rc.left;
		m_btnCancel.GetWindowRect(&rc);
		btnwidth = rc.right - rc.left;
		ScreenToClient(&rc);
		rc.left = (dlgwidth - btnwidth) / 2;
		rc.right = rc.left + btnwidth;
		m_btnCancel.MoveWindow(&rc, TRUE);
	}

    return 0;
}


void CRepoEditor::OnBnClickedButtonDeleteRepo()
{
    // TODO: Add your control notification handler code here
	if (theApp.RemoveRepository(m_repository.Name)) {
		OnOK();
	}else{
		OnCancel();
	}
}

void CRepoEditor::SetCurrentRepository(RepositoryInfo &repository)
{
	m_repository = repository;
}


void CRepoEditor::OnBnClickedButtonUpdate()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_strDisplayName.Trim();
	if (m_strDisplayName.GetLength() == 0) {
		MessageBox(theApp.LoadString(IDS_DISPNAME_CANT_BE_EMPTY)/*L"Display name can't be empty!"*/, theApp.LoadString(IDS_ERROR)/*L"Error"*/, MB_OK);
		m_repoDispNameEdit.SetFocus();
		return;
	}
	if (theApp.RenameRepository(m_repository.Name, m_strDisplayName.GetBuffer())) {
		OnOK();
	}
	else {
		OnCancel();
	}
}


void CRepoEditor::OnChangeEditDispname()
{
	UpdateData(TRUE);
	m_btnUpdate.EnableWindow(m_strDisplayName.Trim().GetLength() > 0);
}
