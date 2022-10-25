// MainDocViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmDocViewer.h"
#include "MainDocViewerDlg.h"
#include "afxdialogex.h"
#include "commondlg.hpp"

//#include <nx\rmc\session.h>
//#include <nx\common\stringex.h>
//#include <nx\common\time.h>



// CMainDocViewerDlg dialog


IMPLEMENT_DYNAMIC(CMainDocViewerDlg, CFlatDlgBase)

CMainDocViewerDlg::CMainDocViewerDlg(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_MAIN_DOC_VIEWER, pParent)
    , m_btnBar(NULL)
{

}

CMainDocViewerDlg::~CMainDocViewerDlg()
{
}

void CMainDocViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_FILEPATH, m_filePath);
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_fileName);
	DDX_Control(pDX, IDC_BOOKMARK, m_faveriteBtn);
	DDX_Control(pDX, IDC_OFFLINE, m_offlineBtn);
	DDX_Control(pDX, IDC_BUTTON_SHARE, m_shareBtn);
	DDX_Control(pDX, IDC_BUTTON_PROTECT, m_ProtectBtn);
	DDX_Control(pDX, IDC_BUTTON_PROPERTIES, m_propertyBtn);
	DDX_Control(pDX, IDC_PICCNTL_STATIC, m_iconOverlay);
}


BEGIN_MESSAGE_MAP(CMainDocViewerDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CMainDocViewerDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_SHARE, &CMainDocViewerDlg::OnClickedButtonShare)
	ON_BN_CLICKED(IDC_BUTTON_PROTECT, &CMainDocViewerDlg::OnClickedButtonProtect)
	ON_BN_CLICKED(IDC_BUTTON_PROPERTIES, &CMainDocViewerDlg::OnClickedButtonProperties)
	ON_BN_CLICKED(IDC_OFFLINE, &CMainDocViewerDlg::OnClickedOffline)
	ON_BN_CLICKED(IDC_BOOKMARK, &CMainDocViewerDlg::OnBnClickedBookmark)
END_MESSAGE_MAP()


// CMainDocViewerDlg message handlers

const int PDF_CTRL_ID = 280;


static void DoEvents(HWND hWnd_i)
{
    MSG stMsg = { 0 };
    while (PeekMessage(&stMsg, hWnd_i, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&stMsg);
        DispatchMessage(&stMsg);
    }
}

BOOL CMainDocViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_toolTip.Create(this);

	AddFontSize(30, m_filePath, false);
	AddFontSize(70, m_fileName, false);
	//m_toolTip.AddTool(&m_cancelBtn, _T("Close close window go back to main panel"));
	m_toolTip.AddTool(&m_faveriteBtn, _T("Add this file to faverite"));
	m_toolTip.AddTool(&m_offlineBtn, _T("Add to offline"));
	m_toolTip.AddTool(&m_shareBtn, _T("Share this file"));
	m_toolTip.AddTool(&m_ProtectBtn, _T("Protect this file"));
	m_toolTip.AddTool(&m_propertyBtn, _T("Show properties of this file"));

	m_faveriteBtn.SetImages(IDB_PNG_FAVORED_AVAIL, 0, 0, RGB(0, 0, 0));
	m_offlineBtn.SetImages(IDB_PNG_OFFLINE_AVAIL, 0, 0, RGB(0, 0, 0));
	m_shareBtn.SetImages(IDB_PNG_FUNC_SHARE_WHITE, 0, 0, RGB(0, 0, 0));
	m_ProtectBtn.SetImages(IDB_PNG_FUNC_PROTECT_WHITE, 0, 0, RGB(0, 0, 0));
	m_propertyBtn.SetImages(IDB_PNG_FUNC_PROPERTY_WHITE, 0, 0, RGB(0, 0, 0));


	CRect rcBtn;
	m_propertyBtn.GetWindowRect(&rcBtn);
	ScreenToClient(rcBtn);
	CRect rcClient;
	GetClientRect(&rcClient);
	m_distanceToRightBorder = rcClient.right - rcBtn.right;
	m_propertyBtn.GetWindowRect(&rcBtn);

	m_offlineBtn.GetWindowRect(&rcBtn);
	ScreenToClient(rcBtn);
	m_distanceToOfflineBtn = rcBtn.left - rcClient.left;

	CString cmd = theApp.m_lpCmdLine;

	LPWSTR *szArglist = NULL;
	int nArgs;
	if (cmd.GetLength())
	{
		szArglist = CommandLineToArgvW(cmd, &nArgs);
	}
	if (NULL == szArglist)
	{
		//MessageBox(L"No file parameter for Doc viewer to read");
		//GetParent()->DestroyWindow();
		//return FALSE;
	}
	else
	{
		CString fullFilePath;
		if (nArgs > 0)
		{
			fullFilePath = szArglist[0];
			int pos = fullFilePath.ReverseFind(L'\\');
			if (pos != -1)
			{
				m_filePathStr = fullFilePath.Left(pos);
				m_fileNameStr = fullFilePath.GetBuffer() + pos;
				m_fileNameStr = m_fileNameStr.TrimLeft(L'\\');

				m_filePath.SetWindowText(m_filePathStr);
				m_fileName.SetWindowText(m_fileNameStr);
			}
			else
			{
				m_filePath.SetWindowText(L"");
				m_fileName.SetWindowText(fullFilePath);
                m_fileNameStr = fullFilePath;
			}
		}
		LocalFree(szArglist);


		if (!m_DocView.Create(L"AdobepdfView", WS_CHILD | WS_VISIBLE | WS_DISABLED, CRect(0, 0, 0, 0), this, PDF_CTRL_ID)){
			AfxMessageBox(L"Adobe reader is not installed!");
            GetParent()->DestroyWindow();
            return FALSE;
		}
		if (!m_DocView.LoadFile(fullFilePath)) {
			AfxMessageBox(L"Fail to load document!");
            //GetParent()->DestroyWindow();
            //return FALSE;
		}
		else
		{
			m_DocView.setLayoutMode(L"SinglePage");
			m_DocView.setPageMode(L"none");
			m_DocView.setView(L"Fit");
			m_DocView.setShowScrollbars(FALSE);
			m_DocView.setShowScrollbars(FALSE);
			m_DocView.gotoFirstPage();
		}
    }

    m_btnBar = new CViewerBtnBar();
    m_btnBar->Create(IDD_BUTTON_BAR, this);
    m_btnBar->ShowWindow(SW_SHOW);

    DoEvents(GetSafeHwnd());
    InvalidateRect(NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CMainDocViewerDlg::OnDestroy()
{
    CDialogEx::OnDestroy();
    delete this;
    // TODO: Add your message handler code here
}


BOOL CMainDocViewerDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcDocView;
    m_iconOverlay.GetWindowRect(&rcDocView);
    ScreenToClient(&rcDocView);
    CBrush *br;
    br = CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH));
    rcClient.bottom = rcDocView.top;
    pDC->FillRect(&rcClient, br);
    return TRUE;
}


