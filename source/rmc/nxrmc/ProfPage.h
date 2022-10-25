#pragma once
#include "FlatDlgBase.h"
#include "afxwin.h"
#include "ListBoxCH.h"


// CProfPage dialog

class CProfPage : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CProfPage)

public:
	CProfPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProfPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROFILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
//    afx_msg void OnIdok();
    afx_msg void OnBnClickedButtonLogout();
    afx_msg void OnBnClickedButtonEditRepo();
    afx_msg void OnBnClickedButtonChangePswd();
    afx_msg void OnDestroy();
    CListBoxCH m_RepoList;
//    afx_msg void OnBnClickedButtonHelp();
//    afx_msg void OnBnClickedButtonContact();
//    afx_msg void OnBnClickedButtonCache();
//    afx_msg void OnBnClickedButtonAbout();
    ::CStatic m_pageTitle;
    ::CStatic m_connectedRepos;
    afx_msg void OnBnClickedButtonEditRepo3();
    afx_msg void OnBnClickedButtonAddRepo();
};
