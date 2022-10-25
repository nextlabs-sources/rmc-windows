#pragma once
#include "FlatDlgBase.h"
#include "afxwin.h"

// CUserEdit dialog

class CUserEdit : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CUserEdit)

public:
	CUserEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserEdit();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USER_EDIT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	
public:
    afx_msg void OnBnClickedButtonChangePswd();
    afx_msg void OnBnClickedButtonLogout();
    afx_msg void OnBnClickedButtonChangPicture();
	void RefreshUserName();
    BOOL OnInitDialog();
    bool m_bLogoff = false;
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnPaint();
    CBtnClass m_logoutBtn;
    afx_msg void OnBnClickedButtonManageprofile();
    CNxButton m_btnAddPhoto;
	CStatic m_emailAddr;
	bool m_bNeedRefresh;
	//CStatic m_userName;
	afx_msg void OnDestroy();
    CNxButton m_changPswd;
    CNxButton m_manageProf;
};
