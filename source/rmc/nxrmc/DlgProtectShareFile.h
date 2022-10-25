#pragma once
#include "resource.h"
#include "afxwin.h"
#include "FlatDlgBase.h"
#include "NxButton.h"

// CDlgProtectShareFile dialog

class CDlgProtectShareFile : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CDlgProtectShareFile)

	BOOL m_bShareFile;
	BOOL m_bProtectFile;

	CString m_FilePath;
public:
	CDlgProtectShareFile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProtectShareFile();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROSHARELOCALFILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	CString m_strFileSize;
	CString m_strFileName;
//	CString m_strFileDate;
	afx_msg void OnBnClickedProtect();
	afx_msg void OnBnClickedShared();
	virtual BOOL OnInitDialog();
	CNxButton m_btnProtect;
	CNxButton m_btnShare;
	CStatic m_stcFileName;
//	CStatic m_stcFileDate;
//	CStatic m_stcFileSize;
};