HBRUSH CMainDocViewerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr =(HBRUSH)GetStockObject(BLACK_BRUSH);

    pDC->SetBkColor(RGB(0,0,0));

    // TODO:  Change any attributes of the DC here
    if (nCtlColor == IDC_STATIC_FILEPATH)
    {
        pDC->SetTextColor(RGB(128, 128, 128));
    }
    else
    {
        pDC->SetTextColor(RGB(255, 255, 255));
    }

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}



void CMainDocViewerDlg::MoveButtonByDistance(CBtnClass &btn, int displacement)
{
    CRect rcBtn;
    btn.GetWindowRect(&rcBtn);
    ScreenToClient(rcBtn);
    btn.SetWindowPos(this, rcBtn.left + displacement, rcBtn.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

}

void CMainDocViewerDlg::OnSize(UINT nType, int cx, int cy)
{
    CFlatDlgBase::OnSize(nType, cx, cy);
    CRect rcDocView;
    if (IsWindow(m_hWnd) && IsWindow(m_iconOverlay.m_hWnd))
    {
        m_iconOverlay.GetWindowRect(&rcDocView);
        ScreenToClient(&rcDocView);

        m_iconOverlay.SetWindowPos(this, 0, rcDocView.top, cx, cy, SWP_NOREPOSITION | SWP_NOZORDER | SWP_SHOWWINDOW);

		rcDocView.right = cx;
		rcDocView.bottom = cy;
		if (m_DocView.GetSafeHwnd())
		{
			m_DocView.MoveWindow(rcDocView);
		}

        CRect rcBtn;
        m_propertyBtn.GetWindowRect(&rcBtn);
        ScreenToClient(rcBtn);
        CRect rcClient;
        GetClientRect(&rcClient);

        int currBtnDistance = rcClient.right - rcBtn.right;
        int  displacement = currBtnDistance - m_distanceToRightBorder;


        m_offlineBtn.ShowWindow(SW_HIDE);
        m_faveriteBtn.ShowWindow(SW_HIDE);
        if (displacement > 0)
        {
            MoveButtonByDistance(m_propertyBtn, displacement);
            MoveButtonByDistance(m_ProtectBtn, displacement);
            MoveButtonByDistance(m_shareBtn, displacement);

        }
        else
        {
            MoveButtonByDistance(m_shareBtn, displacement);
            MoveButtonByDistance(m_ProtectBtn, displacement);
            MoveButtonByDistance(m_propertyBtn, displacement);
        }

        m_shareBtn.GetWindowRect(&rcBtn);
        ScreenToClient(rcBtn);

        CRect rcOffLineBtn;
        m_offlineBtn.GetWindowRect(&rcOffLineBtn);
        ScreenToClient(rcOffLineBtn);


        auto favoriteDelta = rcBtn.left - (rcOffLineBtn.right + rcOffLineBtn.Width() / 2);

        if (favoriteDelta > 0)
        {
            favoriteDelta = (rcOffLineBtn.left + favoriteDelta > m_distanceToOfflineBtn) ? m_distanceToOfflineBtn - rcOffLineBtn.left : favoriteDelta;
            MoveButtonByDistance(m_offlineBtn, favoriteDelta);
            MoveButtonByDistance(m_faveriteBtn, favoriteDelta);

        } 
        else 
        {
            MoveButtonByDistance(m_faveriteBtn, favoriteDelta);
            MoveButtonByDistance(m_offlineBtn, favoriteDelta);
        }

        m_offlineBtn.ShowWindow(SW_SHOW);
        m_faveriteBtn.ShowWindow(SW_SHOW);
        CRect rcBar;
        m_btnBar->GetWindowRect(&rcBar);
        ScreenToClient(&rcBar);

        m_btnBar->SetWindowPos(&CWnd::wndTop, cx - rcBar.Width(), rcDocView.top + rcBar.Height() / 3, rcBar.Width(), rcBar.Height(), SWP_NOSIZE | SWP_SHOWWINDOW);

    }
}


void CMainDocViewerDlg::OpenSharingDlg(HWND hWnd)
{
    // TODO: Add your control notification handler code here
    NXSharingData dataIn = { true, true, true, true, true, true, true, DLGTYPE_SHARING, };
    NXSharingData dataOut;
    auto fname = m_filePathStr + L"\\" + m_fileNameStr;
	LPWSTR pAddresses;
    if (!RmuShowShareDialog(hWnd, fname, dataIn, &dataOut, &pAddresses))
    {

    }
}


void CMainDocViewerDlg::OpenProtectDlg(HWND hWnd)
{
    // TODO: Add your control notification handler code here
    NXSharingData dataIn = { true, true, true, true, true, true, true, DLGTYPE_PROTECT, };
    NXSharingData dataOut;
    auto fname = m_filePathStr + L"\\" + m_fileNameStr;
	LPWSTR pAddresses;
    if (!RmuShowShareDialog(hWnd, fname, dataIn, &dataOut, &pAddresses))
    {

    }
}


void CMainDocViewerDlg::OpenPropertyDlg(HWND hWnd)
{
    NXSharingData dataIn = { true, true, true, true, true, true, true, DLGTYPE_SHARING,0,0,
    { L"evan_li@yahoo.com",L"eli@nextlabs.com" } };
    NXSharingData dataOut;
    auto fname = m_filePathStr + L"\\" + m_fileNameStr;
    if (!RmuShowFilePropertiesDialog(hWnd, fname, dataIn, dataOut))
    {

    }
}



BOOL CMainDocViewerDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    m_toolTip.RelayEvent(pMsg);
    return CFlatDlgBase::PreTranslateMessage(pMsg);
}


