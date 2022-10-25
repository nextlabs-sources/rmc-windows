// ProfPage.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "ProfPage.h"
#include "afxdialogex.h"
#include "ChangePswd.h"
#include "RepoEditor.h"
#include "RepoPage.h"
#include "AboutBox.h"
#include "AddRepository.h"


// CProfPage dialog

IMPLEMENT_DYNAMIC(CProfPage, CFlatDlgBase)

CProfPage::CProfPage(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_PROFILE, pParent)
{

}

CProfPage::~CProfPage()
{
}

void CProfPage::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_REPO, m_RepoList);
    DDX_Control(pDX, IDC_STATIC_PAGE_TITLE, m_pageTitle);
    DDX_Control(pDX, IDC_STATIC_REPOS, m_connectedRepos);
}


BEGIN_MESSAGE_MAP(CProfPage, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
//    ON_COMMAND(IDOK, &CProfPage::OnIdok)
    //ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CProfPage::OnBnClickedButtonLogout)
    ON_BN_CLICKED(IDC_BUTTON_EDIT_REPO, &CProfPage::OnBnClickedButtonEditRepo)
    //ON_BN_CLICKED(IDC_BUTTON_CHANGE_PSWD, &CProfPage::OnBnClickedButtonChangePswd)
    ON_WM_DESTROY()
//    ON_BN_CLICKED(IDC_BUTTON_HELP, &CProfPage::OnBnClickedButtonHelp)
//    ON_BN_CLICKED(IDC_BUTTON_CONTACT, &CProfPage::OnBnClickedButtonContact)
//    ON_BN_CLICKED(IDC_BUTTON_CACHE, &CProfPage::OnBnClickedButtonCache)
//    ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CProfPage::OnBnClickedButtonAbout)
    ON_BN_CLICKED(IDC_BUTTON_ADD_REPO, &CProfPage::OnBnClickedButtonAddRepo)
END_MESSAGE_MAP()


// CProfPage message handlers


HBRUSH CProfPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


BOOL CProfPage::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    for (auto a : repoArray)
    {
        //m_RepoList.AddItem(a.captionText, a.nIcoSelID, a.emailAddress);
    }
    int idx = 0;
    m_RepoList.SetCurSel(idx);
    AddFontSize(30, m_pageTitle);
    AddFontSize(30, m_connectedRepos);
    

    return 0;
}


void CProfPage::OnPaint()
{
    CFlatDlgBase::OnPaint();
}


//void CProfPage::OnIdok()
//{
//    // TODO: Add your command handler code here
//}


//void CProfPage::OnBnClickedButtonLogout()
//{
//    // TODO: Add your control notification handler code here
//    theApp.m_dlgContainer.DestroyWindow();
//    ShowIntroWindow();
//}


void CProfPage::OnBnClickedButtonEditRepo()
{
    // TODO: Add your control notification handler code here
    CRepoEditor dlg(CWnd::FromHandle(theApp.m_mainhWnd));
    dlg.DoModal();

}


//void CProfPage::OnBnClickedButtonChangePswd()
//{
//    // TODO: Add your control notification handler code here
//    CChangePswd dlg(CWnd::FromHandle(theApp.m_mainhWnd));
//    dlg.DoModal();
//}


void CProfPage::OnDestroy()
{
    CFlatDlgBase::OnDestroy();
    delete this;
    // TODO: Add your message handler code here
}



//void CProfPage::OnBnClickedButtonHelp()
//{
//    // TODO: Add your control notification handler code here
//    MessageBox(L"Display help");
//}


//void CProfPage::OnBnClickedButtonContact()
//{
//    // TODO: Add your control notification handler code here
//    MessageBox(L"Display contact");
//}


//void CProfPage::OnBnClickedButtonCache()
//{
//    // TODO: Add your control notification handler code here
//    MessageBox(L"Clear cache");
//}


//void CProfPage::OnBnClickedButtonAbout()
//{
//    // TODO: Add your control notification handler code here
//    CAboutBox dlg(this);
//    dlg.DoModal();
//}







void CProfPage::OnBnClickedButtonAddRepo()
{
    // TODO: Add your control notification handler code here
    CAddRepository dlg(CWnd::FromHandle(theApp.m_mainhWnd));
    dlg.DoModal();
}
