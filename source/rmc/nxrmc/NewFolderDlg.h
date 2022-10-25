#pragma once
#include <vector>
#include <string>
#include "afxwin.h"
#include "BtnClass.h"

// NewFolderDlg dialog

class NewFolderDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(NewFolderDlg)

public:
	NewFolderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~NewFolderDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NEWFOLDER };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCreatefolder();
	CString m_strFolderName;
	std::vector<std::wstring> m_strExistFolders;
	CEdit m_edtFolderName;
	CBtnClass m_btnCreate;
	CBtnClass m_btnCancel;
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditFoldername();
};

