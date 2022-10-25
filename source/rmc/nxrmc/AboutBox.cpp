// AboutBox.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "AboutBox.h"
#include "afxdialogex.h"
#include "nxversion.h"



// CAboutBox dialog

IMPLEMENT_DYNAMIC(CAboutBox, CFlatDlgBase)

CAboutBox::CAboutBox(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_ABOUTBOX, pParent)
{

}

CAboutBox::~CAboutBox()
{
}

void CAboutBox::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RMC_VERSION, m_versionLabel);
    DDX_Control(pDX, IDC_SYSLINK_NXLABS, m_nxRights);
}


BEGIN_MESSAGE_MAP(CAboutBox, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_NXLABS, &CAboutBox::OnNMClickSyslinkNxlabs)
END_MESSAGE_MAP()


// CAboutBox message handlers


HBRUSH CAboutBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


void CAboutBox::OnPaint()
{
    CFlatDlgBase::OnPaint();
}


BOOL CAboutBox::OnInitDialog()
{
    CFlatDlgBase::OnInitDialog();

    // TODO:  Add extra initialization here
    CString versionStr;
    m_versionLabel.GetWindowText(versionStr);
    auto version = theApp.m_pSession->GetProduct().GetVersion();
    versionStr.Format(versionStr, version.c_str());
    m_versionLabel.SetWindowText(versionStr);
    CString versionString;
    versionString.LoadStringW(IDS_APP_VERSION);
    m_nxRights.SetWindowText(versionString);
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CAboutBox::OnNMClickSyslinkNxlabs(NMHDR *pNMHDR, LRESULT *pResult)
{
    ShellExecuteA(NULL, "open", "www.nextlabs.com", NULL, NULL, SW_SHOWNORMAL);
    *pResult = 0;
}
