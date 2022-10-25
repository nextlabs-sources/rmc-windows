#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ActivityLogDlg.h"
#include "FlatDlgBase.h"
#include "NxMFCListCtrl.h"
#include <string>
#include <vector>
// CActivityLogDlg dialog

typedef struct _File_Activity_Log_ {
	std::wstring	strOperation;
	__int64			lFileDate;
	std::wstring	strFileTime;
	std::wstring	strOperator;
	std::wstring	strDevice;
	std::wstring	strDeviceName;
	BOOL			bAllowed;
} FileActivity;

class CActivityLogDlg : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CActivityLogDlg)

	CImageList m_imagelist;
public:
	CActivityLogDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CActivityLogDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILEACTIVITY };
#endif
	std::vector<FileActivity> m_FileActivities;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CNxMFCListCtrl m_lstFileActivity;
	CStatic m_stcFileName;
	CString m_strFileName;
	virtual BOOL OnInitDialog();
};
