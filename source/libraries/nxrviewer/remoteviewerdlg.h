#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

// CRemoteViewerDlg dialog

class CRemoteViewerDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CRemoteViewerDlg)

public:
	CRemoteViewerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRemoteViewerDlg();
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
	enum { IDD = IDD_REMOTE_DIALOG, IDH = IDR_HTML_REMOTEVIEWERDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
