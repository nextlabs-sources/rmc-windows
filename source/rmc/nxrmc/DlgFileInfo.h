#pragma once
#include "NxButton.h"
#include "afxwin.h"
#include "resource.h"
#include "FlatDlgBase.h"

#include <string>
#include <vector>

// CDlgFileInfo dialog

class CDlgFileInfo : public CFlatDlgBase
{
	DECLARE_DYNAMIC(CDlgFileInfo)

public:
	CDlgFileInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFileInfo();

	// Dialog Data
	BOOL m_bOwner;
	BOOL m_bRevoked;
	BOOL m_bDeleted;
	BOOL m_bViewOnly;
	BOOL m_bRepositoryFile;
	BOOL m_bProjectFile;
	std::vector<std::wstring> m_fileRights;
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGEVAULT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL LoadRightsIcon(int idx, std::wstring rightstring);
	void MoveWindowPositionUp(CWnd * pwnd, int ndif);
	DECLARE_MESSAGE_MAP()
public:
	CNxButton m_btnRevoke;
	CBtnClass m_btnRight1;
	CBtnClass m_btnRight2;
	CBtnClass m_btnRight4;
	CBtnClass m_btnRight3;
	CBtnClass m_btnRight5;
	CString m_strFileModify;
	CString m_strFileName;
	CString m_strFilePath;
	CString m_strFileSize;
	virtual BOOL OnInitDialog();
	CStatic m_stcOriginalFileText;
	CStatic m_stcFilePath;
	CStatic m_stcRightText;
};
