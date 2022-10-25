// ConnectToRepo.cpp : implementation file
//

#include "stdafx.h"
#include "ConnectToRepo.h"
#include "afxdialogex.h"
#include "resource.h"
#include "nxrmRMC.h"
#include "RepoPage.h"
#include "AddRepository.h"


// CConnectToRepo dialog

IMPLEMENT_DYNAMIC(CConnectToRepo, CFlatDlgBase)

CConnectToRepo::CConnectToRepo(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_CONNECT_REPO, pParent), m_needRefresh(false)
{

}

CConnectToRepo::~CConnectToRepo()
{
}

void CConnectToRepo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REPO, m_repoList);
	DDX_Control(pDX, IDC_ADDREPOSITORY, m_btnAddRepo);
}


BEGIN_MESSAGE_MAP(CConnectToRepo, CDialogEx)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_LBN_SELCHANGE(IDC_LIST_REPO, &CConnectToRepo::OnLbnSelchangeListRepo)
    ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_ADDREPOSITORY, &CConnectToRepo::OnBnClickedAddrepository)
END_MESSAGE_MAP()


// CConnectToRepo message handlers


BOOL CConnectToRepo::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here
	RefreshRepoList();
	m_btnAddRepo.SetColors(NX_NEXTLABS_STANDARD_GRAY, NX_NEXTLABS_STANDARD_BLACK, TRUE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectToRepo::RefreshRepoList()
{
	m_idxArr.clear();
	m_repoList.ResetContent();
	m_repoList.ReleaseItems();
	theApp.GetRepositoryArray(m_reposArr, true);
	int total_repo = (int)m_reposArr.size();

	m_repoList.SetItemDisplayStyle(NX_TWO_LINE_ITEM);

	m_DisplayName.RemoveAll();
	for (int i = 0; i< total_repo; i++)
	{
		auto a = m_reposArr[i];
		if (a.Type != NX::RmRepository::MYDRIVE)
		{
			int idx = m_repoList.AddItem(a.Name.c_str(), GetRepoIconID(a.Type), GetRepoSelIconID((int)a.Type), a.AccountName.c_str());
			m_DisplayName.Add(a.Name.c_str());
			m_idxArr.push_back(i);
		}
	}


	int idx = 0;
	m_repoList.SetCurSel(idx);
}


BOOL CConnectToRepo::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CFlatDlgBase::OnEraseBkgnd(pDC);
}


HBRUSH CConnectToRepo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


void CConnectToRepo::OnLbnSelchangeListRepo()
{
	int currIdx = m_repoList.GetCurSel();
	currIdx = (int)m_idxArr[currIdx];
	if(theApp.OpenRenameRepositoryUI(currIdx))
	{
		m_needRefresh = true;
		RefreshRepoList();
	}
}

void CConnectToRepo::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    CFlatDlgBase::OnLButtonUp(nFlags, point);
    CRect rect;
    m_addRepoStatic.GetWindowRect(&rect);
    ScreenToClient(&rect);
    if (rect.PtInRect(point))
    {

	}
}


void CConnectToRepo::OnBnClickedAddrepository()
{
	CAddRepository dlg(m_DisplayName, this);
	if (dlg.DoModal() == IDOK)
	{
		m_needRefresh = true;
		RefreshRepoList();
	}
}
