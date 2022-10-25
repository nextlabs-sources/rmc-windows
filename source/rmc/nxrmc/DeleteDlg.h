#pragma once
#include "resource.h"
#include "afxwin.h"
#include "FlatDlgBase.h"
#include "NxButton.h"

// DeleteDlg dialog

class CDeleteDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CDeleteDlg)

public:
	CDeleteDlg(BOOL bdirectory = FALSE, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDeleteDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DELETE };
#endif

protected:
	BOOL m_bIsDirectory;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDelete();
	CStatic m_stcFileDate;
	CString m_strFileDate;
	CString m_strFileName;
	CStatic m_stcFileSize;
	CString m_strFileSize;
	CStatic m_stcLastModify;
	CStatic m_stcSize;
	CNxButton m_btnCancel;
	CNxButton m_btnDelete;
	virtual BOOL OnInitDialog();
	CStatic m_stcFileName;
	afx_msg void OnBnClickedCancel();
};
