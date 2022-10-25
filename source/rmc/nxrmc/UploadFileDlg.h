#pragma once
#include "afxwin.h"
#include "FlatDlgBase.h"
#include "BtnClass.h"


// CUploadFileDlg dialog

class CUploadFileDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CUploadFileDlg)

public:
	CUploadFileDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUploadFileDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_UPLOADFILE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	CString m_stcFileName;
	CString m_strLastModify;
	CString m_strFileName;
	CString m_strPath;
	CString m_strSize;
	CBtnClass m_btnCancel;
	CBtnClass m_btnProtect;
	virtual BOOL OnInitDialog();

	BOOL m_bUpdateOnly;
	afx_msg void OnClickedCancel();
};
