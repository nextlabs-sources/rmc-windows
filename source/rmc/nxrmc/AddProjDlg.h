#pragma once
#include "ProjData.h"
#include "afxwin.h"
#include "BtnClass.h"
#include "FlatDlgBase.h"


// CAddProjDlg dialog

class CAddProjDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CAddProjDlg)

public:
	CAddProjDlg(AddDlgStyle dlgStyle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddProjDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADDPROJ };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    afx_msg void OnStnClickedStaticAddproj();
    CBtnClass m_viewAll;
    CStatic m_addProjImage;
    AddDlgStyle m_dlgStyle;
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    virtual BOOL OnInitDialog();
    CStatic m_viewAllText;
    CToolTipCtrl m_ToolTip;
    HBITMAP m_hbmp;
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnBnClickedViewAll();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
};
