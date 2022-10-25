
// nxrmRMCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "nxrmRMCDlg.h"
#include "afxdialogex.h"
#include "nxrmc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void PackageDownloadCallBack(MSG wmdata)
{
	::SetTimer(wmdata.hwnd, wmdata.message, (UINT)wmdata.wParam, 0);
}

// CnxrmRMCDlg dialog


#define 	IDT_CHECKUPDATE_TIMER 		WM_USER + 200 
#define		IDT_INSTALLPACKAGE_TIMER	WM_USER + 201
CnxrmRMCDlg::CnxrmRMCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SPLASH, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_NXRMC);
}

void CnxrmRMCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CnxrmRMCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    //ON_BN_CLICKED(IDCANCEL, &CnxrmRMCDlg::OnBnClickedCancel)
    ON_WM_TIMER()
    ON_STN_CLICKED(IDC_SPLASH, &CnxrmRMCDlg::OnStnClickedSplash)
END_MESSAGE_MAP()


// CnxrmRMCDlg message handlers

BOOL CnxrmRMCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	//SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    SetTimer(1, 1000, 0);
	SetTimer(IDT_CHECKUPDATE_TIMER, 1000*60*60, 0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CnxrmRMCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CnxrmRMCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CnxrmRMCDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    //CDialogEx::OnCancel();
    ShowWindow(SW_HIDE);
    RmcMain(AfxGetInstanceHandle());
}


void CnxrmRMCDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
	NX::Result res = RESULT(0);
	std::wstring routerUrl = theApp.GetRouter();
	BOOL buptodate;
	std::wstring retstr;
	wstringstream txtOut;
	switch (nIDEvent)
	{
	case 1:
		KillTimer(1);
		theApp.m_pSession = std::shared_ptr<NX::RmSession>(NX::RmSession::Create(NX::RmRouter(L"SkyDRM", routerUrl), res));
		if (theApp.m_pSession == nullptr)
		{
			MessageBox(theApp.LoadStringW(IDS_COULDNOT_INIT), theApp.LoadStringW(IDS_APP_TITLE));
			CDialogEx::OnCancel();
		}
		else
		{
			OnBnClickedCancel();
		}
		break;
	case IDT_CHECKUPDATE_TIMER:
		retstr = theApp.CheckForUpdate(&buptodate);
		if (buptodate && !retstr.empty()) {
			KillTimer(IDT_CHECKUPDATE_TIMER);
			CString txtOut;
			txtOut.FormatMessage(IDS_FMT_NEW_AVAIL, retstr.c_str());
			txtOut += theApp.LoadStringW(IDS_DOWNLOAD_INSTALL)/*L"Do you want to download and install the new version?"*/;
			if (MessageBox(txtOut, theApp.LoadStringW(IDS_APP_TITLE), MB_YESNO|MB_ICONQUESTION) == IDYES) {
				MSG msg;
				msg.hwnd = GetSafeHwnd();
				msg.message = IDT_INSTALLPACKAGE_TIMER;
				msg.wParam = 1000;
				theApp.DownloadNewVersion(TRUE, PackageDownloadCallBack, &msg);
			}
		}
		break;
	case IDT_INSTALLPACKAGE_TIMER:
	{
		KillTimer(IDT_INSTALLPACKAGE_TIMER);
		CString txtOut = theApp.LoadStringW(IDS_NEW_VER_DOANLOADED);
		txtOut += theApp.LoadStringW(IDS_WANT_TO_INSTALL);
		if (MessageBox(txtOut, theApp.LoadStringW(IDS_APP_TITLE), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			theApp.InstallNewVersion();
		}
		break;
	}
	default:
		break;
	}

    CDialogEx::OnTimer(nIDEvent);
}


void CnxrmRMCDlg::OnStnClickedSplash()
{
    // TODO: Add your control notification handler code here
}
