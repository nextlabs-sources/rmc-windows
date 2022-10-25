#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "FlatDlgBase.h"


// CProjByOthersDlg dialog

class CProjByOthersDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CProjByOthersDlg)

public:
    CProjByOthersDlg(CProjData &prjData, CWnd * pParent = NULL);
	CProjByOthersDlg(CWnd * pParent = NULL);
	// standard constructor
	virtual ~CProjByOthersDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROJ_BY_OTHERS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CProjData m_projData;

	CStatic m_projName;
	CStatic m_invitedBy;
	CStatic m_owner;
	CStatic m_ownerName;
	CStatic m_invitorName;
public:
    virtual BOOL OnInitDialog();
	void UpdateProjectInfo(CProjData &prjData);
    void ProjectRefresh();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnNMClickSyslinkJoin(NMHDR *pNMHDR, LRESULT *pResult);
    COwnerDrawStatic m_userPics;
	afx_msg void OnPaint();
    afx_msg void OnBnClickedButtonMdViewMyDrivefile();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    CBtnClass m_btnAccept;
    CBtnClass m_btnDecline;
    afx_msg void OnBnClickedButtonDecline();
    afx_msg void OnBnClickedButtonAccept();

	std::wstring GetProjectID(void) { return m_projData.m_nID; }

    CToolTipCtrl m_ToolTip;

    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
