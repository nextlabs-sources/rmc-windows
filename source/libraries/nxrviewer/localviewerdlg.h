#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

// CLocalViewer dialog

class CLocalViewerDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CLocalViewerDlg)

public:
	CLocalViewerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLocalViewerDlg();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
	STDMETHOD(ShowContextMenu)(
		DWORD dwID,
		POINT* ppt,
		IUnknown* pcmdtReserved,
		IDispatch* pdispReserved);
	STDMETHOD(GetHostInfo)(struct _DOCHOSTUIINFO * pDoc);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOCAL_DIALOG, IDH = IDR_HTML_LOCALVIEWER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
