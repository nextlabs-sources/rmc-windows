
// nxmaindlg.cpp : implementation file
//

#include "stdafx.h"
#include "nxrviewer.h"
#include "nxmaindlg.h"
#include "afxdialogex.h"

#include <Wininet.h>
#include <nx\winutil\process.h>
#include <nx\winutil\registry.h>
#include <nx\rmc\viewermgr.h>
#include <nx\rest\uri.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace NX;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CViewMainDlg dialog



CViewMainDlg::CViewMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_NXRVIEWER_DIALOG, pParent), m_pLocalViewer(NULL), m_pRemoteViewer(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CViewMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewMainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CViewMainDlg message handlers

BOOL CViewMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_pLocalViewer = new CLocalViewerDlg(this);
	m_pRemoteViewer = new CRemoteViewerDlg(this);
	m_pLocalViewer->Create(IDD_LOCAL_DIALOG, this);
	m_pRemoteViewer->Create(IDD_REMOTE_DIALOG, this);
	m_pLocalViewer->ShowWindow(SW_SHOW);
	m_pRemoteViewer->ShowWindow(SW_HIDE);

	RECT rect = { 0, 0, 0, 0 };
	LoadViewerPosition(&rect);
	MoveWindow(&rect, TRUE);

	GetClientRect(&rect);
	m_pLocalViewer->MoveWindow(&rect, TRUE);
	m_pRemoteViewer->MoveWindow(&rect, TRUE);


	// MessageBox(L"Debug", L"Debug", MB_OK | MB_ICONEXCLAMATION);

	BOOL bSucceed = FALSE;
	
	do {

		// First, try to load client key from current machine's store
		Result res = m_clientId.Load(true);
		if (!res) {
			// If there is no global client key, try to load it from current user's cert store
			res = m_clientId.Load(false);
			if (!res) {
				MessageBox(L"Fail to find client key", L"Error", MB_OK | MB_ICONERROR);
				break;
			}
		}

		const NX::win::ProcessEnvironment env(std::vector<std::pair<std::wstring, std::wstring>>(), false);
		const std::wstring& viewerType = env.GetVariable(NXRM_ENV_TYPE_NAME);
		const std::wstring& encodedParams = env.GetVariable(NXRM_ENV_VARIABLE_NAME);

		if (0 != _wcsicmp(viewerType.c_str(), L"Remote")) {
			MessageBox(L"Wrong input viewer type", L"Error", MB_OK | MB_ICONERROR);
			break;
		}

		if (encodedParams.empty()) {
			MessageBox(L"Fail to find SkyDRM Parameters", L"Error", MB_OK | MB_ICONERROR);
			break;
		}

		NX::RmRemoteViewerData vdata;
		res = vdata.FromBase64(std::string(encodedParams.begin(), encodedParams.end()), m_clientId.GetClientKey().data(), (ULONG)m_clientId.GetClientKey().size());
		if (!res) {
			MessageBox(L"Fail to load SkyDRM Parameters", L"Error", MB_OK | MB_ICONERROR);
			break;
		}

		NX::REST::Uri uri(vdata.GetUrl());
		const std::wstring sHost(uri.scheme() + L"://" + uri.host());
		
		const std::vector<std::wstring>& cookies = vdata.GetCookies();
		TRACE(L"Set cookies for host: %s\n", sHost.c_str());
		std::for_each(cookies.begin(), cookies.end(), [&](const std::wstring& cookie) {			
			DWORD flags = NX::iend_with<wchar_t>(cookie, L"HTTPONLY") ? INTERNET_COOKIE_HTTPONLY : 0;
			if (!InternetSetCookieExW(sHost.c_str(), NULL, cookie.c_str(), flags, 0)) {
				TRACE(L"Fail to set cookie (%d): %s\n", GetLastError(), cookie.c_str());
			}
		});

		bSucceed = TRUE;
		
		m_pRemoteViewer->Navigate(vdata.GetUrl().c_str());
		m_pLocalViewer->ShowWindow(SW_HIDE);
		m_pRemoteViewer->ShowWindow(SW_SHOW);

	} while (FALSE);

	if (!bSucceed) {
		CDialog::OnCancel();
	}

	BringWindowToTop();
	::SetWindowPos(this->GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CViewMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CViewMainDlg::OnPaint()
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
HCURSOR CViewMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CViewMainDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
}


void CViewMainDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	const RECT rect = { 0, 0, cx, cy };
	if (m_pLocalViewer) {
		m_pLocalViewer->MoveWindow(&rect, TRUE);
	}
	if (m_pRemoteViewer) {
		m_pRemoteViewer->MoveWindow(&rect, TRUE);
	}
}


void CViewMainDlg::OnDestroy()
{
	// TODO: Add your message handler code here
	SaveViewerPosition();

	if (m_pLocalViewer) {
		delete m_pLocalViewer;
		m_pLocalViewer = NULL;
	}
	if (m_pRemoteViewer) {
		delete m_pRemoteViewer;
		m_pRemoteViewer = NULL;
	}

	CDialogEx::OnDestroy();
}

void CViewMainDlg::SaveViewerPosition()
{
	NX::win::RegUserKey keyApp;
	if (IsIconic())//ignore minimize windows size
		return;
	Result res = keyApp.Create(L"SOFTWARE\\NextLabs\\SkyDRM\\RemoteViewer");
	if (!res) {
		NX::win::RegUserKey keyCompany;
		NX::win::RegUserKey keyProduct;
		res = keyCompany.Create(L"SOFTWARE\\NextLabs");
		if (!res)
			return;
		res = keyProduct.Create(L"SOFTWARE\\NextLabs\\SkyDRM");
		if (!res)
			return;
		res = keyApp.Create(L"SOFTWARE\\NextLabs\\SkyDRM\\RemoteViewer");
		if (!res)
			return;
	}
	RECT rect = { 0 };
	GetWindowRect(&rect);
	int width = rect.right - rect.left; width = (width <= 0) ? 600 : width;
	int height = rect.bottom - rect.top; height = (height <= 0) ? 480 : height;
	keyApp.WriteValue(L"Width", (unsigned int)width);
	keyApp.WriteValue(L"Height", (unsigned int)height);
}

void CViewMainDlg::LoadViewerPosition(LPRECT rect)
{
	int width = 0;
	int height = 0;
	NX::win::RegUserKey keyApp;
	Result res = keyApp.Open(L"SOFTWARE\\NextLabs\\SkyDRM\\RemoteViewer", true);
	if (res) {
		keyApp.ReadValue(L"Width", (unsigned int*)&width);
		keyApp.ReadValue(L"Height", (unsigned int*)&height);
	}

	width = (width <= 0) ? 600 : width;
	height = (height <= 0) ? 480 : height;

	const int maxCX = GetSystemMetrics(SM_CXSCREEN);
	const int maxCY = GetSystemMetrics(SM_CYSCREEN);
	width = min(maxCX, width);
	height = min(maxCY, height);

	rect->left = (maxCX - width) / 2;
	rect->top = (maxCY - height) / 2;
	rect->right = rect->left + width;
	rect->bottom = rect->top + height;
}