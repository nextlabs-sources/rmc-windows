
// nxrmviwdlg.h : header file
//

#pragma once


// CRmcViewerDlg dialog
class CRmcViewerDlg : public CDHtmlDialog
{
// Construction
public:
	CRmcViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NXRMVIW_DIALOG, IDH = IDR_HTML_NXRMVIW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnMenuFileOpen();
	afx_msg void OnMenuFileClose();
	afx_msg void OnMenuFilePrint();
	afx_msg void OnMenuFileShare();
	afx_msg void OnMenuFileDownload();
	afx_msg void OnMenuFileExit();
	afx_msg void OnMenuViewNextPage();
	afx_msg void OnMenuViewPreviousPage();
	afx_msg void OnMenuViewZoomIn();
	afx_msg void OnMenuViewZoomOut();
	afx_msg void OnMenuViewRotateClockwise();
	afx_msg void OnMenuViewRotateAntiClockwise();
	afx_msg void OnMenuViewProperty();
	afx_msg void OnMenuHelpViewHelp();
	afx_msg void OnMenuHelpAbout();

	virtual void OnNavigateError(LPDISPATCH pDisp, VARIANT* pvURL, VARIANT* pvFrame, VARIANT* pvStatusCode, VARIANT_BOOL* pvbCancel);

	STDMETHOD(ShowContextMenu)(
		DWORD dwID,
		POINT* ppt,
		IUnknown* pcmdtReserved,
		IDispatch* pdispReserved);
	STDMETHOD(GetHostInfo)(struct _DOCHOSTUIINFO * pDoc);

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
	DECLARE_EVENTSINK_MAP()

protected:
	void SavePosition();
	void LoadPosition(LPRECT rect, BOOL* pMaximized);
};
