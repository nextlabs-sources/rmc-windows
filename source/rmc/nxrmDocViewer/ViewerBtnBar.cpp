// ViewerBtnBar.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmDocViewer.h"
#include "ViewerBtnBar.h"
#include "afxdialogex.h"
#include "MainDocViewerDlg.h"


// CViewerBtnBar dialog

IMPLEMENT_DYNAMIC(CViewerBtnBar, CDialogEx)

CViewerBtnBar::CViewerBtnBar(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BUTTON_BAR, pParent)
{

}

CViewerBtnBar::~CViewerBtnBar()
{
}

void CViewerBtnBar::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ZOOM_IN, m_zoonInBtn);
    DDX_Control(pDX, IDC_ZOOMOUT, m_zoomOutBtn);
    DDX_Control(pDX, IDC_PAGEUP, m_pageUpBtn);
    DDX_Control(pDX, IDC_PAGEDOWN, m_pageDownBtn);
}


BEGIN_MESSAGE_MAP(CViewerBtnBar, CDialogEx)
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_PAGEUP, &CViewerBtnBar::OnBnClickedPageup)
    ON_BN_CLICKED(IDC_PAGEDOWN, &CViewerBtnBar::OnBnClickedPagedown)
    ON_BN_CLICKED(IDC_ZOOMOUT, &CViewerBtnBar::OnBnClickedZoomout)
    ON_BN_CLICKED(IDC_ZOOM_IN, &CViewerBtnBar::OnBnClickedZoomIn)
END_MESSAGE_MAP()


// CViewerBtnBar message handlers


BOOL CViewerBtnBar::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_toolTip.Create(this);

    m_toolTip.AddTool(&m_zoonInBtn, _T("Zoom in"));
    m_toolTip.AddTool(&m_zoomOutBtn, _T("Zoom out"));
    m_toolTip.AddTool(&m_pageUpBtn, _T("Page up"));
    m_toolTip.AddTool(&m_pageDownBtn, _T("Page down"));

    
    m_zoonInBtn.SetImages(IDB_PNG_ZOOMIN, 0, 0, m_bkColor);
    m_zoomOutBtn.SetImages(IDB_PNG_ZOOMOUT, 0, 0, m_bkColor);
    m_pageUpBtn.SetImages(IDB_PNG_PAGEUP, 0, 0, m_bkColor);
    m_pageDownBtn.SetImages(IDB_PNG_PAGEDOWN, 0, 0, m_bkColor);


    // TODO:  Add extra initialization here

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CViewerBtnBar::OnDestroy()
{
    CDialogEx::OnDestroy();
    delete this;
    // TODO: Add your message handler code here
}


BOOL CViewerBtnBar::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    m_toolTip.RelayEvent(pMsg);
    return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CViewerBtnBar::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    CRect rect;
    GetClientRect(&rect);
    HBRUSH hbr = CreateSolidBrush(m_bkColor);
    pDC->FillRect(&rect, CBrush::FromHandle(hbr));
    return 1;//CDialogEx::OnEraseBkgnd(pDC);
}


void CViewerBtnBar::OnBnClickedPageup()
{
    // TODO: Add your control notification handler code here
    CMainDocViewerDlg *pDlg = (CMainDocViewerDlg *)GetParent();
    pDlg->OnBnClickedPageUp();
}


void CViewerBtnBar::OnBnClickedPagedown()
{
    // TODO: Add your control notification handler code here
    CMainDocViewerDlg *pDlg = (CMainDocViewerDlg *)GetParent();
    pDlg->OnBnClickedPagedown();
}


void CViewerBtnBar::OnBnClickedZoomout()
{
    // TODO: Add your control notification handler code here
    CMainDocViewerDlg *pDlg = (CMainDocViewerDlg *)GetParent();
    pDlg->OnBnClickedZoomout();
}


void CViewerBtnBar::OnBnClickedZoomIn()
{
    // TODO: Add your control notification handler code here
    CMainDocViewerDlg *pDlg = (CMainDocViewerDlg *)GetParent();
    pDlg->OnBnClickedZoomIn();
}
