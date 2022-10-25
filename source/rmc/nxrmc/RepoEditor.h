#pragma once
#include "afxwin.h"
#include "ListBoxCH.h"


// CRepoEditor dialog

class CRepoEditor : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CRepoEditor)

public:
	CRepoEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRepoEditor();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REPO_EDITOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	RepositoryInfo m_repository;

public:
    afx_msg
    virtual BOOL OnInitDialog();

	BOOL m_bRenameOnly;
	BOOL m_bDisconnectOnly;
    ::CStatic m_currRepoName;
    ::CStatic m_emailAddress;
    ::CStatic m_repoPicture;
    afx_msg void OnBnClickedButtonDeleteRepo();
	afx_msg void OnBnClickedButtonUpdate();

	void SetCurrentRepository(RepositoryInfo &repository);

    CBtnClass m_disconnectBtn;
	CStatic m_repoTypeName;
	CEdit m_repoDispNameEdit;
	CBtnClass m_btnUpdate;
	CBtnClass m_btnCancel;
	CString m_strDisplayName;
	afx_msg void OnChangeEditDispname();
};
