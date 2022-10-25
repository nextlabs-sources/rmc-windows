// remoteviewerdlg.cpp : implementation file
//

#include "stdafx.h"
#include "nxrviewer.h"
#include "remoteviewerdlg.h"


// CRemoteViewerDlg dialog

IMPLEMENT_DYNCREATE(CRemoteViewerDlg, CDHtmlDialog)

CRemoteViewerDlg::CRemoteViewerDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(IDD_REMOTE_DIALOG, IDR_HTML_REMOTEVIEWERDLG, pParent)
{

}

CRemoteViewerDlg::~CRemoteViewerDlg()
{
}

void CRemoteViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CRemoteViewerDlg::OnInitDialog()
{
	SetHostFlags(DOCHOSTUIFLAG_NO3DBORDER);
	CDHtmlDialog::OnInitDialog();
	// Avoid popping up windows
	m_pBrowserApp->put_Silent(VARIANT_TRUE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CRemoteViewerDlg, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CRemoteViewerDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CRemoteViewerDlg message handlers

HRESULT CRemoteViewerDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	//OnOK();
	return S_OK;
}

HRESULT CRemoteViewerDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	//OnCancel();
	return S_OK;
}

STDMETHODIMP CRemoteViewerDlg::ShowContextMenu(
	DWORD dwID,
	POINT* ppt,
	IUnknown* pcmdtReserved,
	IDispatch* pdispReserved)
{
	return S_OK;
}

STDMETHODIMP CRemoteViewerDlg::GetHostInfo(struct _DOCHOSTUIINFO * pDoc)
{
	// Disable selection
	static const DWORD flags = DOCHOSTUIFLAG_DISABLE_HELP_MENU
		| DOCHOSTUIFLAG_DIALOG
		| DOCHOSTUIFLAG_FLAT_SCROLLBAR
		| DOCHOSTUIFLAG_NO3DOUTERBORDER
		| DOCHOSTUIFLAG_ENABLE_REDIRECT_NOTIFICATION
		| DOCHOSTUIFLAG_ENABLE_INPLACE_NAVIGATION
		| DOCHOSTUIFLAG_DPI_AWARE;

	CDHtmlDialog::GetHostInfo(pDoc);
	pDoc->dwFlags |= flags;
	return S_OK;
}
