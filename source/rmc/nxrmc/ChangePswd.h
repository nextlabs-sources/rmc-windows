#pragma once
#include "afxwin.h"


// CChangePswd dialog

class CChangePswd : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CChangePswd)

public:
	CChangePswd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CChangePswd();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHANGE_PSWD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnPaint();
    CEdit m_confirmPswd;
    CEdit m_newPswd;
    CEdit m_currPswd;
    afx_msg void OnBnClickedOk();
	CStatic m_emailAddr;
	virtual BOOL OnInitDialog();
	CButton m_btnChange;
	CButton m_btnCancel;
	CString m_strConfirmPswd;
	CString m_strCurrPswd;
	CString m_strNewPswd;
	afx_msg void OnChangeEditNewPswd();
	afx_msg void OnChangeEditCurrPswd();
	afx_msg void OnChangeEditConfirmPswd();
};
