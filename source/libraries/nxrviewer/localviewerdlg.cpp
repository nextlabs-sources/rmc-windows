// localviewerdlg.cpp : implementation file
//

#include "stdafx.h"
#include "nxrviewer.h"
#include "localviewerdlg.h"


// CLocalViewer dialog

IMPLEMENT_DYNCREATE(CLocalViewerDlg, CDHtmlDialog)

CLocalViewerDlg::CLocalViewerDlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(IDD_LOCAL_DIALOG, IDR_HTML_LOCALVIEWER, pParent)
{

}

CLocalViewerDlg::~CLocalViewerDlg()
{
}

void CLocalViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BOOL CLocalViewerDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	// Avoid popping up windows
	m_pBrowserApp->put_Silent(VARIANT_TRUE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CLocalViewerDlg, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CLocalViewerDlg)
	//DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("MainLogo"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CLocalViewer message handlers

HRESULT CLocalViewerDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	//OnOK();
	MessageBox(L"Click logo");
	return S_OK;
}

HRESULT CLocalViewerDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	//OnCancel();
	return S_OK;
}

STDMETHODIMP CLocalViewerDlg::ShowContextMenu(
	DWORD dwID,
	POINT* ppt,
	IUnknown* pcmdtReserved,
	IDispatch* pdispReserved)
{
	return S_OK;
}

STDMETHODIMP CLocalViewerDlg::GetHostInfo(struct _DOCHOSTUIINFO * pDoc)
{
	// Disable selection
	static const DWORD flags = DOCHOSTUIFLAG_DISABLE_HELP_MENU
		// | DOCHOSTUIFLAG_DIALOG
		| DOCHOSTUIFLAG_FLAT_SCROLLBAR
		| DOCHOSTUIFLAG_NO3DOUTERBORDER
		| DOCHOSTUIFLAG_ENABLE_REDIRECT_NOTIFICATION
		| DOCHOSTUIFLAG_ENABLE_INPLACE_NAVIGATION
		| DOCHOSTUIFLAG_DPI_AWARE;

	CDHtmlDialog::GetHostInfo(pDoc);
	pDoc->dwFlags |= flags;
	return S_OK;
}
