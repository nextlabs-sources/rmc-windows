#pragma once
#include "afxwin.h"
#include "BtnClass.h"


// CViewerBtnBar dialog

class CViewerBtnBar : public CDialogEx
{
	DECLARE_DYNAMIC(CViewerBtnBar)

public:
	CViewerBtnBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CViewerBtnBar();

    CToolTipCtrl m_toolTip;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BUTTON_BAR };
#endif

    const COLORREF m_bkColor= RGB(213, 213, 223);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    CBtnClass m_zoonInBtn;
    CBtnClass m_zoomOutBtn;
    CBtnClass m_pageUpBtn;
    CBtnClass m_pageDownBtn;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnBnClickedPageup();
    afx_msg void OnBnClickedPagedown();
    afx_msg void OnBnClickedZoomout();
    afx_msg void OnBnClickedZoomIn();
};
