
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "nxrmDocViewer.h"
#include "MainDocViewerDlg.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

    m_pDocViewer = new CMainDocViewerDlg();
    m_pDocViewer->Create(IDD_MAIN_DOC_VIEWER, this);
    m_pDocViewer->ShowWindow(SW_SHOW);


	// create a view to occupy the client area of the frame
	//if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
	//	CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	//{
	//	TRACE0("Failed to create view window\n");
	//	return -1;
	//}
 //   m_wndView.SetWindowPos(this, 0, 100, 100, 200, SWP_NOZORDER);
    m_hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
    SetIcon(m_hIcon, FALSE);
    SetIcon(m_hIcon, TRUE);
    SetMenu(NULL);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	//m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	//if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);

    m_pDocViewer->SetWindowPos(this, 0, 0, cx, cy, SWP_NOREPOSITION | SWP_NOZORDER);

    // TODO: Add your message handler code here
}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Add your message handler code here and/or call default
    _ASSERT(lpMMI != NULL);
    lpMMI->ptMinTrackSize.x = 800;
    lpMMI->ptMinTrackSize.y = 600;

    CFrameWnd::OnGetMinMaxInfo(lpMMI);
}


void CMainFrame::OnDestroy()
{
    CFrameWnd::OnDestroy();

    DeleteObject(m_hIcon);

    // TODO: Add your message handler code here
}
