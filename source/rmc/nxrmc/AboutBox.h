#pragma once
#include "FlatDlgBase.h"
#include "afxwin.h"
#include "afxcmn.h"


// CAboutBox dialog

class CAboutBox : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CAboutBox)

public:
	CAboutBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAboutBox();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnPaint();
//    CStatic m_version;
    virtual BOOL OnInitDialog();
    CStatic m_versionLabel;
    CLinkCtrl m_nxRights;
    afx_msg void OnNMClickSyslinkNxlabs(NMHDR *pNMHDR, LRESULT *pResult);
};
