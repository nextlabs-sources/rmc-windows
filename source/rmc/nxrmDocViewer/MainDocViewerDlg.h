#pragma once
#include "afxwin.h"
#include "FlatDlgBase.h"
#include "btnclass.h"
#include "CAcroAXDocShim.h"
#include "ViewerBtnBar.h"


// CMainDocViewerDlg dialog

class CMainDocViewerDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CMainDocViewerDlg)

public:
	CMainDocViewerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainDocViewerDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DOC_VIEWER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    int m_distanceToRightBorder;
    int m_distanceToOfflineBtn;
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    CStatic m_filePath;
    CStatic m_fileName;
    CAcroAXDocShim m_DocView;
    afx_msg
    void MoveButtonByDistance(CBtnClass &btn, int displacement);
    void OnSize(UINT nType, int cx, int cy);
    void OpenSharingDlg(HWND hWnd);
    void OpenProtectDlg(HWND hWnd);
    void OpenPropertyDlg(HWND hWnd);
    CBtnClass m_cancelBtn;
    CBtnClass m_faveriteBtn;
    CBtnClass m_offlineBtn;
    CBtnClass m_shareBtn;
    CBtnClass m_ProtectBtn;
    CBtnClass m_propertyBtn;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    CToolTipCtrl m_toolTip;
    afx_msg void OnBnClickedButtonClose();
    afx_msg void OnClickedButtonShare();
    afx_msg void OnClickedButtonProtect();
    afx_msg void OnClickedButtonProperties();
    afx_msg void OnClickedOffline();
    afx_msg void OnBnClickedBookmark();
    CBtnClass m_pageUp;
    CBtnClass m_pageDown;
    afx_msg void OnBnClickedPageUp();
    afx_msg void OnBnClickedPagedown();
    void OnBnClickedZoomout();
    void OnBnClickedZoomIn();
	// picture control for overlay image.
	CStatic m_iconOverlay;
    CViewerBtnBar *m_btnBar;
    CString m_filePathStr;
    CString m_fileNameStr;
};
