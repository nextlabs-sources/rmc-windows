#pragma once
#include "afxwin.h"
#include "nxrmRMC.h"


// CRepoLabel dialog

class CRepoLabel : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CRepoLabel)

public:
	CRepoLabel(const RepositoryInfo &repoData, CWnd* pParent = NULL);   // standard constructor
	virtual ~CRepoLabel();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REPOSOTORY };
#endif

protected:
	CStatic m_repoIcon;
	CStatic m_repoName;
	CStatic m_repoEmail;
    CToolTipCtrl m_ToolTip;


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    RepositoryInfo m_repoData;
    HICON m_hIcon;

	void InitializeWindows(void);
public:
	void UpdateRepoInfo(const RepositoryInfo & repoData);

    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
