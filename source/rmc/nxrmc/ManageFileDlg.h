#pragma once
#include "NxButton.h"
#include "afxwin.h"
#include "resource.h"
#include "FlatDlgBase.h"

#include <string>
#include <vector>

// CManageFileDlg dialog

class CManageFileDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CManageFileDlg)

public:
	CManageFileDlg(CWnd* pParent = NULL, BOOL bPropertyOnly = FALSE);   // standard constructor
	virtual ~CManageFileDlg();

// Dialog Data
	BOOL m_bManage;
	BOOL m_bOwner;
	BOOL m_bRevoked;
	BOOL m_bDeleted;
	BOOL m_bViewOnly;
	BOOL m_bRepositoryFile;
    BOOL m_bProjectFile;
	int m_iSharedMembers;
	std::vector<std::wstring> m_fileRights;
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGEVAULT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL LoadRightsIcon(int idx, std::wstring rightstring);

	BOOL	m_bPropertyOnly;
	DECLARE_MESSAGE_MAP()
public:
	CNxButton m_btnRevoke;
	CString m_strFileName;
	CBtnClass m_btnRight1;
	CBtnClass m_btnRight2;
	CBtnClass m_btnRight4;
	CBtnClass m_btnRight3;
	CBtnClass m_btnRight5;
	virtual BOOL OnInitDialog();
	CString m_strSharedWithEmail;
	afx_msg void OnBnClickedBtnRevoke();
	CStatic m_stcShareWithText;
	CStatic m_stcRightText;
	CStatic m_stcShareWithEmail;
	afx_msg void OnBnClickedButtonManage();
	CNxButton m_btnManage;
};