void CMainDocViewerDlg::OnBnClickedButtonClose()
{
    // TODO: Add your control notification handler code here
    MessageBox(L"Close This window, bring up the control window");
}


void CMainDocViewerDlg::OnClickedButtonShare()
{
    // TODO: Add your control notification handler code here
    OpenSharingDlg(GetSafeHwnd());
}


void CMainDocViewerDlg::OnClickedButtonProtect()
{
    // TODO: Add your control notification handler code here
    OpenProtectDlg(GetSafeHwnd());
}


void CMainDocViewerDlg::OnClickedButtonProperties()
{
    // TODO: Add your control notification handler code here
    OpenPropertyDlg(GetSafeHwnd());
}


void CMainDocViewerDlg::OnClickedOffline()
{
    // TODO: Add your control notification handler code here
    MessageBox(L"Make this file offline");
}


void CMainDocViewerDlg::OnBnClickedBookmark()
{
    // TODO: Add your control notification handler code here
    MessageBox(L"Put this file into favorite list");
}


void CMainDocViewerDlg::OnBnClickedPageUp()
{
	if (m_DocView.GetSafeHwnd())
	{

		m_DocView.gotoPreviousPage();
	}
    // TODO: Add your control notification handler code here
}


void CMainDocViewerDlg::OnBnClickedPagedown()
{
	if (m_DocView.GetSafeHwnd())
	{
		m_DocView.gotoNextPage();
	}
    // TODO: Add your control notification handler code here
}


void CMainDocViewerDlg::OnBnClickedZoomout()
{
    // TODO: Add your control notification handler code here
    MessageBox(L"zoom out");
}


void CMainDocViewerDlg::OnBnClickedZoomIn()
{
    // TODO: Add your control notification handler code here
    MessageBox(L"zoom in");
}
