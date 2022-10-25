#pragma once
#include "FlatDlgBase.h"
#include <string>
#include "afxwin.h"


// CEditProfileDlg dialog

class CEditProfileDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CEditProfileDlg)

public:
	CEditProfileDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditProfileDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MANAGE_PROFILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	std::wstring m_emailAddr;
public:
    virtual BOOL OnInitDialog();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CEdit m_dispName;
	CEdit m_emailEdit;
	CEdit m_phoneNumber;
	afx_msg void OnBnClickedOk();
	CString m_strDispName;
	afx_msg void OnChangeEditDisplayname();
	CButton m_btnUpdate;
};
