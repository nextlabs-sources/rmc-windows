
// nxrmviwdlg.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmviw.h"
#include "nxrmviwdlg.h"
#include "afxdialogex.h"

#include <exdispid.h>


#include <nx\common\result.h>
#include <nx\common\stringex.h>
#include <nx\winutil\registry.h>

using namespace NX;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define MAINFRAME_MIN_WIDTH		640
#define MAINFRAME_MIN_HEIGHT	480


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


// CRmcViewerDlg dialog

BEGIN_DHTML_EVENT_MAP(CRmcViewerDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()


CRmcViewerDlg::CRmcViewerDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(IDD_NXRMVIW_DIALOG, IDR_HTML_NXRMVIW_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRmcViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRmcViewerDlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPENFILE, &CRmcViewerDlg::OnMenuFileOpen)
	ON_COMMAND(ID_FILE_CLOSEFILE, &CRmcViewerDlg::OnMenuFileClose)
	ON_COMMAND(ID_FILE_PRINTFILE, &CRmcViewerDlg::OnMenuFilePrint)
	ON_COMMAND(ID_FILE_SHARE, &CRmcViewerDlg::OnMenuFileShare)
	ON_COMMAND(ID_FILE_DOWNLOAD, &CRmcViewerDlg::OnMenuFileDownload)
	ON_COMMAND(ID_FILE_EXIT, &CRmcViewerDlg::OnMenuFileExit)
	ON_COMMAND(ID_VIEW_NEXTPAGE, &CRmcViewerDlg::OnMenuViewNextPage)
	ON_COMMAND(ID_VIEW_PREVIOUSPAGE, &CRmcViewerDlg::OnMenuViewPreviousPage)
	ON_COMMAND(ID_VIEW_ZOOMIN, &CRmcViewerDlg::OnMenuViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, &CRmcViewerDlg::OnMenuViewZoomOut)
	ON_COMMAND(ID_VIEW_ROTATECLOCKWISE, &CRmcViewerDlg::OnMenuViewRotateClockwise)
	ON_COMMAND(ID_VIEW_ROTATEANTICLOCKWISE, &CRmcViewerDlg::OnMenuViewRotateAntiClockwise)
	ON_COMMAND(ID_VIEW_PROPERTY, &CRmcViewerDlg::OnMenuViewProperty)
	ON_COMMAND(ID_HELP_VIEWHELP, &CRmcViewerDlg::OnMenuHelpViewHelp)
	ON_COMMAND(ID_HELP_ABOUT, &CRmcViewerDlg::OnMenuHelpAbout)
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CRmcViewerDlg, CDHtmlDialog)
	ON_EVENT(CRmcViewerDlg, AFX_IDC_BROWSER, DISPID_NAVIGATEERROR, OnNavigateError, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

// CRmcViewerDlg message handlers

BOOL CRmcViewerDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

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

	// Restore position
	RECT rect = { 0 };
	BOOL bMaximized = FALSE;
	LoadPosition(&rect, &bMaximized);
	MoveWindow(&rect, TRUE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRmcViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDHtmlDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRmcViewerDlg::OnPaint()
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
		CDHtmlDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRmcViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

STDMETHODIMP CRmcViewerDlg::ShowContextMenu(
	DWORD dwID,
	POINT* ppt,
	IUnknown* pcmdtReserved,
	IDispatch* pdispReserved)
{
	// Disable explorer context menu
	return S_OK;
}

STDMETHODIMP CRmcViewerDlg::GetHostInfo(struct _DOCHOSTUIINFO * pDoc)
{
	// Disable selection
	static const DWORD flags = DOCHOSTUIFLAG_DISABLE_HELP_MENU
		| DOCHOSTUIFLAG_DIALOG
		| DOCHOSTUIFLAG_FLAT_SCROLLBAR
		| DOCHOSTUIFLAG_NO3DOUTERBORDER
		| DOCHOSTUIFLAG_ENABLE_REDIRECT_NOTIFICATION
		| DOCHOSTUIFLAG_DPI_AWARE;

	CDHtmlDialog::GetHostInfo(pDoc);
	pDoc->dwFlags |= flags;
	return S_OK;
}

void CRmcViewerDlg::OnNavigateError(LPDISPATCH pDisp, VARIANT* pvURL, VARIANT* pvFrame, VARIANT* pvStatusCode, VARIANT_BOOL* pvbCancel)
{
	MessageBox(L"Nevigate Error", L"Error");
	LoadFromResource(IDR_HTML_NXRMVIW_DIALOG);
}


HRESULT CRmcViewerDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CRmcViewerDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}

void CRmcViewerDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	// Min size is 640 * 600
	const int nWidth = pRect->right - pRect->left;
	const int nHeight = pRect->bottom - pRect->top;

	if (nWidth < MAINFRAME_MIN_WIDTH) {
		if (WMSZ_BOTTOMLEFT == fwSide || WMSZ_LEFT == fwSide || WMSZ_TOPLEFT == fwSide) {
			pRect->left = pRect->right - MAINFRAME_MIN_WIDTH;
		}
		else {
			pRect->right = pRect->left + MAINFRAME_MIN_WIDTH;
		}
	}

	if (nHeight < MAINFRAME_MIN_HEIGHT) {
		if (WMSZ_BOTTOM == fwSide || WMSZ_BOTTOMLEFT == fwSide || WMSZ_BOTTOMRIGHT == fwSide) {
			pRect->bottom = pRect->top + MAINFRAME_MIN_WIDTH;
		}
		else {
			pRect->top = pRect->bottom - MAINFRAME_MIN_WIDTH;
		}
	}

	CDHtmlDialog::OnSizing(fwSide, pRect);
}

void CRmcViewerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDHtmlDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (SIZE_MAXIMIZED == nType || SIZE_RESTORED == nType) {
		NX::win::RegUserKey keyApp;
		Result res = keyApp.Create(L"SOFTWARE\\NextLabs\\SkyDRM\\Viewer");
		if (!res) {
			NX::win::RegUserKey keyCompany;
			NX::win::RegUserKey keyProduct;
			res = keyCompany.Create(L"SOFTWARE\\NextLabs");
			if (!res)
				return;
			res = keyProduct.Create(L"SOFTWARE\\NextLabs\\SkyDRM");
			if (!res)
				return;
			res = keyApp.Create(L"SOFTWARE\\NextLabs\\SkyDRM\\Viewer");
			if (!res)
				return;
		}
		UINT bMaximized = (SIZE_MAXIMIZED == nType) ? 1 : 0;
		keyApp.WriteValue(L"Maximized", bMaximized);
		if (SIZE_RESTORED == nType)
			SavePosition();
	}
}

void CRmcViewerDlg::OnMenuFileOpen()
{
}

void CRmcViewerDlg::OnMenuFileClose()
{
	LoadFromResource(IDR_HTML_NXRMVIW_DIALOG);
}

void CRmcViewerDlg::OnMenuFilePrint()
{
}

void CRmcViewerDlg::OnMenuFileShare()
{
}

void CRmcViewerDlg::OnMenuFileDownload()
{
}

void CRmcViewerDlg::OnMenuFileExit()
{
	CDHtmlDialog::OnOK();
}

void CRmcViewerDlg::OnMenuViewNextPage()
{
}

void CRmcViewerDlg::OnMenuViewPreviousPage()
{
}

void CRmcViewerDlg::OnMenuViewZoomIn()
{
}

void CRmcViewerDlg::OnMenuViewZoomOut()
{
}

void CRmcViewerDlg::OnMenuViewRotateClockwise()
{
}

void CRmcViewerDlg::OnMenuViewRotateAntiClockwise()
{
}

void CRmcViewerDlg::OnMenuViewProperty()
{
}

void CRmcViewerDlg::OnMenuHelpViewHelp()
{
}

void CRmcViewerDlg::OnMenuHelpAbout()
{
}

void CRmcViewerDlg::SavePosition()
{
	NX::win::RegUserKey keyApp;
	Result res = keyApp.Create(L"SOFTWARE\\NextLabs\\SkyDRM\\Viewer");
	if (!res) {
		NX::win::RegUserKey keyCompany;
		NX::win::RegUserKey keyProduct;
		res = keyCompany.Create(L"SOFTWARE\\NextLabs");
		if (!res)
			return;
		res = keyProduct.Create(L"SOFTWARE\\NextLabs\\SkyDRM");
		if (!res)
			return;
		res = keyApp.Create(L"SOFTWARE\\NextLabs\\SkyDRM\\Viewer");
		if (!res)
			return;
	}

	RECT rect = { 0 };
	GetWindowRect(&rect);
	int width = rect.right - rect.left; width = (width <= 0) ? MAINFRAME_MIN_WIDTH : width;
	int height = rect.bottom - rect.top; height = (height <= 0) ? MAINFRAME_MIN_HEIGHT : height;
	keyApp.WriteValue(L"Width", (unsigned int)width);
	keyApp.WriteValue(L"Height", (unsigned int)height);
}

void CRmcViewerDlg::LoadPosition(LPRECT pRect, BOOL* pMaximized)
{
	int width = 0;
	int height = 0;
	int maximized = 0;
	NX::win::RegUserKey keyApp;
	Result res = keyApp.Open(L"SOFTWARE\\NextLabs\\SkyDRM\\Viewer", true);
	if (res) {
		keyApp.ReadValue(L"Width", (unsigned int*)&width);
		keyApp.ReadValue(L"Height", (unsigned int*)&height);
		res = keyApp.ReadValue(L"Maximized", (unsigned int*)&maximized);
		if (!res)
			maximized = 0;
	}

	width = (width < MAINFRAME_MIN_WIDTH) ? MAINFRAME_MIN_WIDTH : width;
	height = (height < MAINFRAME_MIN_HEIGHT) ? MAINFRAME_MIN_HEIGHT : height;

	const int maxCX = GetSystemMetrics(SM_CXSCREEN);
	const int maxCY = GetSystemMetrics(SM_CYSCREEN);
	width = min(maxCX, width);
	height = min(maxCY, height);

	*pMaximized = (1 == maximized) ? TRUE : FALSE;
	pRect->left = (maxCX - width) / 2;
	pRect->top = (maxCY - height) / 2;
	pRect->right = pRect->left + width;
	pRect->bottom = pRect->top + height;
}
