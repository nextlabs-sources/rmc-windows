#pragma once
#include "FlatDlgBase.h"
#include "afxwin.h"
#include "projdata.h"
#include "afxlinkctrl.h"


// COneProjDlg dialog

class COneProjDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(COneProjDlg)

public:
	//COneProjDlg(CWnd* pParent = NULL);
    COneProjDlg(CProjData & projData, CWnd * pParent = NULL);
	COneProjDlg(CWnd * pParent = NULL);
	// standard constructor
	virtual ~COneProjDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ONEPROJ };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CProjData m_projData;
	COwnerDrawStatic m_userPictures;
	CStatic m_projName;
	CStatic m_fileCount;

	CMFCLinkCtrl m_inviteLink;
public:
    afx_msg void OnDestroy();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    virtual BOOL OnInitDialog();
    afx_msg void OnStnClickedProjUsersPictures();
    afx_msg void OnBnClickedMfclinkInvite();
    afx_msg void OnBnClickedButtonTest();
    afx_msg void OnStnClickedFileCount();
    afx_msg void OnNMClickSyslinkInvite(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void UpdateProjectInfo(CProjData &projData);
	std::wstring GetProjectID(void) { return m_projData.m_nID; }
    void ProjectRefresh();

    CToolTipCtrl m_ToolTip;

    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
